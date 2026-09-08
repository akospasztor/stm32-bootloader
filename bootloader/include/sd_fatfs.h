/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SD card FatFs file system mount and unmount
 *          function prototypes.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef SD_FATFS_H
#define SD_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bl Bootloader
 * @{
 * @addtogroup bl_sd_fatfs SdFatFs
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ff.h"

#include <stdbool.h>

/* Functions -----------------------------------------------------------------*/
/**
 * @brief  Mount the FatFs file system on the SD card by linking the disk I/O
 *         driver and mounting the default logical drive.
 *
 * @return True if the file system was mounted successfully; otherwise false.
 */
bool SdCardMount(void);

/**
 * @brief  Unmount the FatFs file system from the SD card and unlink the disk
 *         I/O driver.
 */
void SdCardUnMount(void);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
