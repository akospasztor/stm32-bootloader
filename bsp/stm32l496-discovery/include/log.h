/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the UART-based logging function prototypes for
 *          the debug output.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_LOG_H
#define BSP_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_log Log
 * @{
 */

/* Includes ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief  Initialize the UART peripheral used for logging.
 */
void LogInit(void);

/**
 * @brief  De-initialize the UART peripheral used for logging.
 */
void LogDeInit(void);

/**
 * @brief  Print a formatted log message over the UART interface.
 *
 * @param  format  Printf-style format string.
 * @param  ...     Variable arguments matching the format string.
 */
void LogPrint(const char* format, ...);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
