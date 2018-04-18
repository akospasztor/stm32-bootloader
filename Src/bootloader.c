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
  * Copyright (c) 2018 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

#include "stm32l4xx.h"
#include "bootloader.h"

/* Private typedef -----------------------------------------------------------*/
typedef void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
static uint32_t flash_ptr = APP_ADDRESS;


/*** Initialize bootloader and flash ******************************************/
void Bootloader_Init(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_FLASH_CLK_ENABLE();
    
    /* Clear flash flags */
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    HAL_FLASH_Lock();
}

/*** Erase flash **************************************************************/
uint8_t Bootloader_Erase(void)
{
    uint32_t NbrOfPages = 0;
    uint32_t PageError  = 0;
    FLASH_EraseInitTypeDef  pEraseInit;
    HAL_StatusTypeDef       status = HAL_OK;

    HAL_FLASH_Unlock();

    /* Get the number of pages to erase */
    NbrOfPages = (FLASH_BASE + FLASH_SIZE - APP_ADDRESS) / FLASH_PAGE_SIZE;

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
    
    return (status == HAL_OK) ? BL_OK : BL_ERASE_ERROR;
}

/*** Begin flash programming **************************************************/
void Bootloader_FlashBegin(void)
{    
    /* Reset flash destination address */
    flash_ptr = APP_ADDRESS;
    
    /* Unlock flash */
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
        /* Error occurred while writing data into Flash */
        HAL_FLASH_Lock();
        return BL_WRITE_ERROR;
    }
    
    return BL_OK;
}

/*** Finish flash programming *************************************************/
void Bootloader_FlashEnd(void)
{   
    /* Lock flash */
    HAL_FLASH_Lock();
}

