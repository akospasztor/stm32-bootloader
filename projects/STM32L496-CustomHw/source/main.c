/**
 *******************************************************************************
 * STM32L4 Bootloader
 *******************************************************************************
 * @author Akos Pasztor
 * @file   main.c
 * @brief  Main program
 *	       This file demonstrates the usage of the bootloader.
 *
 * @see    Please refer to README for detailed information.
 *******************************************************************************
 * Copyright (c) 2020 Akos Pasztor.                     https://akospasztor.com
 *******************************************************************************
 */

#include "main.h"
#include "bootloader.h"
#include "fatfs.h"
#include "stm32l4xx.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t BTNcounter = 0;

/* External variables --------------------------------------------------------*/
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFs;  /* File system object for SD logical drive */
extern FIL SDFile;     /* File object for SD */

/* Function prototypes -------------------------------------------------------*/
void Enter_Bootloader(void);
uint8_t SD_Init(void);
void SD_DeInit(void);
void SD_Eject(void);
void GPIO_Init(void);
void GPIO_DeInit(void);
void SystemClock_Config(void);
void Error_Handler(void);
void print(const char* str);

/* Main ----------------------------------------------------------------------*/
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();

    LED_ALL_ON();
    print("\nPower up, Boot started.");
    HAL_Delay(500);
    LED_ALL_OFF();

    /* Check system reset flags */
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST))
    {
        print("OBL flag is active.");
#if(CLEAR_RESET_FLAGS)
        /* Clear system reset flags */
        __HAL_RCC_CLEAR_RESET_FLAGS();
        print("Reset flags cleared.");
#endif
    }

    /* Check for user action:
        - button is pressed >= 1 second:  Enter Bootloader. Green LED is
          blinking.
        - button is pressed >= 4 seconds: Enter ST System Memory. Yellow LED is
          blinking.
        - button is pressed >= 9 seconds: Do nothing, launch application.
    */
    while(IS_BTN_PRESSED() && BTNcounter < 90)
    {
        if(BTNcounter == 10)
        {
            print("Release button to enter Bootloader.");
        }
        if(BTNcounter == 40)
        {
            print("Release button to enter System Memory.");
        }

        if(BTNcounter < 10)
        {
            LED_ALL_ON();
        }
        else if(BTNcounter == 10)
        {
            LED_ALL_OFF();
        }
        else if(BTNcounter < 40)
        {
            LED_G_TG();
        }
        else if(BTNcounter == 40)
        {
            LED_G_OFF();
            LED_Y_ON();
        }
        else
        {
            LED_Y_TG();
        }

        BTNcounter++;
        HAL_Delay(100);
    }

    LED_ALL_OFF();

    /* Perform required actions based on button press duration */
    if(BTNcounter < 90)
    {
        if(BTNcounter > 40)
        {
            print("Entering System Memory...");
            HAL_Delay(1000);
            Bootloader_JumpToSysMem();
        }
        else if(BTNcounter > 10)
        {
            print("Entering Bootloader...");
            Enter_Bootloader();
        }
    }

    /* Check if there is application in user flash area */
    if(Bootloader_CheckForApplication() == BL_OK)
    {
#if(USE_CHECKSUM)
        /* Verify application checksum */
        if(Bootloader_VerifyChecksum() != BL_OK)
        {
            print("Checksum Error.");
            Error_Handler();
        }
        else
        {
            print("Checksum OK.");
        }
#endif

        print("Launching Application.");
        LED_G_ON();
        HAL_Delay(1000);
        LED_G_OFF();

        /* De-initialize bootloader hardware & peripherals */
        SD_DeInit();
        GPIO_DeInit();

        /* Launch application */
        Bootloader_JumpToApplication();
    }

    /* No application found */
    print("No application in flash.");
    while(1)
    {
        LED_R_ON();
        HAL_Delay(150);
        LED_R_OFF();
        HAL_Delay(150);
        LED_R_ON();
        HAL_Delay(150);
        LED_R_OFF();
        HAL_Delay(1050);
    }
}

