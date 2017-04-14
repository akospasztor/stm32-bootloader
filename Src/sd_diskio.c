/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-May-2015
  * @brief   SD Disk I/O driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat = STA_NOINIT;      /* Disk status */
HAL_SD_CardInfoTypedef  SDCardInfo;

/* External variables --------------------------------------------------------*/ 
extern SD_HandleTypeDef hsd1;

/* Private function prototypes -----------------------------------------------*/
DSTATUS SD_initialize(BYTE lun);
DSTATUS SD_status(BYTE lun);
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
#endif  
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff);
#endif

const Diskio_drvTypeDef SD_Driver =
{
    SD_initialize,
    SD_status,
    SD_read, 
#if  _USE_WRITE == 1
    SD_write,
#endif
#if  _USE_IOCTL == 1
    SD_ioctl,
#endif
};

/* Functions -----------------------------------------------------------------*/
uint8_t SD_isDetected(void)
{
    /* Hardware check can be implemented e.g. with card holder detect pin */
    uint8_t status = SD_PRESENT;
    return status;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes a Drive
  * @param  lun : not used 
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT | STA_NODISK;
    
    /* Check if SD card is present */
    if(SD_isDetected() != SD_PRESENT)
    {
        return Stat;
    }
    Stat &= ~STA_NODISK;
    
    /* Initialize SD card */
    if(HAL_SD_Init(&hsd1, &SDCardInfo) != SD_OK)
    {        
        return Stat;
    }
#ifdef BUS_4BITS
    if(HAL_SD_WideBusOperation_Config(&hsd1, SDMMC_BUS_WIDE_4B) != SD_OK)
    {
        return Stat;
    }
#endif
    
    Stat &= ~STA_NOINIT;
    return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_status(BYTE lun)
{
  Stat = STA_NOINIT;
  
  if(HAL_SD_GetStatus(&hsd1) != SD_TRANSFER_OK)
  {
      return Stat;
  }
  
  Stat &= ~STA_NOINIT;
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;
    if(HAL_SD_ReadBlocks(&hsd1,
                         (uint32_t*)buff,
                         (uint64_t)(sector * BLOCK_SIZE),
                         (uint32_t)BLOCK_SIZE,
                         (uint32_t)count) != SD_OK)
    {
        res = RES_ERROR;
    }
    return res;
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
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_OK;
    if(HAL_SD_WriteBlocks(&hsd1,
                          (uint32_t*)buff,
                          (uint64_t)(sector * BLOCK_SIZE),
                          (uint32_t)BLOCK_SIZE,
                          (uint32_t)count) != SD_OK)
    {
        res = RES_ERROR;
    }
    return res;
}
#endif /* _USE_WRITE */

/**
  * @brief  I/O control operation
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if(Stat & STA_NOINIT)
  {
      return RES_NOTRDY;
  }
  
  switch(cmd)
  {
    /* Make sure that no pending write process */
    case CTRL_SYNC :
        res = RES_OK;
        break;
  
    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT :
        HAL_SD_Get_CardInfo(&hsd1, &SDCardInfo);
        *(DWORD*)buff = SDCardInfo.CardCapacity / BLOCK_SIZE;
        res = RES_OK;
        break;
  
    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE :
        *(WORD*)buff = BLOCK_SIZE;
        res = RES_OK;
        break;
  
  /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE :
        *(DWORD*)buff = BLOCK_SIZE;
        break;
  
    default:
        res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL */


