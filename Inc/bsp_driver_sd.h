#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
 extern "C" {
#endif 
     
/*** BUS WIDTH ***/
#define BUS_4BITS   1

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
#define _HSD                hsd
#define _SD_CARD_INFO       SDCardInfo

#ifndef SD_CardInfo
  #define   SD_CardInfo     HAL_SD_CardInfoTypedef
#endif  

#define MSD_OK              ((uint8_t)0x00)
#define MSD_ERROR           ((uint8_t)0x01)

#define SD_DATATIMEOUT      ((uint32_t)100000000)
#define SD_PRESENT          ((uint8_t)0x01)
#define SD_NOT_PRESENT      ((uint8_t)0x00)


/* DMA definitions for SD DMA transfer */
/*
#define __DMAx_TxRx_CLK_ENABLE            __HAL_RCC_DMA2_CLK_ENABLE
#define SD_DMAx_Tx_STREAM                 DMA2_Channel4  
#define SD_DMAx_Rx_STREAM                 DMA2_Channel4  
#define SD_DMAx_Tx_IRQn                   DMA2_Channel4_IRQn
#define SD_DMAx_Rx_IRQn                   DMA2_Channel4_IRQn
#define SD_DMAx_Tx_IRQHandler             DMA2_Channel4_IRQHandler
#define SD_DMAx_Rx_IRQHandler             DMA2_Channel4_IRQHandler
#define SD_DetectIRQHandler()             HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8)
*/

/* SD-usage Functions --------------------------------------------------------*/     
uint8_t readSD(void);
uint8_t mountSD(void);
uint8_t unmountSD(void);
     
/* BSP Functions -------------------------------------------------------------*/
uint8_t BSP_SD_Init(void);
void    BSP_SD_DeInit(void);
uint8_t BSP_SD_ITConfig(void);
void    BSP_SD_DetectIT(void);
void    BSP_SD_DetectCallback(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t BlockSize, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr);
void    BSP_SD_IRQHandler(void);
void    BSP_SD_DMA_Tx_IRQHandler(void);
void    BSP_SD_DMA_Rx_IRQHandler(void);
HAL_SD_TransferStateTypedef BSP_SD_GetStatus(void);
void    BSP_SD_GetCardInfo(HAL_SD_CardInfoTypedef *CardInfo);
uint8_t BSP_SD_IsDetected(void);

#ifdef __cplusplus
}
#endif

#endif /* __SD_H */
