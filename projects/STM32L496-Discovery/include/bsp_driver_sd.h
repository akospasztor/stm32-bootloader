#ifndef __BSP_DRIVER_SD_H
#define __BSP_DRIVER_SD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

/* Exported constants --------------------------------------------------------*/
#define MSD_OK                   ((uint8_t)0x00)
#define MSD_ERROR                ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT ((uint8_t)0x02)

#define SD_TRANSFER_OK    ((uint8_t)0x00)
#define SD_TRANSFER_BUSY  ((uint8_t)0x01)
#define SD_TRANSFER_ERROR ((uint8_t)0x02)

#define SD_PRESENT     ((uint8_t)0x01)
#define SD_NOT_PRESENT ((uint8_t)0x00)

#define SD_DATATIMEOUT (150U) /* ms */

#define SDMMC_IRQ_PRIO  1
#define SD_DMA_IRQ_PRIO 2

/* Exported functions --------------------------------------------------------*/
uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_DeInit(void);
uint8_t BSP_SD_ReadBlocks(uint32_t* pData,
                          uint32_t ReadAddr,
                          uint32_t NumOfBlocks,
                          uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t* pData,
                           uint32_t WriteAddr,
                           uint32_t NumOfBlocks,
                           uint32_t Timeout);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t* pData,
                              uint32_t ReadAddr,
                              uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t* pData,
                               uint32_t WriteAddr,
                               uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t BSP_SD_GetCardState(void);
void BSP_SD_GetCardInfo(BSP_SD_CardInfo* CardInfo);
uint8_t BSP_SD_IsDetected(void);

#endif /* __BSP_DRIVER_SD_H */
