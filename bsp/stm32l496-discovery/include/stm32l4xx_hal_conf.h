/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the STM32L4xx HAL library configuration,
 *          including module selection and oscillator value definitions.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef STM32L4XX_HAL_CONF_H
#define STM32L4XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bsp BSP
 * @{
 * @addtogroup bsp_stm32l496_discovery STM32L496-Discovery
 * @{
 * @addtogroup bsp_stm32l496_discovery_stm32l4xx_hal_conf STM32L4xx_HAL_Conf
 * @{
 */

/* Module selection ----------------------------------------------------------*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define HAL_MODULE_ENABLED
// #define HAL_ADC_MODULE_ENABLED
// #define HAL_CAN_MODULE_ENABLED
// #define HAL_COMP_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
// #define HAL_CRYP_MODULE_ENABLED
// #define HAL_DAC_MODULE_ENABLED
// #define HAL_DCMI_MODULE_ENABLED
// #define HAL_DFSDM_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
// #define HAL_DMA2D_MODULE_ENABLED
// #define HAL_DSI_MODULE_ENABLED
// #define HAL_FIREWALL_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
// #define HAL_GFXMMU_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
// #define HAL_HASH_MODULE_ENABLED
// #define HAL_HCD_MODULE_ENABLED
// #define HAL_I2C_MODULE_ENABLED
// #define HAL_I2S_MODULE_ENABLED
// #define HAL_IRDA_MODULE_ENABLED
// #define HAL_IWDG_MODULE_ENABLED
// #define HAL_LCD_MODULE_ENABLED
// #define HAL_LPTIM_MODULE_ENABLED
// #define HAL_LTDC_MODULE_ENABLED
// #define HAL_NAND_MODULE_ENABLED
// #define HAL_NOR_MODULE_ENABLED
// #define HAL_OPAMP_MODULE_ENABLED
// #define HAL_OSPI_MODULE_ENABLED
// #define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
// #define HAL_QSPI_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
// #define HAL_RNG_MODULE_ENABLED
// #define HAL_RTC_MODULE_ENABLED
// #define HAL_SAI_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
// #define HAL_SMARTCARD_MODULE_ENABLED
// #define HAL_SMBUS_MODULE_ENABLED
// #define HAL_SPI_MODULE_ENABLED
// #define HAL_SRAM_MODULE_ENABLED
// #define HAL_SWPMI_MODULE_ENABLED
// #define HAL_TIM_MODULE_ENABLED
// #define HAL_TSC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
// #define HAL_USART_MODULE_ENABLED
// #define HAL_WWDG_MODULE_ENABLED
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/* Oscillator values adaptation ----------------------------------------------*/
/**
 * @brief Value of the External oscillator in Hz.
 *
 * Adjust the value of External High Speed oscillator (HSE) used in your
 * application. This value is used by the RCC HAL module to compute the system
 * frequency (when HSE is used as system clock source, directly or through the
 * PLL).
 */
#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)8000000U)
#endif /* HSE_VALUE */

/**
 * @brief Timeout for HSE start up, in ms.
 */
#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT ((uint32_t)100U)
#endif /* HSE_STARTUP_TIMEOUT */

/**
 * @brief Internal Multiple Speed oscillator (MSI) default value in Hz.
 *
 * This value is the default MSI range value after Reset.
 */
#if !defined(MSI_VALUE)
#define MSI_VALUE ((uint32_t)4000000U)
#endif /* MSI_VALUE */

/**
 * @brief Internal High Speed oscillator (HSI) value in Hz.
 *
 * This value is used by the RCC HAL module to compute the system frequency
 * (when HSI is used as system clock source, directly or through the PLL).
 */
#if !defined(HSI_VALUE)
#define HSI_VALUE ((uint32_t)16000000U)
#endif /* HSI_VALUE */

/**
 * @brief Internal High Speed oscillator (HSI48) value for USB FS, SDMMC and RNG
 *        in Hz.
 *
 * This internal oscillator is mainly dedicated to provide a high precision
 * clock to the USB peripheral by means of a special Clock Recovery System (CRS)
 * circuitry. When the CRS is not used, the HSI48 RC oscillator runs on it
 * default frequency which is subject to manufacturing process variations.
 */
#if !defined(HSI48_VALUE)
#define HSI48_VALUE ((uint32_t)48000000U)
#endif /* HSI48_VALUE */

/**
 * @brief Internal Low Speed oscillator (LSI) value in Hz.
 *
 * The real value may vary depending on the variations in voltage and
 * temperature.
 */
#if !defined(LSI_VALUE)
#define LSI_VALUE ((uint32_t)32000U)
#endif /* LSI_VALUE */

/**
 * @brief External Low Speed oscillator (LSE) value in Hz.
 *
 * This value is used by the UART, RTC HAL module to compute the system
 * frequency.
 */
#if !defined(LSE_VALUE)
#define LSE_VALUE ((uint32_t)32768U)
#endif /* LSE_VALUE */

/**
 * @brief Time out for LSE start up, in ms.
 */
#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000U)
#endif /* HSE_STARTUP_TIMEOUT */

/**
 * @brief External clock source for SAI1 peripheral in Hz.
 *
 * This value is used by the RCC HAL module to compute the SAI1 & SAI2 clock
 * source frequency.
 */
#if !defined(EXTERNAL_SAI1_CLOCK_VALUE)
#define EXTERNAL_SAI1_CLOCK_VALUE ((uint32_t)2097000U)
#endif /* EXTERNAL_SAI1_CLOCK_VALUE */

/**
 * @brief External clock source for SAI2 peripheral in Hz.
 *
 * This value is used by the RCC HAL module to compute the SAI1 & SAI2 clock
 * source frequency.
 */
#if !defined(EXTERNAL_SAI2_CLOCK_VALUE)
#define EXTERNAL_SAI2_CLOCK_VALUE ((uint32_t)2097000U)
#endif /* EXTERNAL_SAI2_CLOCK_VALUE */

/* Tip: To avoid modifying this file each time you need to use different HSE,
   ===  you can define the HSE value in your toolchain compiler preprocessor. */

/* System configuration ------------------------------------------------------*/
#define VDD_VALUE                3300U /**< Value of VDD in mV. */
#define TICK_INT_PRIORITY        0U    /**< Tick interrupt priority. */
#define USE_RTOS                 0U    /**< Use the HAL RTOS interface. */
#define PREFETCH_ENABLE          0U    /**< Enable the prefetch. */
#define INSTRUCTION_CACHE_ENABLE 1U    /**< Enable the instruction cache. */
#define DATA_CACHE_ENABLE        1U    /**< Enable the data cache. */

/* Assert selection ----------------------------------------------------------*/
// Uncomment the line below to expand the "assert_param" macro in the HAL driver
// Note: This define is specified in the CMakeLists.txt file depending on the
// build configuration.
/* #define USE_FULL_ASSERT    1U */

/* Callback feature configuration --------------------------------------------*/
// Set below the peripheral configuration  to "1U" to add the support
// of HAL callback registration/deregistration feature for the HAL
// driver(s). This allows user application to provide specific callback
// functions thanks to HAL_PPP_RegisterCallback() rather than overwriting
// the default weak callback functions (see each stm32l4xx_hal_ppp.h file
// for possible callback identifiers defined in HAL_PPP_CallbackIDTypeDef
// for each PPP peripheral).
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define USE_HAL_ADC_REGISTER_CALLBACKS       0U
#define USE_HAL_CAN_REGISTER_CALLBACKS       0U
#define USE_HAL_COMP_REGISTER_CALLBACKS      0U
#define USE_HAL_CRYP_REGISTER_CALLBACKS      0U
#define USE_HAL_DAC_REGISTER_CALLBACKS       0U
#define USE_HAL_DCMI_REGISTER_CALLBACKS      0U
#define USE_HAL_DFSDM_REGISTER_CALLBACKS     0U
#define USE_HAL_DMA2D_REGISTER_CALLBACKS     0U
#define USE_HAL_DSI_REGISTER_CALLBACKS       0U
#define USE_HAL_GFXMMU_REGISTER_CALLBACKS    0U
#define USE_HAL_HASH_REGISTER_CALLBACKS      0U
#define USE_HAL_HCD_REGISTER_CALLBACKS       0U
#define USE_HAL_I2C_REGISTER_CALLBACKS       0U
#define USE_HAL_IRDA_REGISTER_CALLBACKS      0U
#define USE_HAL_LPTIM_REGISTER_CALLBACKS     0U
#define USE_HAL_LTDC_REGISTER_CALLBACKS      0U
#define USE_HAL_MMC_REGISTER_CALLBACKS       0U
#define USE_HAL_OPAMP_REGISTER_CALLBACKS     0U
#define USE_HAL_OSPI_REGISTER_CALLBACKS      0U
#define USE_HAL_PCD_REGISTER_CALLBACKS       0U
#define USE_HAL_QSPI_REGISTER_CALLBACKS      0U
#define USE_HAL_RNG_REGISTER_CALLBACKS       0U
#define USE_HAL_RTC_REGISTER_CALLBACKS       0U
#define USE_HAL_SAI_REGISTER_CALLBACKS       0U
#define USE_HAL_SD_REGISTER_CALLBACKS        0U
#define USE_HAL_SMARTCARD_REGISTER_CALLBACKS 0U
#define USE_HAL_SMBUS_REGISTER_CALLBACKS     0U
#define USE_HAL_SPI_REGISTER_CALLBACKS       0U
#define USE_HAL_SWPMI_REGISTER_CALLBACKS     0U
#define USE_HAL_TIM_REGISTER_CALLBACKS       0U
#define USE_HAL_TSC_REGISTER_CALLBACKS       0U
#define USE_HAL_UART_REGISTER_CALLBACKS      0U
#define USE_HAL_USART_REGISTER_CALLBACKS     0U
#define USE_HAL_WWDG_REGISTER_CALLBACKS      0U
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/* SPI peripheral configuration ----------------------------------------------*/
#define USE_SPI_CRC 0U /**< Enable the CRC feature inside HAL SPI driver. */

/* Includes ------------------------------------------------------------------*/

#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_rcc_ex.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32l4xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
#include "stm32l4xx_hal_dma.h"
#include "stm32l4xx_hal_dma_ex.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_DFSDM_MODULE_ENABLED
#include "stm32l4xx_hal_dfsdm.h"
#endif /* HAL_DFSDM_MODULE_ENABLED */

#ifdef HAL_CORTEX_MODULE_ENABLED
#include "stm32l4xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifdef HAL_ADC_MODULE_ENABLED
#include "stm32l4xx_hal_adc.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_CAN_MODULE_ENABLED
#include "stm32l4xx_hal_can.h"
#endif /* HAL_CAN_MODULE_ENABLED */

#ifdef HAL_COMP_MODULE_ENABLED
#include "stm32l4xx_hal_comp.h"
#endif /* HAL_COMP_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
#include "stm32l4xx_hal_crc.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_CRYP_MODULE_ENABLED
#include "stm32l4xx_hal_cryp.h"
#endif /* HAL_CRYP_MODULE_ENABLED */

#ifdef HAL_DAC_MODULE_ENABLED
#include "stm32l4xx_hal_dac.h"
#endif /* HAL_DAC_MODULE_ENABLED */

#ifdef HAL_DCMI_MODULE_ENABLED
#include "stm32l4xx_hal_dcmi.h"
#endif /* HAL_DCMI_MODULE_ENABLED */

#ifdef HAL_DMA2D_MODULE_ENABLED
#include "stm32l4xx_hal_dma2d.h"
#endif /* HAL_DMA2D_MODULE_ENABLED */

#ifdef HAL_DSI_MODULE_ENABLED
#include "stm32l4xx_hal_dsi.h"
#endif /* HAL_DSI_MODULE_ENABLED */

#ifdef HAL_FIREWALL_MODULE_ENABLED
#include "stm32l4xx_hal_firewall.h"
#endif /* HAL_FIREWALL_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32l4xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_HASH_MODULE_ENABLED
#include "stm32l4xx_hal_hash.h"
#endif /* HAL_HASH_MODULE_ENABLED */

#ifdef HAL_SRAM_MODULE_ENABLED
#include "stm32l4xx_hal_sram.h"
#endif /* HAL_SRAM_MODULE_ENABLED */

#ifdef HAL_NOR_MODULE_ENABLED
#include "stm32l4xx_hal_nor.h"
#endif /* HAL_NOR_MODULE_ENABLED */

#ifdef HAL_NAND_MODULE_ENABLED
#include "stm32l4xx_hal_nand.h"
#endif /* HAL_NAND_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
#include "stm32l4xx_hal_i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_IWDG_MODULE_ENABLED
#include "stm32l4xx_hal_iwdg.h"
#endif /* HAL_IWDG_MODULE_ENABLED */

#ifdef HAL_LCD_MODULE_ENABLED
#include "stm32l4xx_hal_lcd.h"
#endif /* HAL_LCD_MODULE_ENABLED */

#ifdef HAL_LPTIM_MODULE_ENABLED
#include "stm32l4xx_hal_lptim.h"
#endif /* HAL_LPTIM_MODULE_ENABLED */

#ifdef HAL_LTDC_MODULE_ENABLED
#include "stm32l4xx_hal_ltdc.h"
#endif /* HAL_LTDC_MODULE_ENABLED */

#ifdef HAL_OPAMP_MODULE_ENABLED
#include "stm32l4xx_hal_opamp.h"
#endif /* HAL_OPAMP_MODULE_ENABLED */

#ifdef HAL_OSPI_MODULE_ENABLED
#include "stm32l4xx_hal_ospi.h"
#endif /* HAL_OSPI_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
#include "stm32l4xx_hal_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_QSPI_MODULE_ENABLED
#include "stm32l4xx_hal_qspi.h"
#endif /* HAL_QSPI_MODULE_ENABLED */

#ifdef HAL_RNG_MODULE_ENABLED
#include "stm32l4xx_hal_rng.h"
#endif /* HAL_RNG_MODULE_ENABLED */

#ifdef HAL_RTC_MODULE_ENABLED
#include "stm32l4xx_hal_rtc.h"
#endif /* HAL_RTC_MODULE_ENABLED */

#ifdef HAL_SAI_MODULE_ENABLED
#include "stm32l4xx_hal_sai.h"
#endif /* HAL_SAI_MODULE_ENABLED */

#ifdef HAL_SD_MODULE_ENABLED
#include "stm32l4xx_hal_sd.h"
#endif /* HAL_SD_MODULE_ENABLED */

#ifdef HAL_SMBUS_MODULE_ENABLED
#include "stm32l4xx_hal_smbus.h"
#endif /* HAL_SMBUS_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
#include "stm32l4xx_hal_spi.h"
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_SWPMI_MODULE_ENABLED
#include "stm32l4xx_hal_swpmi.h"
#endif /* HAL_SWPMI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
#include "stm32l4xx_hal_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_TSC_MODULE_ENABLED
#include "stm32l4xx_hal_tsc.h"
#endif /* HAL_TSC_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
#include "stm32l4xx_hal_uart.h"
#endif /* HAL_UART_MODULE_ENABLED */

#ifdef HAL_USART_MODULE_ENABLED
#include "stm32l4xx_hal_usart.h"
#endif /* HAL_USART_MODULE_ENABLED */

#ifdef HAL_IRDA_MODULE_ENABLED
#include "stm32l4xx_hal_irda.h"
#endif /* HAL_IRDA_MODULE_ENABLED */

#ifdef HAL_SMARTCARD_MODULE_ENABLED
#include "stm32l4xx_hal_smartcard.h"
#endif /* HAL_SMARTCARD_MODULE_ENABLED */

#ifdef HAL_WWDG_MODULE_ENABLED
#include "stm32l4xx_hal_wwdg.h"
#endif /* HAL_WWDG_MODULE_ENABLED */

#ifdef HAL_PCD_MODULE_ENABLED
#include "stm32l4xx_hal_pcd.h"
#endif /* HAL_PCD_MODULE_ENABLED */

#ifdef HAL_HCD_MODULE_ENABLED
#include "stm32l4xx_hal_hcd.h"
#endif /* HAL_HCD_MODULE_ENABLED */

#ifdef HAL_GFXMMU_MODULE_ENABLED
#include "stm32l4xx_hal_gfxmmu.h"
#endif /* HAL_GFXMMU_MODULE_ENABLED */

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief  The assert_param macro is used for function parameter check.
 *
 * @param  expr  If expr is false, it calls assert_failed functionwhich reports
 *               the name of the source file and the source line number of the
 *               call that failed. If expr is true, it returns no value.
 */
#ifdef USE_FULL_ASSERT
// Message:     A conversion shall not remove any const, volatile or _Atomic
//              qualification from the type pointed to by a pointer
//              [misra-c2012-11.8]
// Reason:      The assert_param macro is only used in debug builds. The
//              expression is only tested and never modified.
// Risk:        Removing a const qualifier might circumvent the read-only
//              status of an object and result in it being modified.
//              Removing a const qualifier might result in an exception when
//              the object is accessed.
// Prevention:  None.
// cppcheck-suppress-macro [misra-c2012-11.8]
#define assert_param(expr) \
    ((expr) ? (void)0U : assert_failed((uint8_t*)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
