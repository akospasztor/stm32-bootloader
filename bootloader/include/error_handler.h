/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the error handling function prototypes.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bl Bootloader
 * @{
 * @addtogroup bl_error_handler ErrorHandler
 * @{
 */

/* Functions -----------------------------------------------------------------*/
/**
 * @brief  This function is executed in case of error occurrence.
 */
void ErrorHandler(void);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
