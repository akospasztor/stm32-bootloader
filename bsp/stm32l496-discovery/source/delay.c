/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the delay function implementation using the HAL
 *          tick timer.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bsp
 * @{
 * @addtogroup bsp_stm32l496_discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_delay
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"

#include "mcu_hal.h"

/* Functions -----------------------------------------------------------------*/
void DelayMs(const uint32_t delayMs)
{
    HAL_Delay(delayMs);
}

/**
 * @}
 * @}
 * @}
 */
