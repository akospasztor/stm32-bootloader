/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author Akos Pasztor
 * @brief  This file provides the CMSIS Cortex-M4 system initialization and
 *         clock configuration. Originally provided by ST.
 *
 * This file provides two functions and one global variable to be called from
 * the user application:
 * - SystemInit(): This function is called at startup just after reset and
 *   before jumping to the main program. This call is made inside the
 *   startup_stm32l4xx.s file.
 * - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *   by the application to setup the SysTick timer or configure other
 *   parameters.
 * - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *   be called whenever the core clock is changed during program execution.
 *
 * After each device reset the MSI (4 MHz) is used as system clock source.
 * Then SystemInit() function is called from the startup_stm32l4xx.s" file to
 * configure the system clock before to jumping to the main program.
 *
 * This file configures the system clock as follows:
 *
 * | Parameter                                  | Value    |
 * |--------------------------------------------|----------|
 * | System Clock source                        | MSI      |
 * | SYSCLK (Hz)                                | 4000000  |
 * | HCLK (Hz)                                  | 4000000  |
 * | AHB Prescaler                              | 1        |
 * | APB1 Prescaler                             | 1        |
 * | APB2 Prescaler                             | 1        |
 * | PLL_M                                      | 1        |
 * | PLL_N                                      | 8        |
 * | PLL_P                                      | 7        |
 * | PLL_Q                                      | 2        |
 * | PLL_R                                      | 2        |
 * | PLLSAI1_P                                  | NA       |
 * | PLLSAI1_Q                                  | NA       |
 * | PLLSAI1_R                                  | NA       |
 * | PLLSAI2_P                                  | NA       |
 * | PLLSAI2_Q                                  | NA       |
 * | PLLSAI2_R                                  | NA       |
 * | Require 48MHz for USB OTG FS, SDIO and RNG | Disabled |
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
 * @addtogroup bsp_stm32l496_discovery_stm32l4xx_system STM32L4xx_System
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

#if !defined(HSE_VALUE)
/** Value of the external oscillator in Hz */
#define HSE_VALUE 8000000U
#endif /* HSE_VALUE */

#if !defined(MSI_VALUE)
/** Value of the internal oscillator (MSI) in Hz */
#define MSI_VALUE 4000000U
#endif /* MSI_VALUE */

#if !defined(HSI_VALUE)
/** Value of the internal oscillator (HSI) in Hz */
#define HSI_VALUE 16000000U
#endif /* HSI_VALUE */

/**
 * @brief Define to enable user-defined vector table address setting
 *
 * Uncomment the following line if you need to relocate the vector table
 * anywhere in Flash or SRAM, else the vector table is kept at the automatic
 * remap of the boot address selected.
 *
 * @note  The vector table addresses must be defined in line with the linker
          configuration.
 */
// #define USER_VECT_TAB_ADDRESS

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */

/**
 * @brief Define to relocate the vector table to the SRAM
 *
 * Uncomment the following line if the vector table needs to be relocated to the
 * SRAM, otherwise the vector table address remains in the MCU flash.
 */
/* #define VECT_TAB_SRAM */

#if defined(VECT_TAB_SRAM)
/** Vector table base address field. This value must be a multiple of 0x200. */
#define VECT_TAB_BASE_ADDRESS SRAM1_BASE
#else
/** Vector table base address field. This value must be a multiple of 0x200. */
#define VECT_TAB_BASE_ADDRESS FLASH_BASE
#endif /* VECT_TAB_SRAM */

#if !defined(VECT_TAB_OFFSET)
/** Vector table offset field. This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET 0x00000000U
#endif /* VECT_TAB_OFFSET */

#endif /* USER_VECT_TAB_ADDRESS */

/**
 * @brief  The system core clock value in Hz.
 *
 * The SystemCoreClock variable is updated in three ways:
 * 1. By calling CMSIS function SystemCoreClockUpdate().
 * 2. By calling HAL API function HAL_RCC_GetHCLKFreq().
 * 3. Each time HAL_RCC_ClockConfig() is called to configure the system clock
 *    frequency.
 *
 * @note  If you use this function to configure the system clock; then there is
 *        no need to call the first two functions listed above, because the
 *        SystemCoreClock variable is updated automatically.
 */
uint32_t SystemCoreClock = 4000000U;

