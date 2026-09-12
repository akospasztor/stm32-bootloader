/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author Akos Pasztor
 * @brief  This file contains the board initialization and de-initialization
 *         implementation, including system clock configuration, GPIO pinout
 *         setup, and peripheral initialization for the STM32L496 Discovery
 *         board.
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
 * @addtogroup bsp_stm32l496_discovery_board
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"

#include "error_handler.h"
#include "led.h"
#include "log.h"
#include "rcc.h"
#include "systick.h"

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Initialize the board GPIO pinout including button, LEDs, UART, and
 *         SDMMC pins.
 */
static void PinoutInit(void);

/**
 * @brief  De-initialize all board GPIO pins and disable the GPIO port clocks.
 */
static void PinoutDeInit(void);

/**
 * @brief  Configure the system clock, PLL, and peripheral clocks.
 */
static void SystemClockConfig(void);

/* Functions -----------------------------------------------------------------*/
void BoardInit(void)
{
    (void)HAL_Init();

    SystemClockConfig();
    SystickInit();
    PinoutInit();
    LogInit();
}

void BoardDeInit(void)
{
    LogDeInit();
    PinoutDeInit();

    RccDisableFlash();
    RccDisablePwr();
    RccDisableSysCfg();

    (void)HAL_RCC_DeInit();
    SystickDeInit();
    (void)HAL_DeInit();
}

void SetVectorTableLocation(const uint32_t address)
{
    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The following defines are addressing the memory mapped
    //              registers of the microcontroller.
    // Risk:        Conversion of a pointer to object into an integer may
    //              produce a value that cannot be represented in the chosen
    //              integer type resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    SCB->VTOR = address;
}

static void PinoutInit(void)
{
    GPIO_InitTypeDef gpioInitStruct = {0};

    RccEnablePortA();
    RccEnablePortB();
    RccEnablePortC();
    RccEnablePortD();

    // Configure inputs
    gpioInitStruct.Pin  = BUTTON_GPIO_PIN;
    gpioInitStruct.Mode = GPIO_MODE_INPUT;
    gpioInitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &gpioInitStruct);

    // Configure outputs
    LedGreen1Off();
    gpioInitStruct.Pin   = LED_GREEN1_GPIO_PIN;
    gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpioInitStruct.Pull  = GPIO_NOPULL;
    gpioInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GREEN1_GPIO_PORT, &gpioInitStruct);

    LedGreen2Off();
    gpioInitStruct.Pin   = LED_GREEN2_GPIO_PIN;
    gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpioInitStruct.Pull  = GPIO_NOPULL;
    gpioInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GREEN2_GPIO_PORT, &gpioInitStruct);

    // Configure alternate functions for UART pins
    gpioInitStruct.Mode      = GPIO_MODE_AF_PP;
    gpioInitStruct.Pull      = GPIO_NOPULL;
    gpioInitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInitStruct.Alternate = VCP_UART_GPIO_AF;

    gpioInitStruct.Pin = VCP_UART_GPIO_TX_PIN;
    HAL_GPIO_Init(VCP_UART_GPIO_TX_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = VCP_UART_GPIO_RX_PIN;
    HAL_GPIO_Init(VCP_UART_GPIO_RX_PORT, &gpioInitStruct);

    // Configure alternate functions for SDMMC pins
    gpioInitStruct.Mode      = GPIO_MODE_AF_PP;
    gpioInitStruct.Pull      = GPIO_PULLUP;
    gpioInitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInitStruct.Alternate = SDMMC_GPIO_AF;

    gpioInitStruct.Pin = SDMMC_GPIO_D0_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_D0_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = SDMMC_GPIO_D1_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_D1_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = SDMMC_GPIO_D2_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_D2_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = SDMMC_GPIO_D3_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_D3_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = SDMMC_GPIO_CLK_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_CLK_PORT, &gpioInitStruct);

    gpioInitStruct.Pin = SDMMC_GPIO_CMD_PIN;
    HAL_GPIO_Init(SDMMC_GPIO_CMD_PORT, &gpioInitStruct);
}

static void PinoutDeInit(void)
{
    HAL_GPIO_DeInit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN);
    HAL_GPIO_DeInit(LED_GREEN1_GPIO_PORT, LED_GREEN1_GPIO_PIN);
    HAL_GPIO_DeInit(LED_GREEN2_GPIO_PORT, LED_GREEN2_GPIO_PIN);
    HAL_GPIO_DeInit(VCP_UART_GPIO_TX_PORT, VCP_UART_GPIO_TX_PIN);
    HAL_GPIO_DeInit(VCP_UART_GPIO_RX_PORT, VCP_UART_GPIO_RX_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_D0_PORT, SDMMC_GPIO_D0_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_D1_PORT, SDMMC_GPIO_D1_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_D2_PORT, SDMMC_GPIO_D2_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_D3_PORT, SDMMC_GPIO_D3_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_CLK_PORT, SDMMC_GPIO_CLK_PIN);
    HAL_GPIO_DeInit(SDMMC_GPIO_CMD_PORT, SDMMC_GPIO_CMD_PIN);

    RccDisablePortA();
    RccDisablePortB();
    RccDisablePortC();
    RccDisablePortD();
}

static void SystemClockConfig(void)
{
    RCC_OscInitTypeDef rccOscInitStruct;
    RCC_ClkInitTypeDef rccClkInitStruct;
    RCC_PeriphCLKInitTypeDef rccPeriphClkInit;

    RccEnableFlash();
    RccEnablePwr();
    RccEnableSysCfg();

    /* Initializes the CPU, AHB and APB bus clocks */
    rccOscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
    rccOscInitStruct.MSIState            = RCC_MSI_ON;
    rccOscInitStruct.MSICalibrationValue = 0;
    rccOscInitStruct.MSIClockRange       = RCC_MSIRANGE_6;
    rccOscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    rccOscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
    rccOscInitStruct.PLL.PLLM            = 1;
    rccOscInitStruct.PLL.PLLN            = 24;
    rccOscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    rccOscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    rccOscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&rccOscInitStruct) != HAL_OK)
    {
        ErrorHandler();
    }

    rccClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    rccClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    rccClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    rccClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    rccClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&rccClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        ErrorHandler();
    }

    rccPeriphClkInit.PeriphClockSelection =
        RCC_PERIPHCLK_SDMMC1 | RCC_PERIPHCLK_USART2;
    rccPeriphClkInit.Usart2ClockSelection    = RCC_USART2CLKSOURCE_PCLK1;
    rccPeriphClkInit.Sdmmc1ClockSelection    = RCC_SDMMC1CLKSOURCE_PLLSAI1;
    rccPeriphClkInit.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
    rccPeriphClkInit.PLLSAI1.PLLSAI1M        = 1;
    rccPeriphClkInit.PLLSAI1.PLLSAI1N        = 24;
    rccPeriphClkInit.PLLSAI1.PLLSAI1P        = RCC_PLLP_DIV2;
    rccPeriphClkInit.PLLSAI1.PLLSAI1Q        = RCC_PLLQ_DIV2;
    rccPeriphClkInit.PLLSAI1.PLLSAI1R        = RCC_PLLR_DIV2;
    rccPeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&rccPeriphClkInit) != HAL_OK)
    {
        ErrorHandler();
    }

    /* Configure the main internal regulator output voltage */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        ErrorHandler();
    }
}

/**
 * @}
 * @}
 * @}
 */
