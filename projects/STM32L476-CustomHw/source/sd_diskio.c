/**
 *******************************************************************************
 * STM32L4 Bootloader
 *******************************************************************************
 * @author Akos Pasztor
 * @file   sd_diskio.c
 * @brief  SD diskio driver
 *	       This file contains the implementation of the SD diskio driver
 *         used by the FatFs module. The driver uses the HAL library of ST.
 *
 *******************************************************************************
 * Copyright (c) 2020 Akos Pasztor.                     https://akospasztor.com
 *******************************************************************************
 */

#include "sd_diskio.h"
#include "ff_gen_drv.h"

/* Defines -------------------------------------------------------------------*/
#define SD_TIMEOUT SD_DATATIMEOUT /* Defined in bsp_driver_sd.h */

/*
 * Depending on the use case, the SD card initialization could be done at the
 * application level: if it is the case, disable the define below to disable
 * the BSP_SD_Init() call in the SD_Initialize() and manually add a call to
 * BSP_SD_Init() elsewhere in the application.
 */
// #define ENABLE_SD_INIT

/* Enable the define below to use the SD driver with DMA.
 * The define has effect in SD_read and SD_write.
 * BSP_DRIVER_SD should handle the DMA MSP initialization.
 */
#define ENABLE_SD_DMA_DRIVER

/*
 * When using cachable memory region, it may be needed to maintain the cache
 * validity. Enable the define below to activate a cache maintenance at each
 * read and write operation.
 * Notice: This is applicable only for cortex M7 based platform.
 */
// #define ENABLE_SD_DMA_CACHE_MAINTENANCE 1

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat     = STA_NOINIT; /* Disk status */
static volatile UINT ReadStatus  = 0;
static volatile UINT WriteStatus = 0;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize(BYTE);
DSTATUS SD_status(BYTE);
DRESULT SD_read(BYTE, BYTE*, DWORD, UINT);

