/**
  ******************************************************************************
  * STM32L4 Bootloader
  ******************************************************************************
  * @author Akos Pasztor
  * @file   bootloader.c
  * @brief  Bootloader implementation
  *	        This file contains the functions of the bootloader. The bootloader 
  *	        implementation uses the official HAL library of ST.
  * @see    Please refer to README for detailed information.
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

#include "stm32l4xx.h"
#include "bootloader.h"

/* Private typedef -----------------------------------------------------------*/
typedef void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
static uint32_t flash_ptr = APP_ADDRESS;


/*** Check if application fits into user flash ********************************/
uint8_t Bootloader_CheckSize(uint32_t appsize)
{
    return ((FLASH_BASE + FLASH_SIZE - APP_ADDRESS) >= appsize) ? BL_OK : BL_SIZE_ERROR;
}

/*** Erase flash **************************************************************/
uint8_t Bootloader_Erase(void)
{
    uint32_t NbrOfPages = 0;
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();

    /* Get the number of page to  erase */
    NbrOfPages = (FLASH_BASE + FLASH_SIZE);
    NbrOfPages = (NbrOfPages - APP_ADDRESS) / FLASH_PAGE_SIZE;

    if(NbrOfPages > FLASH_PAGE_NBPERBANK)
    {
        pEraseInit.Banks = FLASH_BANK_1;
        pEraseInit.NbPages = NbrOfPages % FLASH_PAGE_NBPERBANK;
        pEraseInit.Page = FLASH_PAGE_NBPERBANK - pEraseInit.NbPages;
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

        NbrOfPages = FLASH_PAGE_NBPERBANK;
    }

    if(status == HAL_OK)
    {
        pEraseInit.Banks = FLASH_BANK_2;
        pEraseInit.NbPages = NbrOfPages;
        pEraseInit.Page = FLASH_PAGE_NBPERBANK - pEraseInit.NbPages;
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
    }

    HAL_FLASH_Lock();

    if(status != HAL_OK)
    {
        /* Error occurred while page erase */
        return BL_ERASE_ERROR;
    }

    return BL_OK;
}

/*** Initialize flash for programming *****************************************/
void Bootloader_FlashInit(void)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPTVERR);
    HAL_FLASH_Lock();
    
    /* Reset Flash destination address */
    flash_ptr = APP_ADDRESS;
    
    HAL_FLASH_Unlock();
}

/*** Program 64bit data into flash ********************************************/
uint8_t Bootloader_FlashNext(uint64_t data)
{
    if( !(flash_ptr <= (FLASH_BASE + FLASH_SIZE - 8)) || (flash_ptr < APP_ADDRESS) )
    {
        HAL_FLASH_Lock();
        return BL_WRITE_ERROR;
    }
    
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_ptr, data) == HAL_OK)      
    {
        /* Check the written value */
        if(*(uint64_t*)flash_ptr != data)
        {
            /* Flash content doesn't match source content */
            HAL_FLASH_Lock();
            return BL_WRITE_ERROR;
        }   
        /* Increment Flash destination address */
        flash_ptr += 8;
    }
    else
    {
        /* Error occurred while writing data in Flash memory */
        HAL_FLASH_Lock();
        return BL_WRITE_ERROR;
    }
    
    return BL_OK;
}

/*** Finish flash programming *************************************************/
void Bootloader_FlashEnd(void)
{    
    HAL_FLASH_Lock();
}

/*** Verify checksum of application *******************************************/
uint8_t Bootloader_VerifyChecksum(void)
{
#if USE_CHECKSUM
    CRC_HandleTypeDef CrcHandle;
    volatile uint32_t calculatedCrc = 0;
    
    __HAL_RCC_CRC_CLK_ENABLE();
    CrcHandle.Instance = CRC;
    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;
    if(HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {    
        return BL_CHKS_ERROR;
    }
    
    calculatedCrc = HAL_CRC_Calculate(&CrcHandle, (uint32_t*)APP_ADDRESS, APP_SIZE);
    
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
    
    if( (*(uint32_t*)CRC_ADDRESS) != calculatedCrc)
    {
        return BL_CHKS_ERROR;
    }
#endif
    return BL_OK;
}

/*** Check for application in user flash **************************************/
uint8_t Bootloader_CheckForApplication(void)
{
    return ( ((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000 ) ? BL_OK : BL_NO_APP;
}

/*** Jump to application ******************************************************/
void Bootloader_JumpToApplication(void)
{
    uint32_t  JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
    pFunction Jump = (pFunction)JumpAddress;
    
    HAL_RCC_DeInit();
    HAL_DeInit();
    
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    
#if SET_VECTOR_TABLE
    SCB->VTOR = APP_ADDRESS;
#endif
    
    __set_MSP(*(__IO uint32_t*)APP_ADDRESS);
    Jump();
}

/*** Jump to System Memory (ST Bootloader) ************************************/
void Bootloader_JumpToSysMem(void)
{
    uint32_t  JumpAddress = *(__IO uint32_t*)(SYSMEM_ADDRESS + 4);
    pFunction Jump = (pFunction)JumpAddress;
    
    HAL_RCC_DeInit();
    HAL_DeInit();
    
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    __set_MSP(*(__IO uint32_t*)SYSMEM_ADDRESS);
    Jump();
    
    while(1);
}

