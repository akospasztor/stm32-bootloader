/**
 *******************************************************************************
 * STM32L4 Bootloader
 *******************************************************************************
 * @author Akos Pasztor
 * @file   bsp_driver_sd.c
 * @brief  SD BSP driver
 *	       This file contains the implementation of the SD BSP driver used by
 *         the FatFs module. The driver uses the HAL library of ST.
 *
 *******************************************************************************
 * @copyright (c) 2020 Akos Pasztor.                    https://akospasztor.com
 *******************************************************************************
 */

#include "bsp_driver_sd.h"

/* Variables -----------------------------------------------------------------*/
SD_HandleTypeDef hsd1;

/* Private function prototypes -----------------------------------------------*/
static void BSP_SD_MspInit(void);
static void BSP_SD_MspDeInit(void);
static HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef* hsd);
static HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef* hsd);

/* External function prototypes ----------------------------------------------*/
extern void Error_Handler(void);
extern void SD_ReadCpltCallback(void);
extern void SD_WriteCpltCallback(void);

/**
 * @brief  Initializes the SD card device.
 * @retval SD status
 */
uint8_t BSP_SD_Init(void)
{
    uint8_t tries;

    /* Check if the SD card is plugged in the slot */
    if(BSP_SD_IsDetected() != SD_PRESENT)
    {
        return MSD_ERROR_SD_NOT_PRESENT;
    }

    /* SD interface configuration */
    hsd1.Instance                 = SDMMC1;
    hsd1.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
    hsd1.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
    hsd1.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd1.Init.BusWide             = SDMMC_BUS_WIDE_1B;
    hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
    hsd1.Init.ClockDiv            = 0;

    for(tries = 0; tries < 5; ++tries)
    {
        /* Msp SD initialization */
        BSP_SD_MspDeInit();
        BSP_SD_MspInit();

        /* HAL SD initialization */
        if(HAL_SD_Init(&hsd1) != HAL_OK)
        {
            continue;
        }

        /* Enable wide operation */
        if(HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK)
        {
            /* Retry */
            continue;
        }

        /* Everything is ok */
        return MSD_OK;
    }

    return MSD_ERROR;
}

/**
 * @brief  De-Initializes the SD card device
 * @retval None
 */
uint8_t BSP_SD_DeInit(void)
{
    /* HAL SD de-initialization */
    HAL_SD_DeInit(&hsd1);

    /* Msp SD de-initialization */
    BSP_SD_MspDeInit();

    /* SDMMC Reset */
    __HAL_RCC_SDMMC1_FORCE_RESET();
    __HAL_RCC_SDMMC1_RELEASE_RESET();

    /* Misc */
    hsd1.State               = HAL_SD_STATE_RESET;
    hsd1.Context             = 0;
    hsd1.ErrorCode           = 0;
    hsd1.SdCard.CardType     = 0;
    hsd1.SdCard.CardVersion  = 0;
    hsd1.SdCard.Class        = 0;
    hsd1.SdCard.RelCardAdd   = 0;
    hsd1.SdCard.BlockNbr     = 0;
    hsd1.SdCard.BlockSize    = 0;
    hsd1.SdCard.LogBlockNbr  = 0;
    hsd1.SdCard.LogBlockSize = 0;
    hsd1.CSD[0]              = 0;
    hsd1.CSD[1]              = 0;
    hsd1.CSD[2]              = 0;
    hsd1.CSD[3]              = 0;
    hsd1.CID[0]              = 0;
    hsd1.CID[1]              = 0;
    hsd1.CID[2]              = 0;
    hsd1.CID[3]              = 0;

    return MSD_OK;
}

/**
 * @brief  Reads block(s) from a specified address in an SD card, in polling
 * mode.
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  ReadAddr: Address from where data is to be read
 * @param  NumOfBlocks: Number of SD blocks to read
 * @param  Timeout: Timeout for read operation
 * @retval SD status
 */
uint8_t BSP_SD_ReadBlocks(uint32_t* pData,
                          uint32_t ReadAddr,
                          uint32_t NumOfBlocks,
                          uint32_t Timeout)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_ReadBlocks(&hsd1, (uint8_t*)pData, ReadAddr, NumOfBlocks,
                         Timeout) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}

/**
 * @brief  Writes block(s) to a specified address in an SD card, in polling
 * mode.
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  WriteAddr: Address from where data is to be written
 * @param  NumOfBlocks: Number of SD blocks to write
 * @param  Timeout: Timeout for write operation
 * @retval SD status
 */
uint8_t BSP_SD_WriteBlocks(uint32_t* pData,
                           uint32_t WriteAddr,
                           uint32_t NumOfBlocks,
                           uint32_t Timeout)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_WriteBlocks(&hsd1, (uint8_t*)pData, WriteAddr, NumOfBlocks,
                          Timeout) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}

/**
 * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  ReadAddr: Address from where data is to be read
 * @param  NumOfBlocks: Number of SD blocks to read
 * @retval SD status
 */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t* pData,
                              uint32_t ReadAddr,
                              uint32_t NumOfBlocks)
{
    HAL_StatusTypeDef sd_state = HAL_OK;

    /* Invalidate the dma tx handle*/
    hsd1.hdmatx = NULL;

    /* Prepare the dma channel for a read operation */
    sd_state = SD_DMAConfigRx(&hsd1);

    if(sd_state == HAL_OK)
    {
        /* Read block(s) in DMA transfer mode */
        sd_state = HAL_SD_ReadBlocks_DMA(&hsd1, (uint8_t*)pData, ReadAddr,
                                         NumOfBlocks);
    }

    return (sd_state == HAL_OK) ? MSD_OK : MSD_ERROR;
}

