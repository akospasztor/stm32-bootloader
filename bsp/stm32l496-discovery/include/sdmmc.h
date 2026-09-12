/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SDMMC peripheral driver function prototypes
 *          for SD card initialization, read, write, and DMA transfer
 *          operations.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_SDMMC_H
#define BSP_SDMMC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_sdmmc SdMmc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "mcu_hal.h"

#include <stdbool.h>
#include <stdint.h>

/* Type definitions ----------------------------------------------------------*/
/** Type definition for the HAL SD card information structure. */
typedef HAL_SD_CardInfoTypeDef SdCardInfo;

/** Callback function pointer type definition. */
typedef void (*Callback)(void);

/* Functions------------------------------------------------------------------*/

/**
 * @brief  Initialize the SDMMC peripheral and the SD card.
 *
 * @return True if the initialization was successful; otherwise false.
 */
bool SdmmcInit(void);

/**
 * @brief  De-initialize the SDMMC peripheral.
 */
void SdmmcDeInit(void);

/**
 * @brief  Get the SD card information (capacity, block size, etc.).
 *
 * @param  cardInfo  Pointer to the structure to store the card information.
 *
 * @return True if the card information was retrieved successfully; otherwise
 *         false.
 */
bool SdmmcGetCardInfo(SdCardInfo* const cardInfo);

/**
 * @brief  Check if the SD card is in the transfer state.
 *
 * @return True if the card is in transfer state; otherwise false.
 */
bool SdmmcIsCardInTransferState(void);

/**
 * @brief  Read blocks from the SD card in blocking mode.
 *
 * @param  data          Pointer to the buffer to store the read data.
 * @param  blockAddress  Block address to start reading from.
 * @param  numOfBlocks   Number of blocks to read.
 * @param  timeout       Timeout in milliseconds.
 *
 * @return True if the read operation was successful; otherwise false.
 */
bool SdmmcReadBlocks(uint8_t* const data,
                     const uint32_t blockAddress,
                     const uint32_t numOfBlocks,
                     const uint32_t timeout);

/**
 * @brief  Write blocks to the SD card in blocking mode.
 *
 * @param  data          Pointer to the data to be written.
 * @param  blockAddress  Block address to start writing to.
 * @param  numOfBlocks   Number of blocks to write.
 * @param  timeout       Timeout in milliseconds.
 *
 * @return True if the write operation was successful; otherwise false.
 */
bool SdmmcWriteBlocks(const uint8_t* const data,
                      const uint32_t blockAddress,
                      const uint32_t numOfBlocks,
                      const uint32_t timeout);

/**
 * @brief  Read blocks from the SD card using DMA.
 *
 * @param  data          Pointer to the buffer to store the read data.
 * @param  blockAddress  Block address to start reading from.
 * @param  numOfBlocks   Number of blocks to read.
 *
 * @return True if the DMA read transfer was started successfully; otherwise
 *         false.
 */
bool SdmmcReadBlocksDma(uint8_t* const data,
                        const uint32_t blockAddress,
                        const uint32_t numOfBlocks);

/**
 * @brief  Write blocks to the SD card using DMA.
 *
 * @param  data          Pointer to the data to be written.
 * @param  blockAddress  Block address to start writing to.
 * @param  numOfBlocks   Number of blocks to write.
 *
 * @return True if the DMA write transfer was started successfully; otherwise
 *         false.
 */
bool SdmmcWriteBlocksDma(const uint8_t* const data,
                         const uint32_t blockAddress,
                         const uint32_t numOfBlocks);

/**
 * @brief  Register a callback for the DMA receive complete event.
 *
 * @param  callback  Function pointer to the callback.
 */
void SdmmcRegisterRxCompleteCallback(Callback callback);

/**
 * @brief  Register a callback for the DMA transmit complete event.
 *
 * @param  callback  Function pointer to the callback.
 */
void SdmmcRegisterTxCompleteCallback(Callback callback);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
