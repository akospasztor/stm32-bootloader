/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the UART-based logging implementation for debug
 *          output via the virtual COM port.
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
 * @addtogroup bsp_stm32l496_discovery_log
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "log.h"

#include "board.h"
#include "error_handler.h"
#include "rcc.h"

#include <stdarg.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
/** UART handle for the virtual COM port used for logging. */
static UART_HandleTypeDef vcpUart;

/* Functions -----------------------------------------------------------------*/
void LogPrint(const char* format, ...)
{
    /** Buffer used for logging. */
    static uint8_t logBuffer[VCP_UART_BUFFER_SIZE] = {0};

    // Message:     The standard header file <stdarg.h> shall not be used
    //              [misra-c2012-17.1]
    // Reason:      This function provides a flexible, printf-style method to
    //              print log messages over the UART interface. The
    //              implementation relies on the variadic functions.
    // Risk:        The Standard lists many instances of undefined behavior
    //              associated with the features of <stdarg.h>, including:
    //              - va_end not being used prior to end of a function in which
    //                va_start was used
    //              - va_arg being used in different functions on the same
    //                va_list
    //              - the type of an argument not being compatible with the
    //                type specified to va_arg
    // Prevention:  Code reviews and testing.
    va_list arg;            // cppcheck-suppress [misra-c2012-17.1]
    va_start(arg, format);  // cppcheck-suppress [misra-c2012-17.1]

    const int32_t length =
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        vsnprintf((char*)logBuffer, VCP_UART_BUFFER_SIZE,
                  format,  // NOLINT(clang-diagnostic-format-nonliteral)
                  arg);

    va_end(arg);  // cppcheck-suppress [misra-c2012-17.1]

    if (length > 0)
    {
        (void)HAL_UART_Transmit(&vcpUart, (uint8_t*)logBuffer, (uint16_t)length,
                                VCP_UART_TX_TIMEOUT);
    }
}

void LogInit(void)
{
    RccEnableUart2();

    vcpUart.Instance                    = VCP_UART_INSTANCE;
    vcpUart.Init.BaudRate               = VCP_UART_BAUDRATE;
    vcpUart.Init.WordLength             = UART_WORDLENGTH_8B;
    vcpUart.Init.StopBits               = UART_STOPBITS_1;
    vcpUart.Init.Parity                 = UART_PARITY_NONE;
    vcpUart.Init.Mode                   = UART_MODE_TX_RX;
    vcpUart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    vcpUart.Init.OverSampling           = UART_OVERSAMPLING_16;
    vcpUart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    vcpUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&vcpUart) != HAL_OK)
    {
        ErrorHandler();
    }
}

void LogDeInit(void)
{
    (void)HAL_UART_DeInit(&vcpUart);

    RccDisableUart2();
}

/**
 * @}
 * @}
 * @}
 */
