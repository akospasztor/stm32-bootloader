/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SD card disk I/O driver interface for the
 *          FatFs middleware.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef SD_DISK_IO_H
#define SD_DISK_IO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bl Bootloader
 * @{
 * @addtogroup bl_sd_diskio SdDiskIo
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"

/* Functions -----------------------------------------------------------------*/
/**
 * @brief  Get a pointer to the SD card disk I/O driver structure containing
 *         the initialize, status, read, write, and ioctl callbacks used by
 *         the FatFs middleware.
 *
 * @return Pointer to the SD card disk I/O driver structure.
 */
const Diskio_drvTypeDef* SdDiskIoGetDriver(void);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
