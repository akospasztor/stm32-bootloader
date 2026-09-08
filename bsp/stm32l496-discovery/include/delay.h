/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the delay function prototypes.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_DELAY_H
#define BSP_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_delay Delay
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief  Blocking delay for the specified number of milliseconds.
 *
 * @param  delayMs  Delay duration in milliseconds.
 */
void DelayMs(const uint32_t delayMs);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
