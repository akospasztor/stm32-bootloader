/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the LED driver function prototypes for
 *          controlling the green LEDs on the STM32L496 Discovery board.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_LED_H
#define BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_led Led
 * @{
 */

/* Includes ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief  Turn on the 1st green LED (LD2).
 */
void LedGreen1On(void);

/**
 * @brief  Turn off the 1st green LED (LD2).
 */
void LedGreen1Off(void);

/**
 * @brief  Toggle the 1st green LED (LD2).
 */
void LedGreen1Toggle(void);

/**
 * @brief  Turn on the 2nd green LED (LD3).
 */
void LedGreen2On(void);

/**
 * @brief  Turn off the 2nd green LED (LD3).
 */
void LedGreen2Off(void);

/**
 * @brief  Toggle the 2nd green LED (LD3).
 */
void LedGreen2Toggle(void);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