#if _USE_WRITE == 1
DRESULT SD_write(BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef SD_Driver = {
    SD_initialize, SD_status, SD_read,
#if _USE_WRITE == 1
    SD_write,
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
    SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun)
{
    Stat = STA_NOINIT;

    if(BSP_SD_GetCardState() == MSD_OK)
    {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
 * @brief  Initializes a Drive
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT;

#if defined(ENABLE_SD_INIT)
    if(BSP_SD_Init() == MSD_OK)
    {
        Stat = SD_CheckStatus(lun);
    }
#else
    Stat = SD_CheckStatus(lun);
#endif

    ReadStatus  = 0;
    WriteStatus = 0;

    return Stat;
}

/**
 * @brief  Gets Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_status(BYTE lun)
{
    return SD_CheckStatus(lun);
}

/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */
DRESULT SD_read(BYTE lun, BYTE* buff, DWORD sector, UINT count)
{
    DRESULT res;
    uint32_t timeout;

#if defined(ENABLE_SD_DMA_DRIVER)
    /* Use SD Driver in DMA mode */

#if defined(ENABLE_SD_DMA_CACHE_MAINTENANCE)
    uint32_t alignedAddr;
#endif /* SD_DMA_CACHE_MAINTENANCE */

    res        = RES_ERROR;
    ReadStatus = 0;

    if(BSP_SD_ReadBlocks_DMA((uint32_t*)buff, (uint32_t)(sector), count) ==
       MSD_OK)
    {
        /* Wait for DMA Complete */
        timeout = HAL_GetTick();
        while((ReadStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
        {
        }

        /* In case of a timeout return error */
        if(ReadStatus == 0)
        {
            res = RES_ERROR;
        }
        else
        {
            ReadStatus = 0;
            timeout    = HAL_GetTick();
            while((HAL_GetTick() - timeout) < SD_TIMEOUT)
            {
                if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                {
                    res = RES_OK;
#if defined(ENABLE_SD_DMA_CACHE_MAINTENANCE)
                    /* The SCB_InvalidateDCache_by_Addr() requires a 32-Byte
                     * aligned address, adjust the address and the D-Cache size
                     * to invalidate accordingly.
                     */
                    alignedAddr = (uint32_t)buff & ~0x1F;
                    SCB_InvalidateDCache_by_Addr(
                        (uint32_t*)alignedAddr,
                        count * BLOCKSIZE + ((uint32_t)buff - alignedAddr));
#endif /* SD_DMA_CACHE_MAINTENANCE */
                    break;
                }
            }
        }
    }

    return res;

#else
    /* Use SD Driver in blocking mode */
    res = RES_ERROR;
    if(BSP_SD_ReadBlocks((uint32_t*)buff, (uint32_t)(sector), count,
                         SDMMC_HAL_TIMEOUT) == MSD_OK)
    {
        /* Wait until the read operation is finished */
        timeout = HAL_GetTick();
        while((HAL_GetTick() - timeout) < SD_TIMEOUT)
        {
            if(BSP_SD_GetCardState() == MSD_OK)
            {
                return RES_OK;
            }
        }
    }

    return res;
#endif /* ENABLE_SD_DMA_DRIVER */
}

/**
 * @brief  Writes Sector(s)
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 */
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE* buff, DWORD sector, UINT count)
{
    DRESULT res;
    uint32_t timeout;

#if defined(ENABLE_SD_DMA_DRIVER)
    /* Use SD Driver in DMA mode */

#if defined(ENABLE_SD_DMA_CACHE_MAINTENANCE)
    uint32_t alignedAddr;
    alignedAddr = (uint32_t)buff & ~0x1F;
    SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr,
                            count * BLOCKSIZE + ((uint32_t)buff - alignedAddr));
#endif

    res         = RES_ERROR;
    WriteStatus = 0;

    if(BSP_SD_WriteBlocks_DMA((uint32_t*)buff, (uint32_t)(sector), count) ==
       MSD_OK)
    {
        /* Wait for DMA complete */
        timeout = HAL_GetTick();
        while((WriteStatus == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
        {
        }

        /* In case of a timeout return error */
        if(WriteStatus == 0)
        {
            res = RES_ERROR;
        }
        else
        {
            WriteStatus = 0;
            timeout     = HAL_GetTick();
            while((HAL_GetTick() - timeout) < SD_TIMEOUT)
            {
                if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                {
                    res = RES_OK;
                    break;
                }
            }
        }
    }

    return res;

#else
    /* Use SD Driver in blocking mode */
    res = RES_ERROR;
    if(BSP_SD_WriteBlocks((uint32_t*)buff, (uint32_t)(sector), count,
                          SDMMC_HAL_TIMEOUT) == MSD_OK)
    {
        /* Wait until the Write operation is finished */
        timeout = HAL_GetTick();
        while((HAL_GetTick() - timeout) < SD_TIMEOUT)
        {
            if(BSP_SD_GetCardState() == MSD_OK)
            {
                return RES_OK;
            }
        }
    }

    return res;
#endif /* ENABLE_SD_DMA_DRIVER */
}
#endif /* _USE_WRITE == 1 */

/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void* buff)
{
    DRESULT res = RES_ERROR;
    BSP_SD_CardInfo CardInfo;

    if(Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch(cmd)
    {
        /* Make sure that no pending write process */
        case CTRL_SYNC:
            res = RES_OK;
            break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT:
            BSP_SD_GetCardInfo(&CardInfo);
            *(DWORD*)buff = CardInfo.LogBlockNbr;
            res           = RES_OK;
            break;

        /* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE:
            BSP_SD_GetCardInfo(&CardInfo);
            *(WORD*)buff = CardInfo.LogBlockSize;
            res          = RES_OK;
            break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE:
            BSP_SD_GetCardInfo(&CardInfo);
            *(DWORD*)buff = CardInfo.LogBlockSize;
            res           = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */

/**
 * @brief Rx Transfer complete callback
 * @param hsd: SD handle
 * @retval None
 */
void BSP_SD_ReadCpltCallback(void)
{
    ReadStatus = 1;
}

/**
 * @brief Tx Transfer complete callback
 * @param hsd: SD handle
 * @retval None
 */
void BSP_SD_WriteCpltCallback(void)
{
    WriteStatus = 1;
}