/**
 * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  WriteAddr: Address from where data is to be written
 * @param  NumOfBlocks: Number of SD blocks to write
 * @retval SD status
 */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t* pData,
                               uint32_t WriteAddr,
                               uint32_t NumOfBlocks)
{
    HAL_StatusTypeDef sd_state = HAL_OK;

    /* Invalidate the dma rx handle*/
    hsd1.hdmarx = NULL;

    /* Prepare the dma channel for a read operation */
    sd_state = SD_DMAConfigTx(&hsd1);

    if(sd_state == HAL_OK)
    {
        /* Write block(s) in DMA transfer mode */
        sd_state = HAL_SD_WriteBlocks_DMA(&hsd1, (uint8_t*)pData, WriteAddr,
                                          NumOfBlocks);
    }

    return (sd_state == HAL_OK) ? MSD_OK : MSD_ERROR;
}

/**
 * @brief  Erases the specified memory area of the given SD card.
 * @param  StartAddr: Start byte address
 * @param  EndAddr: End byte address
 * @retval SD status
 */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_Erase(&hsd1, StartAddr, EndAddr) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}

/**
 * @brief  Gets the current SD card data status.
 * @param  None
 * @retval Data transfer state.
 *          This value can be one of the following values:
 *            @arg  SD_TRANSFER_OK: No data transfer is acting
 *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
 */
uint8_t BSP_SD_GetCardState(void)
{
    HAL_SD_CardStateTypedef card_state;
    card_state = HAL_SD_GetCardState(&hsd1);

    if(card_state == HAL_SD_CARD_TRANSFER)
    {
        return SD_TRANSFER_OK;
    }
    else if((card_state == HAL_SD_CARD_SENDING) ||
            (card_state == HAL_SD_CARD_RECEIVING) ||
            (card_state == HAL_SD_CARD_PROGRAMMING))
    {
        return SD_TRANSFER_BUSY;
    }
    else
    {
        return SD_TRANSFER_ERROR;
    }
}

/**
 * @brief  Get SD information about specific SD card.
 * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
 * @retval None
 */
void BSP_SD_GetCardInfo(BSP_SD_CardInfo* CardInfo)
{
    HAL_SD_GetCardInfo(&hsd1, CardInfo);
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param  None
 * @retval Returns if SD is detected or not
 */
uint8_t BSP_SD_IsDetected(void)
{
    return SD_PRESENT;
}

/**
 * @brief SD Abort callbacks
 * @param hsd: SD handle
 * @retval None
 */
void HAL_SD_AbortCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    // Do nothing
}

/**
 * @brief SD Error callbacks
 * @param hsd: SD handle
 * @retval None
 */
void HAL_SD_ErrorCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    Error_Handler();
}

/**
 * @brief Rx Transfer completed callback
 * @param hsd: SD handle
 * @retval None
 */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef* hsd)
{
    SD_ReadCpltCallback();
}

/**
 * @brief Tx Transfer completed callback
 * @param hsd: SD handle
 * @retval None
 */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef* hsd)
{
    SD_WriteCpltCallback();
}

/**
 * @brief Initializes the SD MSP.
 * @param None
 * @retval None
 */
void BSP_SD_MspInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable SDMMC1 clock */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    /* Enable DMA2 clocks */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Common GPIO configuration */
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;

    /* GPIOC configuration */
    GPIO_InitStruct.Pin =
        GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* GPIOD configuration */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* NVIC configuration for SDMMC1 interrupts */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, SDMMC_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

    /* DMA initialization should be done here but
     * (as there is only one channel for RX and TX)
     * it is configured and done directly when required.
     */
}

/**
 * @brief De-Initializes the SD MSP.
 * @param None
 * @retval None
 */
void BSP_SD_MspDeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Disable SDMMC1 clock */
    __HAL_RCC_SDMMC1_CLK_DISABLE();

    /* Do NOT disable DMA2 clock */
    /*__HAL_RCC_DMA2_CLK_DISABLE();*/

    /* GPIOs to analog */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = 0;

    /* GPIOC configuration */
    GPIO_InitStruct.Pin =
        GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* GPIOD configuration */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* NVIC configuration for SDMMC1 interrupts */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
}

/**
 * @brief Configure the DMA to receive data from the SD card
 * @retval
 *  HAL_ERROR or HAL_OK
 */
static HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef* hsd)
{
    static DMA_HandleTypeDef hdma_rx;
    HAL_StatusTypeDef status = HAL_ERROR;

    /* Configure DMA Rx parameters */
    hdma_rx.Instance                 = DMA2_Channel5;
    hdma_rx.Init.Request             = DMA_REQUEST_7;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_rx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, hdma_rx);

    /* Stop any ongoing transfer and reset the state */
    HAL_DMA_Abort(&hdma_rx);

    /* Deinitialize the Channel for new transfer */
    HAL_DMA_DeInit(&hdma_rx);

    /* Configure the DMA Channel */
    status = HAL_DMA_Init(&hdma_rx);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, SD_DMA_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

    return status;
}

/**
 * @brief Configure the DMA to transmit data to the SD card
 * @retval
 *  HAL_ERROR or HAL_OK
 */
static HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef* hsd)
{
    static DMA_HandleTypeDef hdma_tx;
    HAL_StatusTypeDef status;

    /* Configure DMA Tx parameters */
    hdma_tx.Instance                 = DMA2_Channel5;
    hdma_tx.Init.Request             = DMA_REQUEST_7;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_tx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, hdma_tx);

    /* Stop any ongoing transfer and reset the state */
    HAL_DMA_Abort(&hdma_tx);

    /* Deinitialize the Channel for new transfer */
    HAL_DMA_DeInit(&hdma_tx);

    /* Configure the DMA Channel */
    status = HAL_DMA_Init(&hdma_tx);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, SD_DMA_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

    return status;
}
