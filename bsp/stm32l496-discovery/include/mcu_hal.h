/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file includes the microcontroller-specific HAL header files.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_MCU_HAL
#define BSP_MCU_HAL

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(STM32L496xx)
#include "stm32l4xx_hal.h"
#else
#error "Please specify the target microcontroller."
#endif

#ifdef __cplusplus
}
#endif

#endif
