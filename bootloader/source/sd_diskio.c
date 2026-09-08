/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SD card disk I/O driver implementation used
 *          by the FatFs middleware for block-level read and write operations.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bl
 * @{
 * @addtogroup bl_sd_diskio
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "sd_diskio.h"

#include "board.h"
#include "log.h"
#include "sdmmc.h"

#include <stdbool.h>

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Callback for the SD DMA read transfer complete.
 */
static void SdReadCompleteCallback(void);

/**
 * @brief  Callback for the SD DMA write transfer complete.
 */
static void SdWriteCompleteCallback(void);

/**
 * @brief  Initialize the SD card disk I/O driver and register DMA callbacks.
 *
 * @param  pdrv  Physical drive number.
 * @return Disk status flags.
 */
static DSTATUS SdDiskIoInit(BYTE pdrv);

/**
 * @brief  Return the current status of the SD card.
 *
 * @param  pdrv  Physical drive number.
 * @return Disk status flags.
 */
static DSTATUS SdDiskIoStatus(BYTE pdrv);

/**
 * @brief  Read one or more sectors from the SD card.
 *
 * @param  pdrv    Physical drive number.
 * @param  buff    Pointer to the buffer to store the read data.
 * @param  sector  Start sector number.
 * @param  count   Number of sectors to read.
 * @return RES_OK on success, RES_ERROR on failure.
 */
static DRESULT SdDiskIoRead(BYTE pdrv, BYTE* buff, DWORD sector, UINT count);

/**
 * @brief  Write one or more sectors to the SD card.
 *
 * @param  pdrv    Physical drive number.
 * @param  buff    Pointer to the data to be written.
 * @param  sector  Start sector number.
 * @param  count   Number of sectors to write.
 * @return RES_OK on success, RES_ERROR on failure.
 */
static DRESULT SdDiskIoWrite(BYTE pdrv,
                             const BYTE* buff,
                             DWORD sector,
                             UINT count);

/**
 * @brief  Perform miscellaneous disk I/O control operations on the SD card.
 *
 * @param  pdrv  Physical drive number.
 * @param  cmd   Control command code (CTRL_SYNC, GET_SECTOR_COUNT, etc.).
 * @param  buff  Pointer to the parameter buffer.
 * @return RES_OK on success, RES_PARERR on unknown command, RES_NOTRDY if the
 *         card is not ready.
 */
static DRESULT SdDiskIoIoctl(BYTE pdrv, BYTE cmd, void* buff);

/* Private variables ---------------------------------------------------------*/
/** Flag indicating that a DMA read transfer has completed. */
static volatile bool isReadComplete = false;

/** Flag indicating that a DMA write transfer has completed. */
static volatile bool isWriteComplete = false;

/* Functions -----------------------------------------------------------------*/
const Diskio_drvTypeDef* SdDiskIoGetDriver(void)
{
    static const Diskio_drvTypeDef SD_DISK_IO_DRIVER = {
        SdDiskIoInit,  SdDiskIoStatus, SdDiskIoRead,
        SdDiskIoWrite, SdDiskIoIoctl,
    };

    return &SD_DISK_IO_DRIVER;
}

static DSTATUS SdDiskIoInit(BYTE pdrv)
{
    SdmmcRegisterRxCompleteCallback(SdReadCompleteCallback);
    SdmmcRegisterTxCompleteCallback(SdWriteCompleteCallback);

    isReadComplete  = false;
    isWriteComplete = false;

    return SdDiskIoStatus(pdrv);
}

static DSTATUS SdDiskIoStatus(BYTE pdrv)
{
    UNUSED(pdrv);
    DSTATUS result = STA_NOINIT;

    if (SdmmcIsCardInTransferState())
    {
        result &= ~STA_NOINIT;
    }

    return result;
}

static DRESULT SdDiskIoRead(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    UNUSED(pdrv);
    DRESULT result = RES_ERROR;

#if defined(USE_SDMMC_DMA)
    /* Use SD Driver in DMA mode */
    isReadComplete = false;
    if (SdmmcReadBlocksDma(buff, sector, count))
    {
        /* Wait for DMA Complete */
        uint32_t timeout = HAL_GetTick();
        while (!isReadComplete &&
               ((HAL_GetTick() - timeout) < SDMMC_TRANSFER_TIMEOUT))
        {
        }

        /* In case of a timeout return error */
        if (!isReadComplete)
        {
            result = RES_ERROR;
        }
        else
        {
            if (SdmmcIsCardInTransferState())
            {
                result = RES_OK;
            }
            else
            {
                LogPrint("ERROR: SD card is not in transfer state.\n");
            }
        }
    }
#else
    /* Use SD Driver in blocking mode */
    if (SdmmcReadBlocks(buff, sector, count, SDMMC_TRANSFER_TIMEOUT))
    {
        if (SdmmcIsCardInTransferState())
        {
            result = RES_OK;
        }
    }
#endif /* USE_SDMMC_DMA */

    return result;
}

static DRESULT SdDiskIoWrite(BYTE pdrv,
                             const BYTE* buff,
                             DWORD sector,
                             UINT count)
{
    UNUSED(pdrv);
    DRESULT result = RES_ERROR;

#if defined(USE_SDMMC_DMA)
    /* Use SD Driver in DMA mode */
    isWriteComplete = false;
    if (SdmmcWriteBlocksDma(buff, sector, count))
    {
        /* Wait for DMA complete */
        uint32_t timeout = HAL_GetTick();
        while (!isWriteComplete &&
               ((HAL_GetTick() - timeout) < SDMMC_TRANSFER_TIMEOUT))
        {
        }

        /* In case of a timeout return error */
        if (!isWriteComplete)
        {
            result = RES_ERROR;
        }
        else
        {
            if (SdmmcIsCardInTransferState())
            {
                result = RES_OK;
            }
            else
            {
                LogPrint("ERROR: SD card is not in transfer state.\n");
            }
        }
    }
#else
    /* Use SD Driver in blocking mode */
    result = RES_ERROR;
    if (SdmmcWriteBlocks(buff, sector, count, SDMMC_TRANSFER_TIMEOUT))
    {
        if (SdmmcIsCardInTransferState())
        {
            result = RES_OK;
        }
    }
#endif /* USE_SDMMC_DMA */

    return result;
}

static DRESULT SdDiskIoIoctl(BYTE pdrv, BYTE cmd, void* buff)
{
    UNUSED(pdrv);
    DRESULT result = RES_NOTRDY;

    if (SdmmcIsCardInTransferState())
    {
        SdCardInfo sdCardInfo = {0};
        switch (cmd)
        {
            /* Make sure that no pending write process */
            case CTRL_SYNC:
                result = RES_OK;
                break;

            /* Get number of sectors on the disk (DWORD) */
            case GET_SECTOR_COUNT:
                if (SdmmcGetCardInfo(&sdCardInfo))
                {
                    // Message:     A conversion should not be performed from
                    //              pointer to void into pointer to object
                    //              [misra-c2012-11.5]
                    // Reason:      The pointer to the buffer needs to be
                    //              converted to a pointer to DWORD in order to
                    //              fill the buffer with the requested value.
                    // Risk:        Conversion of a pointer to void into a
                    //              pointer to object may result in a pointer
                    //              that is not correctly aligned, resulting in
                    //              undefined behavior.
                    // Prevention:  Code reviews.
                    // cppcheck-suppress [misra-c2012-11.5]
                    *(DWORD*)buff = sdCardInfo.LogBlockNbr;
                    result        = RES_OK;
                }
                break;

            /* Get R/W sector size (WORD) */
            case GET_SECTOR_SIZE:
                if (SdmmcGetCardInfo(&sdCardInfo))
                {
                    // Message:     A conversion should not be performed from
                    //              pointer to void into pointer to object
                    //              [misra-c2012-11.5]
                    // Reason:      The pointer to the buffer needs to be
                    //              converted to a pointer to WORD in order to
                    //              fill the buffer with the requested value.
                    // Risk:        Conversion of a pointer to void into a
                    //              pointer to object may result in a pointer
                    //              that is not correctly aligned, resulting in
                    //              undefined behavior.
                    // Prevention:  Code reviews.
                    // cppcheck-suppress [misra-c2012-11.5]
                    *(WORD*)buff = (WORD)sdCardInfo.LogBlockSize;
                    result       = RES_OK;
                }
                break;

            /* Get erase block size in unit of sector (DWORD) */
            case GET_BLOCK_SIZE:
                if (SdmmcGetCardInfo(&sdCardInfo))
                {
                    // Message:     A conversion should not be performed from
                    //              pointer to void into pointer to object
                    //              [misra-c2012-11.5]
                    // Reason:      The pointer to the buffer needs to be
                    //              converted to a pointer to DWORD in order to
                    //              fill the buffer with the requested value.
                    // Risk:        Conversion of a pointer to void into a
                    //              pointer to object may result in a pointer
                    //              that is not correctly aligned, resulting in
                    //              undefined behavior.
                    // Prevention:  Code reviews.
                    // cppcheck-suppress [misra-c2012-11.5]
                    *(DWORD*)buff = sdCardInfo.LogBlockSize;
                    result        = RES_OK;
                }
                break;

            default:
                result = RES_PARERR;
                break;
        }
    }

    return result;
}

static void SdReadCompleteCallback(void)
{
    isReadComplete = true;
}

static void SdWriteCompleteCallback(void)
{
    isWriteComplete = true;
}

/**
 * @}
 * @}
 */
