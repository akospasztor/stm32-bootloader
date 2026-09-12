/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the RCC (Reset and Clock Control) driver function
 *          prototypes for enabling and disabling peripheral clocks and managing
 *          reset flags.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_RCC_H
#define BSP_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_rcc Rcc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/* Functions------------------------------------------------------------------*/
/**
 * @brief  Enable the CRC peripheral clock.
 */
void RccEnableCrc(void);

/**
 * @brief  Disable the CRC peripheral clock.
 */
void RccDisableCrc(void);

/**
 * @brief  Enable the DMA2 peripheral clock.
 */
void RccEnableDma2(void);

/**
 * @brief  Disable the DMA2 peripheral clock.
 */
void RccDisableDma2(void);

/**
 * @brief  Enable the flash interface clock.
 */
void RccEnableFlash(void);

/**
 * @brief  Disable the flash interface clock.
 */
void RccDisableFlash(void);

/**
 * @brief  Enable the GPIO port A clock.
 */
void RccEnablePortA(void);

/**
 * @brief  Disable the GPIO port A clock.
 */
void RccDisablePortA(void);

/**
 * @brief  Enable the GPIO port B clock.
 */
void RccEnablePortB(void);

/**
 * @brief  Disable the GPIO port B clock.
 */
void RccDisablePortB(void);

/**
 * @brief  Enable the GPIO port C clock.
 */
void RccEnablePortC(void);

/**
 * @brief  Disable the GPIO port C clock.
 */
void RccDisablePortC(void);

/**
 * @brief  Enable the GPIO port D clock.
 */
void RccEnablePortD(void);

/**
 * @brief  Disable the GPIO port D clock.
 */
void RccDisablePortD(void);

/**
 * @brief  Enable the PWR clock.
 */
void RccEnablePwr(void);

/**
 * @brief  Disable the PWR clock.
 */
void RccDisablePwr(void);

/**
 * @brief  Enable the SDMMC1 peripheral clock.
 */
void RccEnableSdmmc1(void);

/**
 * @brief  Disable the SDMMC1 peripheral clock.
 */
void RccDisableSdmmc1(void);

/**
 * @brief  Enable the SYSCFG clock.
 */
void RccEnableSysCfg(void);

/**
 * @brief  Disable the SYSCFG clock.
 */
void RccDisableSysCfg(void);

/**
 * @brief  Enable the USART2 peripheral clock.
 */
void RccEnableUart2(void);

/**
 * @brief  Disable the USART2 peripheral clock.
 */
void RccDisableUart2(void);

/**
 * @brief  Check if the option byte loader reset flag is set.
 *
 * @return True if the OBL reset flag is active; otherwise false.
 */
bool RccResetFlagOblIsSet(void);

/**
 * @brief  Clear all RCC reset flags.
 */
void RccResetFlagsClear(void);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
