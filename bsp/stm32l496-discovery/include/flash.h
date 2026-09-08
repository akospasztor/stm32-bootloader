/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the MCU internal flash driver function prototypes
 *          for read, write, erase, and write protection operations.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_flash Flash
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
/** Address of System Memory (ST Bootloader) */
#define SYSTEM_MEMORY_ADDRESS 0x1FFF0000U

/* Functions -----------------------------------------------------------------*/

/**
 * @brief  Clear all flash error flags.
 */
void FlashClearErrorFlags(void);

/**
 * @brief  Check if an array of data fits into the flash.
 *
 * @param  address  The physical starting address of the data in the flash.
 * @param  length   The length of the data.
 *
 * @return True if the provided data length fits into the flash starting from
 *         the provided physical address; otherwise false.
 */
bool FlashCheckIfDataFits(const uint32_t address, const uint32_t length);

/**
 * @brief  Check if flash write protection is enabled for the specified region.
 *
 * @param  address  Starting address of the region to check.
 * @param  length   Length of the region in bytes.
 *
 * @return True if write protection is enabled for any part of the specified
 *         region; otherwise false.
 */
bool FlashCheckIfWriteProtectionEnabled(const uint32_t address,
                                        const uint32_t length);

/**
 * @brief  Disable flash write protection for the application area.
 *
 * @note   This function triggers a system reset after modifying the option
 *         bytes.
 *
 * @return True if write protection was successfully disabled; otherwise false.
 */
bool FlashDisableWriteProtection(void);

/**
 * @brief  Enable flash write protection for the specified region.
 *
 * @note   This function triggers a system reset after modifying the option
 *         bytes.
 *
 * @param  address  Starting address of the region to protect.
 * @param  length   Length of the region in bytes.
 *
 * @return True if write protection was successfully enabled; otherwise false.
 */
bool FlashEnableWriteProtection(const uint32_t address, const uint32_t length);

/**
 * @brief  Compare buffer content with microcontroller flash content.
 *
 * This function is useful for instance during flash write: if the content of
 * the buffer matches with the flash content, the flash does not need to be
 * written.
 *
 * @note   The function does not check for overflow of the provided buffer.
 *
 * @param  data     Buffer which content is to be compared with the flash.
 * @param  address  Physical address of the flash for the comparison.
 * @param  length   Length of the compared content.
 *
 * @return True if the contents match, otherwise false. If the provided length
 *         is zero, the returned value is false.
 */
bool FlashCompare(const uint8_t* const data,
                  const uint32_t address,
                  const uint32_t length);

/**
 * @brief  Read data from the microcontroller flash.
 *
 * @param  data     Buffer where the data from the flash is copied into.
 * @param  address  Physical address where the data is to be read.
 * @param  length   Data size (in bytes) to be read from the flash.
 *
 * @return The number of bytes successfully read.
 */
uint32_t FlashRead(uint8_t* const data,
                   const uint32_t address,
                   const uint32_t length);

/**
 * @brief  Write data into the microcontroller flash.
 *
 * The function automatically erases the required flash pages before writing to
 * the flash. All existing data located in those pages will be erased.
 *
 * @param  data     Buffer containing the data to be written.
 * @param  address  Physical flash address where the data is written.
 * @param  length   The length of the data to be written in bytes.
 *
 * @return The number of bytes written to the flash.
 */
uint32_t FlashWrite(const uint8_t* const data,
                    const uint32_t address,
                    const uint32_t length);

/**
 * @brief  Remap the microcontroller flash memory to the built-in system flash
 *         memory.
 *
 * @note   This is required before jumping to the ST built-in system memory
 *         bootloader.
 */
void FlashRemapMemoryToSystemFlash(void);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
