/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the LED driver implementation for controlling
 *          the green LEDs on the STM32L496 Discovery board.
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
 * @addtogroup bsp_stm32l496_discovery_led
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "led.h"

#include "board.h"
#include "mcu_hal.h"

/* Functions -----------------------------------------------------------------*/
void LedGreen1On(void)
{
    HAL_GPIO_WritePin(LED_GREEN1_GPIO_PORT, LED_GREEN1_GPIO_PIN,
                      LED_GREEN1_GPIO_ACTIVE_STATE);
}

void LedGreen1Off(void)
{
    HAL_GPIO_WritePin(LED_GREEN1_GPIO_PORT, LED_GREEN1_GPIO_PIN,
                      LED_GREEN1_GPIO_INACTIVE_STATE);
}

void LedGreen1Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GREEN1_GPIO_PORT, LED_GREEN1_GPIO_PIN);
}

void LedGreen2On(void)
{
    HAL_GPIO_WritePin(LED_GREEN2_GPIO_PORT, LED_GREEN2_GPIO_PIN,
                      LED_GREEN2_GPIO_ACTIVE_STATE);
}

void LedGreen2Off(void)
{
    HAL_GPIO_WritePin(LED_GREEN2_GPIO_PORT, LED_GREEN2_GPIO_PIN,
                      LED_GREEN2_GPIO_INACTIVE_STATE);
}

void LedGreen2Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GREEN2_GPIO_PORT, LED_GREEN2_GPIO_PIN);
}

/**
 * @}
 * @}
 * @}
 */
