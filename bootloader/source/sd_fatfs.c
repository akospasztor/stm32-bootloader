/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SD card FatFs file system implementation,
 *          including SD card mount and unmount operations.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bl
 * @{
 * @addtogroup bl_sd_fatfs
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "sd_fatfs.h"

#include "log.h"
#include "sd_diskio.h"
#include "sdmmc.h"

/* Private variables ---------------------------------------------------------*/

/** SD logical drive path used by the FatFs middleware. */
static char sdPath[4];

/* Functions -----------------------------------------------------------------*/
bool SdCardMount(void)
{
    static FATFS sdFileSystem;

    // Message:     A function should have a single point of exit at the end
    //              [misra-c2012-15.5]
    // Reason:      Multiple and early returns are used for avoiding a complex
    //              implementation of checking the results of the intermediate
    //              operations. It also allows better code readability.
    // Risk:        A single point of exit is required by IEC 61508 and ISO
    //              26262 as part of the requirements for a modular approach.
    //              Early returns may lead to the unintentional omission of
    //              function termination code. If a function has exit points
    //              interspersed with statements that produce persistent side
    //              effects, it is not easy to determine which side effects will
    //              occur when the function is executed.
    // Prevention:  Code reviews and testing.
    // cppcheck-suppress-begin [misra-c2012-15.5]

    // Initialize the SD card
    if (!SdmmcInit())
    {
        LogPrint("SD card cannot be initialized.\n");
        return false;
    }

    // Initialize the FatFs driver
    if (FATFS_LinkDriver(SdDiskIoGetDriver(), sdPath) != 0U)
    {
        LogPrint("FatFs cannot be initialized.\n");
        return false;
    }

    /* Mount the SD card */
    if (FR_OK != f_mount(&sdFileSystem, (TCHAR const*)sdPath, 1))
    {
        LogPrint("SD card cannot be mounted.\n");
        return false;
    }

    return true;
    // cppcheck-suppress-end [misra-c2012-15.5]
}

void SdCardUnMount(void)
{
    (void)f_mount(NULL, (TCHAR const*)sdPath, 0);
    (void)FATFS_UnLinkDriver(sdPath);
    SdmmcDeInit();
}

/**
 * @}
 * @}
 */
