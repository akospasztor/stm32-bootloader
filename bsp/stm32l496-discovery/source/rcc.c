/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the RCC (Reset and Clock Control) driver
 *          implementation for enabling and disabling peripheral clocks and
 *          managing reset flags.
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
 * @addtogroup bsp_stm32l496_discovery_rcc
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "rcc.h"

#include "mcu_hal.h"

/* Functions -----------------------------------------------------------------*/
void RccEnableCrc(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();
}

void RccDisableCrc(void)
{
    __HAL_RCC_CRC_CLK_DISABLE();
}

void RccEnableDma2(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();
}

void RccDisableDma2(void)
{
    __HAL_RCC_DMA2_CLK_DISABLE();
}

void RccEnableFlash(void)
{
    __HAL_RCC_FLASH_CLK_ENABLE();
}

void RccDisableFlash(void)
{
    __HAL_RCC_FLASH_CLK_DISABLE();
}

void RccEnablePortA(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

void RccDisablePortA(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
}

void RccEnablePortB(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

void RccDisablePortB(void)
{
    __HAL_RCC_GPIOB_CLK_DISABLE();
}

void RccEnablePortC(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

void RccDisablePortC(void)
{
    __HAL_RCC_GPIOC_CLK_DISABLE();
}

void RccEnablePortD(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

void RccDisablePortD(void)
{
    __HAL_RCC_GPIOD_CLK_DISABLE();
}

void RccEnablePwr(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
}

void RccDisablePwr(void)
{
    __HAL_RCC_PWR_CLK_DISABLE();
}

void RccEnableSdmmc1(void)
{
    __HAL_RCC_SDMMC1_CLK_ENABLE();
}

void RccDisableSdmmc1(void)
{
    __HAL_RCC_SDMMC1_CLK_DISABLE();
}

void RccEnableSysCfg(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

void RccDisableSysCfg(void)
{
    __HAL_RCC_SYSCFG_CLK_DISABLE();
}

void RccEnableUart2(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();
}

void RccDisableUart2(void)
{
    __HAL_RCC_USART2_CLK_DISABLE();
}

bool RccResetFlagOblIsSet(void)
{
    // Message:     The right hand operand of a shift operator shall lie in the
    //              range zero to one less than the width in bits of the
    //              essential type of the left hand operand [misra-c2012-12.2]
    // Reason:      The expression is part of the microcontroller vendor library
    //              (HAL). The macro has been reviewed and is known to operate
    //              correctly for the target device and compiler configuration.
    // Risk:        Theoretical risk of undefined behavior if the shift operand
    //              were outside the valid range. However, the values are fixed
    //              and controlled by the HAL definitions, so the risk is
    //              negligible.
    // Prevention:  Code reviews and testing.
    // cppcheck-suppress [misra-c2012-12.2]
    return (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) == 1U);
}

void RccResetFlagsClear(void)
{
    __HAL_RCC_CLEAR_RESET_FLAGS();
}

/**
 * @}
 * @}
 * @}
 */
