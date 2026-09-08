/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SDMMC peripheral driver implementation for
 *          SD card communication using DMA transfers.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bsp
 * @{
 * @addtogroup bsp_stm32l496_discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_sdmmc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "sdmmc.h"

#include "board.h"
#include "error_handler.h"
#include "mcu_hal.h"
#include "rcc.h"

/* Private variables ---------------------------------------------------------*/
/** SD card peripheral handle. */
static SD_HandleTypeDef sdHandle = {0};
/** Callback for the SD DMA read transfer complete. */
static Callback rxCompleteCallback = NULL;
/** Callback for the SD DMA write transfer complete. */
static Callback txCompleteCallback = NULL;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Reset the SD peripheral handle state and card information.
 */
static void SdmmcResetPeripheralHandleState(void);

/**
 * @brief  Configure the DMA channel for SD card receive transfers.
 *
 * @return True if the DMA channel was configured successfully; otherwise false.
 */
static bool SdmmcConfigureDmaRx(void);

/**
 * @brief  Configure the DMA channel for SD card transmit transfers.
 *
 * @return True if the DMA channel was configured successfully; otherwise false.
 */
static bool SdmmcConfigureDmaTx(void);

/* Functions -----------------------------------------------------------------*/
bool SdmmcInit(void)
{
    bool result = false;

    RccEnableSdmmc1();
    RccEnableDma2();

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following instance definition is addressing the memory
    //              mapped base register of the hardware peripheral.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    sdHandle.Instance                 = SDMMC1;
    sdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
    sdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
    sdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    sdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
    sdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
    sdHandle.Init.ClockDiv            = 0;

    for (uint_fast8_t tries = 0; tries < SDMMC_INIT_MAX_TRIES; ++tries)
    {
        SdmmcResetPeripheralHandleState();
        if (HAL_OK != HAL_SD_Init(&sdHandle))
        {
            /* Retry */
            continue;
        }

        /* Enable wide bus operation */
        if (HAL_OK !=
            HAL_SD_ConfigWideBusOperation(&sdHandle, SDMMC_BUS_WIDE_4B))
        {
            /* Retry */
            continue;
        }

        result = true;
    }

    HAL_NVIC_SetPriority(SDMMC1_IRQn, SDMMC_IRQ_PRIORITY, 0U);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

    // Note: there is only one DMA channel for RX and TX, therefore the DMA
    // initialization is done directly before a DMA transfer.

    return result;
}

void SdmmcDeInit(void)
{
    (void)HAL_SD_DeInit(&sdHandle);
    SdmmcResetPeripheralHandleState();

    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
    HAL_NVIC_DisableIRQ(DMA2_Channel5_IRQn);

    RccDisableSdmmc1();
    RccDisableDma2();
}

bool SdmmcGetCardInfo(SdCardInfo* const cardInfo)
{
    return (HAL_OK == HAL_SD_GetCardInfo(&sdHandle, cardInfo));
}

bool SdmmcIsCardInTransferState(void)
{
    return (HAL_SD_CARD_TRANSFER == HAL_SD_GetCardState(&sdHandle));
}

bool SdmmcReadBlocks(uint8_t* const data,
                     const uint32_t blockAddress,
                     const uint32_t numOfBlocks,
                     const uint32_t timeout)
{
    return (HAL_OK == HAL_SD_ReadBlocks(&sdHandle, data, blockAddress,
                                        numOfBlocks, timeout));
}

bool SdmmcWriteBlocks(const uint8_t* const data,
                      const uint32_t blockAddress,
                      const uint32_t numOfBlocks,
                      const uint32_t timeout)
{
    // Message:     A conversion shall not remove any const, volatile or _Atomic
    //              qualification from the type pointed to by a pointer
    //              [misra-c2012-11.8]
    // Reason:      The HAL implementation expects a non-const pointer to
    //              non-const data, however the write operation does not
    //              (and should not) modify neither the pointer, nor the
    //              value. Therefore, this wrapper function expects a const
    //              pointer to const data and both the MISRA rule and the
    //              the message from clang-tidy [error: cast from 'const
    //              unsigned X *' to 'unsigned X *' drops const qualifier]
    //              is suppressed here.
    // Risk:        Removing a const qualifier might circumvent the
    //              read-only status of an object and result in it being
    //              modified.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.8]
    // NOLINTNEXTLINE(clang-diagnostic-cast-qual)
    return (HAL_OK == HAL_SD_WriteBlocks(&sdHandle, (uint8_t*)data,
                                         blockAddress, numOfBlocks, timeout));
}

bool SdmmcReadBlocksDma(uint8_t* const data,
                        const uint32_t blockAddress,
                        const uint32_t numOfBlocks)
{
    bool result = false;

    // Invalidate the dma tx handle
    sdHandle.hdmatx = NULL;

    // Prepare the dma channel for read operation
    if (SdmmcConfigureDmaRx())
    {
        result = (HAL_OK == HAL_SD_ReadBlocks_DMA(&sdHandle, data, blockAddress,
                                                  numOfBlocks));
    }

    return result;
}

bool SdmmcWriteBlocksDma(const uint8_t* const data,
                         const uint32_t blockAddress,
                         const uint32_t numOfBlocks)
{
    bool result = false;

    // Invalidate the dma rx handle
    sdHandle.hdmarx = NULL;

    // Prepare the dma channel for write operation
    if (SdmmcConfigureDmaTx())
    {
        // Message:     A conversion shall not remove any const, volatile or
        //              _Atomic
        //              qualification from the type pointed to by a pointer
        //              [misra-c2012-11.8]
        // Reason:      The HAL implementation expects a non-const pointer to
        //              non-const data, however the write operation does not
        //              (and should not) modify neither the pointer, nor the
        //              value. Therefore, this wrapper function expects a const
        //              pointer to const data and both the MISRA rule and the
        //              the message from clang-tidy [error: cast from 'const
        //              unsigned X *' to 'unsigned X *' drops const qualifier]
        //              is suppressed here.
        // Risk:        Removing a const qualifier might circumvent the
        //              read-only status of an object and result in it being
        //              modified.
        // Prevention:  Code reviews.
        // cppcheck-suppress [misra-c2012-11.8]
        // NOLINTNEXTLINE(clang-diagnostic-cast-qual)
        result = (HAL_OK == HAL_SD_WriteBlocks_DMA(&sdHandle, (uint8_t*)data,
                                                   blockAddress, numOfBlocks));
    }

    return result;
}

static bool SdmmcConfigureDmaRx(void)
{
    static DMA_HandleTypeDef dmaRx = {0};

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following instance definition is addressing the memory
    //              mapped base register of the hardware peripheral.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    dmaRx.Instance                 = DMA2_Channel5;
    dmaRx.Init.Request             = DMA_REQUEST_7;
    dmaRx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    dmaRx.Init.PeriphInc           = DMA_PINC_DISABLE;
    dmaRx.Init.MemInc              = DMA_MINC_ENABLE;
    dmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaRx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dmaRx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&sdHandle, hdmarx, dmaRx);

    /* Stop any ongoing transfer and reset the state */
    (void)HAL_DMA_Abort(&dmaRx);

    /* Deinitialize the Channel for new transfer */
    (void)HAL_DMA_DeInit(&dmaRx);

    /* Configure the DMA Channel */
    const bool result = (HAL_OK == HAL_DMA_Init(&dmaRx));
    if (result)
    {
        /* NVIC configuration for DMA transfer complete interrupt */
        HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, SDMMC_DMA_IRQ_PRIORITY, 0U);
        HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);
    }

    return result;
}

