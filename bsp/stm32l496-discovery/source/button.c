/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author Akos Pasztor
 * @brief  This file contains the push button driver implementation for the
 *         STM32L496 Discovery board.
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
 * @addtogroup bsp_stm32l496_discovery_button
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "button.h"

#include "board.h"
#include "mcu_hal.h"

/* Functions -----------------------------------------------------------------*/
bool ButtonIsPressed(void)
{
    return (BUTTON_GPIO_PRESSED_STATE ==
            HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN));
}

/**
 * @}
 * @}
 * @}
 */
