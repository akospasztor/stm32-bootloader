/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the board-specific GPIO pin definitions,
 *          peripheral configurations, and initialization function prototypes
 *          for the STM32L496 Discovery board.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_BOARD_H
#define BSP_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_board Board
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "mcu_hal.h"

#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
// Message:     A conversion should not be performed between a pointer to
//              object and an integer type [misra-c2012-11.4]
// Reason:      The following GPIO port and pin defines contain conversions that
//              are addressing memory mapped registers and hardware specific
//              features.
// Risk:        Conversion of a pointer to object into an integer may produce a
//              value that cannot be represented in the chosen integer type
//              resulting in undefined behavior.
// Prevention:  Code reviews.

/** GPIO port of the 1st green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN1_GPIO_PORT GPIOB
/** GPIO pin of the 1st green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN1_GPIO_PIN GPIO_PIN_13
/** Active pin state of the 1st green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN1_GPIO_ACTIVE_STATE GPIO_PIN_RESET
/** Inactive pin state of the 1st green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN1_GPIO_INACTIVE_STATE GPIO_PIN_SET

/** GPIO port of the 2nd green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN2_GPIO_PORT GPIOA
/** GPIO pin of the 2nd green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN2_GPIO_PIN GPIO_PIN_5
/** Active pin state of the 2nd green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN2_GPIO_ACTIVE_STATE GPIO_PIN_SET
/** Inactive pin state of the 2nd green LED. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define LED_GREEN2_GPIO_INACTIVE_STATE GPIO_PIN_RESET

/** GPIO port of the joystick center button. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define BUTTON_GPIO_PORT GPIOC
/** GPIO pin of the joystick center button. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define BUTTON_GPIO_PIN GPIO_PIN_13
/** Pressed state of the joystick center button. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define BUTTON_GPIO_PRESSED_STATE GPIO_PIN_SET

/** Maximum number of SDMMC initialization attempts. */
#define SDMMC_INIT_MAX_TRIES 5U
/** SDMMC interrupt priority. */
#define SDMMC_IRQ_PRIORITY 1U
/** SDMMC DMA interrupt priority. */
#define SDMMC_DMA_IRQ_PRIORITY 2U
/** SDMMC transfer timeout in milliseconds. */
#define SDMMC_TRANSFER_TIMEOUT 500U

/** GPIO port of the SDMMC data line 0. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D0_PORT GPIOC

/** GPIO pin of the SDMMC data line 0. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D0_PIN GPIO_PIN_8

/** GPIO port of the SDMMC data line 1. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D1_PORT GPIOC

/** GPIO pin of the SDMMC data line 1. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D1_PIN GPIO_PIN_9

/** GPIO port of the SDMMC data line 2. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D2_PORT GPIOC

/** GPIO pin of the SDMMC data line 2. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D2_PIN GPIO_PIN_10

/** GPIO port of the SDMMC data line 3. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D3_PORT GPIOC

/** GPIO pin of the SDMMC data line 3. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_D3_PIN GPIO_PIN_11

/** GPIO port of the SDMMC clock line. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_CLK_PORT GPIOC

/** GPIO pin of the SDMMC clock line. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_CLK_PIN GPIO_PIN_12

/** GPIO port of the SDMMC command line. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_CMD_PORT GPIOD

/** GPIO pin of the SDMMC command line. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_CMD_PIN GPIO_PIN_2

/** GPIO alternate function for the SDMMC peripheral. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define SDMMC_GPIO_AF GPIO_AF12_SDMMC1

/** SysTick interrupt priority. */
#define SYSTICK_IRQ_PRIORITY 0U

/** Size of the buffer used for logging. */
#define VCP_UART_BUFFER_SIZE 80U
/** UART transmit timeout in milliseconds. */
#define VCP_UART_TX_TIMEOUT 100U

/** UART peripheral instance connected to the on-board ST-Link VCP. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_INSTANCE USART2

/** UART baud rate for the VCP connection. */
#define VCP_UART_BAUDRATE 115200U

/** GPIO port of the UART TX GPIO connected to the on-board ST-Link VCP. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_GPIO_TX_PORT GPIOD

/** GPIO pin of the UART TX GPIO connected to the on-board ST-Link VCP. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_GPIO_TX_PIN GPIO_PIN_6

/** GPIO port of the UART RX GPIO connected to the on-board ST-Link VCP. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_GPIO_RX_PORT GPIOA

/** GPIO pin of the UART RX GPIO. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_GPIO_RX_PIN GPIO_PIN_2

/** GPIO alternate function of UART. */
// cppcheck-suppress-macro [misra-c2012-11.4]
#define VCP_UART_GPIO_AF GPIO_AF7_USART2

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief  Initialize the board hardware and peripherals.
 *
 * Configures the system clock, GPIOs, LEDs, button, UART, and other
 * peripherals required by the bootloader.
 */
void BoardInit(void);

/**
 * @brief  De-initialize the board hardware and peripherals.
 *
 * Resets the peripherals and GPIOs to their default state before jumping to
 * the user application or the system memory bootloader.
 */
void BoardDeInit(void);

/**
 * @brief  Set the vector table location to the specified address.
 *
 * @param  address  Base address of the vector table.
 */
void SetVectorTableLocation(const uint32_t address);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