/** The AHB prescaler values. */
const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
                                   1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};

/** The APB prescaler values. */
const uint8_t APBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};

/** The MSI iscillator values values. */
const uint32_t MSIRangeTable[12] = {100000U,   200000U,   400000U,   800000U,
                                    1000000U,  2000000U,  4000000U,  8000000U,
                                    16000000U, 24000000U, 32000000U, 48000000U};

/**
 * @brief  Initialize the system before calling the main() function.
 */
void SystemInit(void)
{
#if defined(USER_VECT_TAB_ADDRESS)
    // Configure the vector table location
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif

    // Configure the FPU settings
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |=
        ((3UL << 20U) | (3UL << 22U));  // Set CP10 and CP11 full access
#endif
}

/**
 * @brief  Update the SystemCoreClock variable according to the clock register
 *         values.
 *
 * The SystemCoreClock variable contains the core clock (HCLK), it can be used
 * by the user application to setup the SysTick timer or configure other
 * parameters.
 *
 * Each time the core clock (HCLK) changes, this function must be called to
 * update SystemCoreClock variable value. Otherwise, any configuration based
 * on this variable will be incorrect.
 *
 * The system frequency computed by this function is not the real frequency in
 * the chip. It is calculated based on the predefined constant and the selected
 * clock source:
 * - If SYSCLK source is MSI, SystemCoreClock will contain the MSI_VALUE (*)
 * - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE (**)
 * - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE (***)
 * - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE (***)
 *   or HSI_VALUE (*) or MSI_VALUE (*) multiplied/divided by the PLL factors.
 *
 * (*) MSI_VALUE is a constant defined in stm32l4xx_hal.h file (default value
 * 4 MHz) but the real value may vary depending on the variations in voltage and
 * temperature.
 *
 * (**) HSI_VALUE is a constant defined in stm32l4xx_hal.h file (default value
 * 16 MHz) but the real value may vary depending on the variations in voltage
 * and temperature.
 *
 * (***) HSE_VALUE is a constant defined in stm32l4xx_hal.h file (default value
 * 8 MHz), user has to ensure that HSE_VALUE is same as the real frequency
 * of the crystal used. Otherwise, this function may have wrong result.
 *
 * @note The result of this function could be not correct when using fractional
 *       value for HSE crystal.
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp;
    uint32_t msirange;
    uint32_t pllvco;
    uint32_t pllsource;
    uint32_t pllm;
    uint32_t pllr;

    // Get MSI range
    if ((RCC->CR & RCC_CR_MSIRGSEL) == 0U)
    {
        // MSISRANGE from RCC_CSR applies
        msirange = (RCC->CSR & RCC_CSR_MSISRANGE) >> 8U;
    }
    else
    {
        // MSIRANGE from RCC_CR applies
        msirange = (RCC->CR & RCC_CR_MSIRANGE) >> 4U;
    }
    // MSI frequency range in Hz
    msirange = MSIRangeTable[msirange];

    // Get SYSCLK source
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
        case 0x00:
            // MSI used as system clock source
            SystemCoreClock = msirange;
            break;

        case 0x04:
            // HSI used as system clock source
            SystemCoreClock = HSI_VALUE;
            break;

        case 0x08:
            // HSE used as system clock source
            SystemCoreClock = HSE_VALUE;
            break;

        case 0x0C:
            // PLL used as system clock  source
            // PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE) / PLLM * PLLN
            // SYSCLK = PLL_VCO / PLLR
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
            pllm      = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> 4U) + 1U;

            switch (pllsource)
            {
                case 0x02:
                    // HSI used as PLL clock source
                    pllvco = (HSI_VALUE / pllm);
                    break;

                case 0x03:
                    // HSE used as PLL clock source
                    pllvco = (HSE_VALUE / pllm);
                    break;

                default:
                    // MSI used as PLL clock source
                    pllvco = (msirange / pllm);
                    break;
            }
            pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 8U);
            pllr   = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> 25U) + 1U) * 2U;
            SystemCoreClock = pllvco / pllr;
            break;

        default:
            SystemCoreClock = msirange;
            break;
    }
    // Compute HCLK clock frequency
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4U)];
    SystemCoreClock >>= tmp;
}

/**
 * @}
 * @}
 * @}
 */
