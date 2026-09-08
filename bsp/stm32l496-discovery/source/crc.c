/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the CRC peripheral driver implementation using
 *          the microcontroller hardware CRC unit.
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
 * @addtogroup bsp_stm32l496_discovery_crc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "crc.h"

#include "error_handler.h"
#include "mcu_hal.h"
#include "rcc.h"

/* Private variables ---------------------------------------------------------*/
/** Handle for the CRC peripheral. */
static CRC_HandleTypeDef crcHandle;

/* Functions -----------------------------------------------------------------*/
void CrcInit(void)
{
    RccEnableCrc();

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following CRC define is addressing the memory mapped
    //              base register of the CRC hardware peripheral.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    crcHandle.Instance = CRC;

    crcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    crcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    crcHandle.Init.CRCLength               = CRC_POLYLENGTH_32B;
    crcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    crcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    crcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_BYTES;

    if (HAL_CRC_Init(&crcHandle) != HAL_OK)
    {
        ErrorHandler();
    }
}

void CrcDeInit(void)
{
    (void)HAL_CRC_DeInit(&crcHandle);
    RccDisableCrc();
}

uint32_t CrcAccumulate(const uint8_t* const data, const uint32_t size)
{
    // Message:     A conversion shall not remove any const, volatile or _Atomic
    //              qualification from the type pointed to by a pointer
    //              [misra-c2012-11.8]
    //              A conversion shall not be performed between a pointer to
    //              object type and a pointer to a different object type
    //              [misra-c2012-11.3]
    //              Cast from 'const uint8_t *' to 'uint32_t *' increases
    //              required alignment from 1 to 4.
    // Reason:      The HAL implementation expects a non-const pointer to
    //              non-const data, however the CRC calculation does not (and
    //              should not) modify neither the pointer, nor the value.
    //              Therefore, this wrapper function expects a const pointer to
    //              const data and both the MISRA rule and the the message from
    //              clang-tidy [error: cast from 'const unsigned X *' to
    //              'unsigned X *' drops const qualifier] is suppressed here.
    //              Additionally, the HAL implementation expects a `uint32_t*`
    //              data pointer. The CRC peripheral is configured to process
    //              bytes as input data. When using bytes, the HAL
    //              implementation casts the data pointer back to `uint8_t*`.
    // Risk:        Removing a const qualifier might circumvent the read-only
    //              status of an object and result in it being modified.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.3, misra-c2012-11.8]
    // NOLINTNEXTLINE(clang-diagnostic-cast-qual,clang-diagnostic-cast-align)
    return HAL_CRC_Accumulate(&crcHandle, (uint32_t* const)data, size);
}

/**
 * @}
 * @}
 * @}
 */
