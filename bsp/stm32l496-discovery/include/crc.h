/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the CRC peripheral driver function prototypes
 *          for initialization, calculation, and de-initialization.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BSP_CRC_H
#define BSP_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_crc Crc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Functions------------------------------------------------------------------*/
/**
 * @brief  Initialize the CRC peripheral.
 */
void CrcInit(void);

/**
 * @brief  De-initialize the CRC peripheral.
 */
void CrcDeInit(void);

/**
 * @brief  Calculate the CRC value over the provided buffer.
 *
 * The calculation is done in an accumulated manner; i.e. the calculation starts
 * with the previously computed CRC value as the CRC initialization value.
 *
 * @param  data  Pointer to the buffer containing the data.
 * @param  size  Size of the buffer in bytes.
 *
 * @return The calculated CRC value.
 */
uint32_t CrcAccumulate(const uint8_t* const data, const uint32_t size);

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