static bool SdmmcConfigureDmaTx(void)
{
    static DMA_HandleTypeDef dmaTx = {0};

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following instance definition is addressing the memory
    //              mapped base register of the hardware peripheral.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    dmaTx.Instance                 = DMA2_Channel5;
    dmaTx.Init.Request             = DMA_REQUEST_7;
    dmaTx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    dmaTx.Init.PeriphInc           = DMA_PINC_DISABLE;
    dmaTx.Init.MemInc              = DMA_MINC_ENABLE;
    dmaTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaTx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dmaTx.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&sdHandle, hdmatx, dmaTx);

    /* Stop any ongoing transfer and reset the state */
    (void)HAL_DMA_Abort(&dmaTx);

    /* Deinitialize the Channel for new transfer */
    (void)HAL_DMA_DeInit(&dmaTx);

    /* Configure the DMA Channel */
    const bool result = (HAL_OK == HAL_DMA_Init(&dmaTx));
    if (result)
    {
        /* NVIC configuration for DMA transfer complete interrupt */
        HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, SDMMC_DMA_IRQ_PRIORITY, 0U);
        HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);
    }

    return result;
}

void SdmmcRegisterRxCompleteCallback(Callback callback)
{
    rxCompleteCallback = callback;
}

void SdmmcRegisterTxCompleteCallback(Callback callback)
{
    txCompleteCallback = callback;
}

