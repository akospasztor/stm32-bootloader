/**
  ******************************************************************************
  * Permasense Wireless GPS 2.0
  ******************************************************************************
  * @author Akos Pasztor
  * @file   sd.c
  * @brief  
  *			
  *			
  *
  ******************************************************************************
  * Copyright (c) 2016 Akos Pasztor, ETH Zurich. All rights reserved.
  ******************************************************************************
**/

#include "bsp_driver_sd.h"
#include "fatfs.h"
#include "bootloader.h"

FATFS FatFs;
uint8_t SD_Present = 0;
uint8_t readed[10] = {0x00};
volatile uint32_t size;

/* External variables --------------------------------------------------------*/ 
extern SD_HandleTypeDef _HSD;
extern HAL_SD_CardInfoTypedef _SD_CARD_INFO; 

/* SD-usage Functions --------------------------------------------------------*/
uint8_t readSD(void)
{
    FIL fil;
    FRESULT fr;    
    
    fr = f_open(&fil, "image.bin", FA_READ);
    if(fr == FR_OK)
    {
        UINT num;
        
        size = f_size(&fil);
        size = Bootloader_CheckSize(size);
        
        fr = f_read(&fil, readed, 8, &num);
        fr = FR_OK;
        
        f_close(&fil);
    }
    else
    {
        fr = FR_OK;
    }
    return 0;
}
            
uint8_t mountSD(void)
{
    FRESULT fr;
    uint8_t tries;
    
    for(tries=5; tries>0; --tries)
    {
        fr = f_mount(&FatFs, "", 1);
        if(fr == FR_OK) { return 0; }
        HAL_Delay(200);
    }
    return 1;
}
    
uint8_t unmountSD(void)
{
    f_mount(NULL, "", 1);
    return 0;
}

/* BSP Functions -------------------------------------------------------------*/

/**
  * @brief  Initializes the SD card device.
  * @retval SD status.
  */
uint8_t BSP_SD_Init(void)
{
    uint8_t state = MSD_OK;
    /* Check if the SD card is plugged in the slot */
    if (BSP_SD_IsDetected() != SD_PRESENT)
    {
        return MSD_ERROR;
    }    
    
    state = HAL_SD_Init(&_HSD, &_SD_CARD_INFO);
    
#ifdef BUS_4BITS
    /* Configure SD Bus width */
    if (state == MSD_OK)
    {
        /* Enable wide operation */
        if (HAL_SD_WideBusOperation_Config(&_HSD, SDMMC_BUS_WIDE_4B) != SD_OK)
        {
            state = MSD_ERROR;
        }
        else
        {
            state = MSD_OK;
        }
    }
#endif
    
    return state;
}

/**
  * @brief  De-Initializes the SD card device.
  * @retval none.
  */
void BSP_SD_DeInit(void)
{
    HAL_SD_DeInit(&_HSD);
}

/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @retval Returns 0 in success otherwise 1. 
  */
uint8_t BSP_SD_ITConfig(void)
{  
    return (uint8_t)0;
}

/** @brief  SD detect IT treatment
  * @retval None
  */
void BSP_SD_DetectIT(void)
{
    /* SD detect IT callback */
    BSP_SD_DetectCallback(); 
}

/** @brief  SD detect IT detection callback
  * @retval None
  */
__weak void BSP_SD_DetectCallback(void)
{
      /* NOTE: This function Should not be modified, when the callback is needed,
      the BSP_SD_DetectCallback could be implemented in the user file
      */  
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks)
{
    uint8_t sd_state;
    if(HAL_SD_ReadBlocks(&_HSD, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK)  
    {
        sd_state = MSD_ERROR;
    }
    else
    {
        sd_state = MSD_OK;
    }
    return sd_state;  
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks)
{
    uint8_t sd_state;
    if(HAL_SD_WriteBlocks(&_HSD, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK)  
    {
        sd_state = MSD_ERROR;
    }
    else
    {
        sd_state = MSD_OK;
    }
    return sd_state;  
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to read 
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks)
{
    uint8_t state = MSD_OK;

    /* Read block(s) in DMA transfer mode */
    if(HAL_SD_ReadBlocks_DMA(&_HSD, pData, ReadAddr, BlockSize, NumOfBlocks) != SD_OK)  
    {
        state = MSD_ERROR;
    }

    /* Wait until transfer is complete */
    if(state == MSD_OK)
    {
        if(HAL_SD_CheckReadOperation(&_HSD, (uint32_t)SD_DATATIMEOUT) != SD_OK)  
        {
            state = MSD_ERROR;
        }
        else
        {
            state = MSD_OK;
        }
    }

    return state; 
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written  
  * @param  BlockSize: SD card data block size, that should be 512
  * @param  NumOfBlocks: Number of SD blocks to write 
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks)
{
    uint8_t state = MSD_OK;

    /* Write block(s) in DMA transfer mode */
    if(HAL_SD_WriteBlocks_DMA(&_HSD, pData, WriteAddr, BlockSize, NumOfBlocks) != SD_OK)  
    {
        state = MSD_ERROR;
    }

    /* Wait until transfer is complete */
    if(state == MSD_OK)
    {
        if(HAL_SD_CheckWriteOperation(&_HSD, (uint32_t)SD_DATATIMEOUT) != SD_OK)  
        {
            state = MSD_ERROR;
        }
        else
        {
            state = MSD_OK;
        }
    }
  
    return state; 
}

/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr)
{
    if(HAL_SD_Erase(&_HSD, StartAddr, EndAddr) != SD_OK)  
    {
        return MSD_ERROR;
    }
    else
    {
        return MSD_OK;
    }
}

/**
  * @brief  Handles SD card interrupt request.
  * @retval None
  */
void BSP_SD_IRQHandler(void)
{
    HAL_SD_IRQHandler(&_HSD);
}

/**
  * @brief  Handles SD DMA Tx transfer interrupt request.
  * @retval None
  */
void BSP_SD_DMA_Tx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(_HSD.hdmatx); 
}

/**
  * @brief  Handles SD DMA Rx transfer interrupt request.
  * @retval None
  */
void BSP_SD_DMA_Rx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(_HSD.hdmarx);
}

/**
  * @brief  Gets the current SD card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  SD_TRANSFER_OK: No data transfer is acting
  *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
  *            @arg  SD_TRANSFER_ERROR: Data transfer error 
  */
HAL_SD_TransferStateTypedef BSP_SD_GetStatus(void)
{
    return(HAL_SD_GetStatus(&_HSD));
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None 
  */
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypedef* CardInfo)
{
    /* Get SD card Information */
    HAL_SD_Get_CardInfo(&_HSD, CardInfo);
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param  None
 * @retval Returns if SD is detected or not
 */
uint8_t BSP_SD_IsDetected(void)
{
    __IO uint8_t status = SD_PRESENT;
    return status;
}

