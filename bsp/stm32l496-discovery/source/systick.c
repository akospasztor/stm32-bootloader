/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the SysTick timer initialization and
 *          de-initialization implementation.
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
 * @addtogroup bsp_stm32l496_discovery_systick
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "systick.h"

#include "board.h"
#include "mcu_hal.h"

/* Functions -----------------------------------------------------------------*/
void SystickInit(void)
{
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000U);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SYSTICK_IRQ_PRIORITY, 0U, 0U);
}

void SystickDeInit(void)
{
    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following defines are addressing the memory mapped
    //              registers of the microcontroller.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    SysTick->CTRL = 0U;  // cppcheck-suppress [misra-c2012-11.4]
    SysTick->LOAD = 0U;  // cppcheck-suppress [misra-c2012-11.4]
    SysTick->VAL  = 0U;  // cppcheck-suppress [misra-c2012-11.4]
}

/**
 * @brief This function handles systick timer interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @}
 * @}
 * @}
 */