static void SdmmcResetPeripheralHandleState(void)
{
    sdHandle.State               = HAL_SD_STATE_RESET;
    sdHandle.Context             = 0;
    sdHandle.ErrorCode           = 0;
    sdHandle.SdCard.CardType     = 0;
    sdHandle.SdCard.CardVersion  = 0;
    sdHandle.SdCard.Class        = 0;
    sdHandle.SdCard.RelCardAdd   = 0;
    sdHandle.SdCard.BlockNbr     = 0;
    sdHandle.SdCard.BlockSize    = 0;
    sdHandle.SdCard.LogBlockNbr  = 0;
    sdHandle.SdCard.LogBlockSize = 0;
    sdHandle.CSD[0]              = 0;
    sdHandle.CSD[1]              = 0;
    sdHandle.CSD[2]              = 0;
    sdHandle.CSD[3]              = 0;
    sdHandle.CID[0]              = 0;
    sdHandle.CID[1]              = 0;
    sdHandle.CID[2]              = 0;
    sdHandle.CID[3]              = 0;
}

/**
 * @brief  SDMMC1 interrupt handler.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&sdHandle);
}

/**
 * @brief  DMA2 Channel 5 interrupt handler for SDMMC DMA transfers.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void DMA2_Channel5_IRQHandler(void)
{
    if ((sdHandle.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_SINGLE_BLOCK)) ||
        (sdHandle.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_MULTIPLE_BLOCK)))
    {
        HAL_DMA_IRQHandler(sdHandle.hdmarx);
    }
    else if ((sdHandle.Context ==
              (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_SINGLE_BLOCK)) ||
             (sdHandle.Context ==
              (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_MULTIPLE_BLOCK)))
    {
        HAL_DMA_IRQHandler(sdHandle.hdmatx);
    }
    else
    {
        ErrorHandler();
    }
}

/**
 * @brief  SD card receive complete callback.
 *
 * @note   This function is declared in the MCU HAL library.
 * @param  hsd  Pointer to the SD handle.
 */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    if (rxCompleteCallback != NULL)
    {
        rxCompleteCallback();
    }
}

/**
 * @brief  SD card transmit complete callback.
 *
 * @note   This function is declared in the MCU HAL library.
 * @param  hsd  Pointer to the SD handle.
 */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    if (txCompleteCallback != NULL)
    {
        txCompleteCallback();
    }
}

/**
 * @brief  SD card abort callback.
 *
 * @note   This function is declared in the MCU HAL library.
 * @param  hsd  Pointer to the SD handle.
 */
void HAL_SD_AbortCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    // Do nothing
}

/**
 * @brief  SD card error callback.
 *
 * @note   This function is declared in the MCU HAL library.
 * @param  hsd  Pointer to the SD handle.
 */
void HAL_SD_ErrorCallback(SD_HandleTypeDef* hsd)
{
    UNUSED(hsd);
    ErrorHandler();
}

/**
 * @}
 * @}
 * @}
 */