/*** Bootloader ***************************************************************/
void Enter_Bootloader(void)
{
    FRESULT fr;
    UINT num;
    uint8_t i;
    uint8_t status;
    uint64_t data;
    uint32_t cntr;
    uint32_t addr;
    char msg[40] = {0x00};

    /* Check for flash write protection */
    if(Bootloader_GetProtectionStatus() & BL_PROTECTION_WRP)
    {
        print("Application space in flash is write protected.");
        print("Press button to disable flash write protection...");
        LED_R_ON();
        for(i = 0; i < 100; ++i)
        {
            LED_Y_TG();
            HAL_Delay(50);
            if(IS_BTN_PRESSED())
            {
                print("Disabling write protection and generating system "
                      "reset...");
                Bootloader_ConfigProtection(BL_PROTECTION_NONE);
            }
        }
        LED_R_OFF();
        LED_Y_OFF();
        print("Button was not pressed, write protection is still active.");
        print("Exiting Bootloader.");
        return;
    }

    /* Initialize SD card */
    if(SD_Init())
    {
        /* SD init failed */
        print("SD card cannot be initialized.");
        return;
    }

    /* Mount SD card */
    fr = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
    if(fr != FR_OK)
    {
        /* f_mount failed */
        print("SD card cannot be mounted.");
        sprintf(msg, "FatFs error code: %u", fr);
        print(msg);
        return;
    }
    print("SD mounted.");

    /* Open file for programming */
    fr = f_open(&SDFile, CONF_FILENAME, FA_READ);
    if(fr != FR_OK)
    {
        /* f_open failed */
        print("File cannot be opened.");
        sprintf(msg, "FatFs error code: %u", fr);
        print(msg);

        SD_Eject();
        print("SD ejected.");
        return;
    }
    print("Software found on SD.");

    /* Check size of application found on SD card */
    if(Bootloader_CheckSize(f_size(&SDFile)) != BL_OK)
    {
        print("Error: app on SD card is too large.");

        f_close(&SDFile);
        SD_Eject();
        print("SD ejected.");
        return;
    }
    print("App size OK.");

    /* Step 1: Init Bootloader and Flash */
    Bootloader_Init();

    /* Step 2: Erase Flash */
    print("Erasing flash...");
    LED_Y_ON();
    Bootloader_Erase();
    LED_Y_OFF();
    print("Flash erase finished.");

    /* If BTN is pressed, then skip programming */
    if(IS_BTN_PRESSED())
    {
        print("Programming skipped.");

        f_close(&SDFile);
        SD_Eject();
        print("SD ejected.");
        return;
    }

    /* Step 3: Programming */
    print("Starting programming...");
    LED_Y_ON();
    cntr = 0;
    Bootloader_FlashBegin();
    do
    {
        data = 0xFFFFFFFFFFFFFFFF;
        fr   = f_read(&SDFile, &data, 8, &num);
        if(num)
        {
            status = Bootloader_FlashNext(data);
            if(status == BL_OK)
            {
                cntr++;
            }
            else
            {
                sprintf(msg, "Programming error at: %lu byte", (cntr * 8));
                print(msg);

                f_close(&SDFile);
                SD_Eject();
                print("SD ejected.");

                LED_G_OFF();
                LED_Y_OFF();
                return;
            }
        }
        if(cntr % 256 == 0)
        {
            /* Toggle green LED during programming */
            LED_G_TG();
        }
    } while((fr == FR_OK) && (num > 0));

    /* Step 4: Finalize Programming */
    Bootloader_FlashEnd();
    f_close(&SDFile);
    LED_G_OFF();
    LED_Y_OFF();
    print("Programming finished.");
    sprintf(msg, "Flashed: %lu bytes.", (cntr * 8));
    print(msg);

    /* Open file for verification */
    fr = f_open(&SDFile, CONF_FILENAME, FA_READ);
    if(fr != FR_OK)
    {
        /* f_open failed */
        print("File cannot be opened.");
        sprintf(msg, "FatFs error code: %u", fr);
        print(msg);

        SD_Eject();
        print("SD ejected.");
        return;
    }

    /* Step 5: Verify Flash Content */
    addr = APP_ADDRESS;
    cntr = 0;
    do
    {
        data = 0xFFFFFFFFFFFFFFFF;
        fr   = f_read(&SDFile, &data, 4, &num);
        if(num)
        {
            if(*(uint32_t*)addr == (uint32_t)data)
            {
                addr += 4;
                cntr++;
            }
            else
            {
                sprintf(msg, "Verification error at: %lu byte.", (cntr * 4));
                print(msg);

                f_close(&SDFile);
                SD_Eject();
                print("SD ejected.");

                LED_G_OFF();
                return;
            }
        }
        if(cntr % 256 == 0)
        {
            /* Toggle green LED during verification */
            LED_G_TG();
        }
    } while((fr == FR_OK) && (num > 0));
    print("Verification passed.");
    LED_G_OFF();

    /* Eject SD card */
    SD_Eject();
    print("SD ejected.");

    /* Enable flash write protection */
#if(USE_WRITE_PROTECTION)
    print("Enablig flash write protection and generating system reset...");
    if(Bootloader_ConfigProtection(BL_PROTECTION_WRP) != BL_OK)
    {
        print("Failed to enable write protection.");
        print("Exiting Bootloader.");
    }
#endif
}

