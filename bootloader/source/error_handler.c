/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the error handling implementation.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bl
 * @{
 * @addtogroup bl_error_handler
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "error_handler.h"

#include "delay.h"
#include "led.h"
#include "mcu_hal.h"

#include <stdint.h>

/* Functions -----------------------------------------------------------------*/
void ErrorHandler(void)
{
    LedGreen1On();
    DelayMs(250);
    LedGreen1Off();
    LedGreen2On();
    DelayMs(250);
    LedGreen2Off();
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the file and the line number where the error
 *         has occurred.
 *
 * @param  file  Pointer to the file name.
 * @param  line  Error line number.
 */
// NOLINTNEXTLINE(readability-identifier-naming,readability-non-const-parameter)
void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);

    __disable_irq();
    for (;;)
    {
    }
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 * @}
 */
