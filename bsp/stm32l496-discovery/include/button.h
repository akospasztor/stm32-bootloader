/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the push button driver function prototypes for
 *          the STM32L496 Discovery board.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_button Button
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief  Check whether the joystick center button is currently pressed.
 *
 * @return True if the button is pressed; otherwise false.
 */
bool ButtonIsPressed(void);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