/*** SD Card ******************************************************************/
uint8_t SD_Init(void)
{
    SDCARD_ON();

    if(FATFS_Init())
    {
        /* Error */
        return 1;
    }

    if(BSP_SD_Init())
    {
        /* Error */
        return 1;
    }

    return 0;
}

void SD_DeInit(void)
{
    BSP_SD_DeInit();
    FATFS_DeInit();
    SDCARD_OFF();
}

void SD_Eject(void)
{
    f_mount(NULL, (TCHAR const*)SDPath, 0);
}

/*** GPIO Configuration ***/
void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pin output levels */
    HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_Y_Port, LED_Y_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SD_PWR_Port, SD_PWR_Pin, GPIO_PIN_SET);

    /* LED_G_Pin, LED_Y_Pin, LED_R_Pin */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin = LED_G_Pin;
    HAL_GPIO_Init(LED_G_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_Y_Pin;
    HAL_GPIO_Init(LED_Y_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_R_Pin;
    HAL_GPIO_Init(LED_R_Port, &GPIO_InitStruct);

    /* SD Card Power Pin */
    GPIO_InitStruct.Pin   = SD_PWR_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SD_PWR_Port, &GPIO_InitStruct);

    /* User Button */
    GPIO_InitStruct.Pin   = BTN_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BTN_Port, &GPIO_InitStruct);
}
void GPIO_DeInit(void)
{
    HAL_GPIO_DeInit(BTN_Port, BTN_Pin);
    HAL_GPIO_DeInit(LED_G_Port, LED_G_Pin);
    HAL_GPIO_DeInit(LED_Y_Port, LED_Y_Pin);
    HAL_GPIO_DeInit(LED_R_Port, LED_R_Pin);
    HAL_GPIO_DeInit(SD_PWR_Port, SD_PWR_Pin);

    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
}

/*** System Clock Configuration ***/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /* Initializes the CPU, AHB and APB bus clocks */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 24;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection    = RCC_PERIPHCLK_SDMMC1;
    PeriphClkInit.Sdmmc1ClockSelection    = RCC_SDMMC1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source   = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M        = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N        = 24;
    PeriphClkInit.PLLSAI1.PLLSAI1P        = RCC_PLLP_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1Q        = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R        = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the main internal regulator output voltage */
    if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the Systick */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/*** HAL MSP init ***/
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/*** Debug ***/
void print(const char* str)
{
#if(USE_SWO_TRACE)
    puts(str);
#endif
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
    while(1)
    {
        LED_R_TG();
        HAL_Delay(500);
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
}

#endif
