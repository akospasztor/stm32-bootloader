/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the Cortex-M4 exception handlers. The peripheral
 *          interrupt service routines are implemented in their respective
 *          drivers.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
 * @brief This function handles the non-maskable interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void NMI_Handler(void)
{
    for (;;)
    {
    }
}

/**
 * @brief This function handles the hard fault interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void HardFault_Handler(void)
{
    for (;;)
    {
    }
}

/**
 * @brief This function handles the memory management fault interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void MemManage_Handler(void)
{
    for (;;)
    {
    }
}

/**
 * @brief This function handles the prefetch faults and memory access faults
 *        interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void BusFault_Handler(void)
{
    for (;;)
    {
    }
}

/**
 * @brief This function handles the undefined instruction or illegal states
 *        interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void UsageFault_Handler(void)
{
    for (;;)
    {
    }
}

/**
 * @brief This function handles the system service call interrupts via the SWI
 *        instruction.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void SVC_Handler(void)
{
}

/**
 * @brief This function handles the debug monitor interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles the pendable request interrupts.
 */
// NOLINTNEXTLINE(readability-identifier-naming,clang-diagnostic-missing-prototypes)
void PendSV_Handler(void)
{
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/