/*** Get flash protection status **********************************************/
uint8_t Bootloader_GetProtectionStatus(void)
{
    FLASH_OBProgramInitTypeDef OBStruct = {0};
    uint8_t protection = BL_PROTECTION_NONE;
    
    HAL_FLASH_Unlock();
    
    /** Bank 1 **/
    OBStruct.PCROPConfig = FLASH_BANK_1;
    OBStruct.WRPArea = OB_WRPAREA_BANK1_AREAA;
    HAL_FLASHEx_OBGetConfig(&OBStruct);
    /* PCROP */
    if(OBStruct.PCROPEndAddr > OBStruct.PCROPStartAddr)
    {
        if(OBStruct.PCROPStartAddr >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_PCROP;
        }
    }
    /* WRP Area_A */
    if(OBStruct.WRPEndOffset > OBStruct.WRPStartOffset)
    {
        if((OBStruct.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_WRP;
        }
    }
    
    OBStruct.WRPArea = OB_WRPAREA_BANK1_AREAB;
    HAL_FLASHEx_OBGetConfig(&OBStruct);
    /* WRP Area_B */
    if(OBStruct.WRPEndOffset > OBStruct.WRPStartOffset)
    {
        if((OBStruct.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_WRP;
        }
    }
    
    /** Bank 2 **/
    OBStruct.PCROPConfig = FLASH_BANK_2;
    OBStruct.WRPArea = OB_WRPAREA_BANK2_AREAA;
    HAL_FLASHEx_OBGetConfig(&OBStruct);
    /* PCROP */
    if(OBStruct.PCROPEndAddr > OBStruct.PCROPStartAddr)
    {
        if(OBStruct.PCROPStartAddr >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_PCROP;
        }
    }
    /* WRP Area_A */
    if(OBStruct.WRPEndOffset > OBStruct.WRPStartOffset)
    {
        if((OBStruct.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE + FLASH_PAGE_SIZE * FLASH_PAGE_NBPERBANK) >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_WRP;
        }
    }
    
    OBStruct.WRPArea = OB_WRPAREA_BANK2_AREAB;
    HAL_FLASHEx_OBGetConfig(&OBStruct);
    /* WRP Area_B */
    if(OBStruct.WRPEndOffset > OBStruct.WRPStartOffset)
    {
        if((OBStruct.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE + FLASH_PAGE_SIZE * FLASH_PAGE_NBPERBANK) >= APP_ADDRESS)
        {
            protection |= BL_PROTECTION_WRP;
        }
    }
   
    /** RDP **/
    if(OBStruct.RDPLevel != OB_RDP_LEVEL_0)
    {
        protection |= BL_PROTECTION_RDP;
    }    
    
    HAL_FLASH_Lock();
    return protection;
}

/*** Configure flash write protection ***********************************************/
uint8_t Bootloader_ConfigProtection(uint32_t protection)
{
    FLASH_OBProgramInitTypeDef  OBStruct = {0};
    HAL_StatusTypeDef           status = HAL_ERROR;
    
    status = HAL_FLASH_Unlock();
    status |= HAL_FLASH_OB_Unlock();

    /* Bank 1 */
    OBStruct.WRPArea = OB_WRPAREA_BANK1_AREAA;    
    OBStruct.OptionType = OPTIONBYTE_WRP;
    if(protection & BL_PROTECTION_WRP)
    {
        /* Enable WRP protection for application space */
        OBStruct.WRPStartOffset = (APP_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;
        OBStruct.WRPEndOffset = FLASH_PAGE_NBPERBANK - 1;
    }
    else
    {
        /* Remove WRP protection */
        OBStruct.WRPStartOffset = 0xFF;
        OBStruct.WRPEndOffset = 0x00;
    }
    status |= HAL_FLASHEx_OBProgram(&OBStruct);

    /* Area B is not used */
    OBStruct.WRPArea = OB_WRPAREA_BANK1_AREAB;    
    OBStruct.OptionType = OPTIONBYTE_WRP;
    OBStruct.WRPStartOffset = 0xFF;
    OBStruct.WRPEndOffset = 0x00;
    status |= HAL_FLASHEx_OBProgram(&OBStruct);

    /* Bank 2 */
    OBStruct.WRPArea = OB_WRPAREA_BANK2_AREAA;
    OBStruct.OptionType = OPTIONBYTE_WRP;
    if(protection & BL_PROTECTION_WRP)
    {
         /* Enable WRP protection for application space */
        OBStruct.WRPStartOffset = 0x00;
        OBStruct.WRPEndOffset = FLASH_PAGE_NBPERBANK - 1;
    }
    else
    {
        /* Remove WRP protection */
        OBStruct.WRPStartOffset = 0xFF;
        OBStruct.WRPEndOffset = 0x00;
    }
    status |= HAL_FLASHEx_OBProgram(&OBStruct);

    /* Area B is not used */
    OBStruct.WRPArea = OB_WRPAREA_BANK2_AREAB;
    OBStruct.OptionType = OPTIONBYTE_WRP;
    OBStruct.WRPStartOffset = 0xFF;
    OBStruct.WRPEndOffset = 0x00;
    status |= HAL_FLASHEx_OBProgram(&OBStruct);
    
    if(status == HAL_OK)
    {
        /* Loading Flash Option Bytes - this generates a system reset. */ 
        status |= HAL_FLASH_OB_Launch();
    }
    
    status |= HAL_FLASH_OB_Lock();
    status |= HAL_FLASH_Lock();

    return (status == HAL_OK) ? BL_OK : BL_OBP_ERROR;
}

/*** Check if application fits into user flash ********************************/
uint8_t Bootloader_CheckSize(uint32_t appsize)
{
    return ((FLASH_BASE + FLASH_SIZE - APP_ADDRESS) >= appsize) ? BL_OK : BL_SIZE_ERROR;
}

/*** Verify checksum of application located in flash **************************/
uint8_t Bootloader_VerifyChecksum(void)
{
#if (USE_CHECKSUM)
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
    
    if( (*(uint32_t*)CRC_ADDRESS) == calculatedCrc )
    {
        return BL_OK;
    }
#endif
    return BL_CHKS_ERROR;
}

/*** Check for application in user flash **************************************/
uint8_t Bootloader_CheckForApplication(void)
{
    return ( ((*(__IO uint32_t*)APP_ADDRESS) & ~(RAM_SIZE-1)) == 0x20000000 ) ? BL_OK : BL_NO_APP;
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
    
#if (SET_VECTOR_TABLE)
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
    
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    __set_MSP(*(__IO uint32_t*)SYSMEM_ADDRESS);
    Jump();
    
    while(1);
}
