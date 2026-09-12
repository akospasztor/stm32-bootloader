/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SysTick timer initialization and
 *          de-initialization function prototypes.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_SYSTICK_H
#define BSP_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_systick SysTick
 * @{
 */

/* Functions------------------------------------------------------------------*/
/**
 * @brief  Initialize the SysTick timer with a 1 ms tick period.
 */
void SystickInit(void);

/**
 * @brief  De-initialize the SysTick timer.
 */
void SystickDeInit(void);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
