/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the main application header with bootloader
 *          version definitions.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bl Bootloader
 * @{
 * @addtogroup bl_main Main
 * @{
 */

/* Defines -------------------------------------------------------------------*/
#define BOOTLOADER_VERSION_MAJOR 1U /*!< Major version */
#define BOOTLOADER_VERSION_MINOR 1U /*!< Minor version */
#define BOOTLOADER_VERSION_PATCH 3U /*!< Patch version */

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
