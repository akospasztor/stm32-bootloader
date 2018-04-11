/**
  ******************************************************************************
  * STM32L4 Bootloader
  ******************************************************************************
  * @author Akos Pasztor
  * @file   main.c
  * @brief  Main program
  *	        This file demonstrates the usage of the bootloader.
  *
  * @see    Please refer to README for detailed information. 
  ******************************************************************************
  * Copyright (c) 2018 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

#include "stm32l4xx.h"
#include "main.h"
#include "bootloader.h"
#include "fatfs.h"

/* Variables -----------------------------------------------------------------*/
static uint8_t BTNcounter = 0;

/* Function prototypes -------------------------------------------------------*/
void    Enter_Bootloader(void);
uint8_t SDMMC1_Init(void);
void    SDMMC1_DeInit(void);
void    GPIO_Init(void);
void    GPIO_DeInit(void);
void    SystemClock_Config(void);
void    Error_Handler(void);
void    print(const char* str);

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
#if (CLEAR_RESET_FLAGS)
        /* Clear system reset flags */
        __HAL_RCC_CLEAR_RESET_FLAGS();
        print("Reset flags cleared.");
#endif
    }
    
    /* Check for user action:
        - button is pressed >= 1 second:  Enter Bootloader
        - button is pressed >= 4 seconds: Enter ST System Memory
        - button is pressed >= 9 seconds: Do nothing, launch application
    */
    while(IS_BTN_PRESSED() && BTNcounter < 90)
    {
        if(BTNcounter == 10) { print("Release button to enter Bootloader."); }
        if(BTNcounter == 40) { print("Release button to enter System Memory."); }
        
        if(BTNcounter < 10)         { LED_ALL_ON(); }
        else if(BTNcounter == 10)   { LED_ALL_OFF(); }
        else if(BTNcounter < 40)    { LED_G_TG(); }
        else if(BTNcounter == 40)   { LED_G_OFF(); LED_Y_ON(); }
        else                        { LED_Y_TG(); }
        
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
        
#if (USE_CHECKSUM)
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
        SDMMC1_DeInit();
        GPIO_DeInit();
        
        /* Launch application */
        Bootloader_JumpToApplication();
    }

    /* No application found */
    print("No application in flash.");
    while(1)
    {
        LED_R_ON();     HAL_Delay(150);
        LED_R_OFF();    HAL_Delay(150);
        LED_R_ON();     HAL_Delay(150);
        LED_R_OFF();    HAL_Delay(1050);
    }
}

/*** Bootloader ***************************************************************/
void Enter_Bootloader(void)
{
    FATFS FatFs;
    FIL fil;
    FRESULT fr;
    UINT num;
    uint8_t  i;
    uint64_t data;
    uint32_t cntr = 0;
    char msg[40] = {0x00};
    
    /* Check for flash write protection */
    if(Bootloader_GetProtectionStatus() & BL_PROTECTION_WRP)
    {
        print("Application space in flash is write protected.");
        print("Press button to disable flash write protection...");
        LED_R_ON();
        for(i=0; i<100; ++i)
        {
            LED_Y_TG();
            HAL_Delay(50);
            if(IS_BTN_PRESSED())
            {
                print("Disabling write protection and generating system reset...");
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
    if(!SDMMC1_Init())
    {        
        /* Mount SD card */
        if(f_mount(&FatFs, "", 1) == FR_OK)
        {
            print("SD mounted.");
            /* Open file */
            fr = f_open(&fil, CONF_FILENAME, FA_READ);
            if(fr == FR_OK)
            {
                print("Software found on SD.");
                /* Check application size found on SD card */
                if(Bootloader_CheckSize( f_size(&fil) ) == BL_OK)
                {
                    print("App size OK.");
                    
                    /* Init Bootloader and Flash */
                    Bootloader_Init();
                    
                    /* Erase Flash */
                    print("Erasing flash...");
                    LED_Y_ON();
                    Bootloader_Erase();
                    LED_Y_OFF();
                    print("Flash erase finished.");
                    
                    /* If BTN is pressed, then skip programming */
                    if(IS_BTN_PRESSED())
                    {
                        print("Programming skipped.");
                        f_close(&fil);
                        return;
                    }
                    
                    /* Programming */
                    print("Starting programming...");
                    LED_Y_ON();
                    Bootloader_FlashBegin();
                    do
                    {
                        data = 0xFFFFFFFFFFFFFFFF;
                        fr = f_read(&fil, &data, 8, &num);
                        if(num)
                        {
                            uint8_t status = Bootloader_FlashNext(data);
                            if(status == BL_OK)
                            {
                                cntr++;
                            }
                            else
                            {
                                sprintf(msg, "Error at: %u", cntr);
                                print(msg);
                            }
                        }
                        if(cntr % 256 == 0)
                        {
                            /* Toggle green LED during programming */
                            LED_G_TG();
                        }
                    } while((fr == FR_OK) && (num > 0));
                    print("Programming finished.");
                    sprintf(msg, "Flashed: %u of (uint64_t) data.", cntr);
                    print(msg);
                    Bootloader_FlashEnd();
                    LED_G_OFF();
                    LED_Y_OFF();
                }
                f_close(&fil);
                
            } 
            else /* f_open fails */
            {
                print("File cannot be opened.");
            }
            
            f_mount(NULL, "", 1);
            print("SD dismounted.");
            
        } 
        else /* f_mount fails */
        { 
            print("SD card cannot be mounted.");
        }
        
    }
    else /* SDMMC1_Init fails */
    { 
        print("SD card cannot be initialized.");
    }
    
    /* Enable flash write protection */
#if (USE_WRITE_PROTECTION)
    print("Enablig flash write protection and generating system reset...");
    if(Bootloader_ConfigProtection(BL_PROTECTION_WRP) != BL_OK)
    {
        print("Failed to enable write protection.");
        print("Exiting Bootloader.");
    }
#endif
}

/*** SDIO *********************************************************************/
uint8_t SDMMC1_Init(void)
{
    SDCARD_ON();
    
    if(BSP_SD_Init())
    {
        /* Error */
        return 1;
    }
    
    if(FATFS_Init())
    {
        /* Error */
        return 1;
    }
    
    return 0;
}
void SDMMC1_DeInit(void)
{
    FATFS_DeInit();
    BSP_SD_DeInit();
    SDCARD_OFF();
}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hsd->Instance==SDMMC1)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_SDMMC1_CLK_ENABLE();

        /* SDMMC1 GPIO Configuration    
        PC8     ---> SDMMC1_D0
        PC9     ---> SDMMC1_D1
        PC10    ---> SDMMC1_D2
        PC11    ---> SDMMC1_D3
        PC12    ---> SDMMC1_CK
        PD2     ---> SDMMC1_CMD 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
    if(hsd->Instance==SDMMC1)
    {
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
    }
}

/*** GPIO Configuration ***/
void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pin output levels */
    HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_Y_Port, LED_Y_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SD_PWR_Port, SD_PWR_Pin, GPIO_PIN_SET);
    
    /* LED_G_Pin, LED_Y_Pin, LED_R_Pin */
    GPIO_InitStruct.Pin = LED_G_Pin | LED_Y_Pin | LED_R_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    /* SD Card Power Pin */
    GPIO_InitStruct.Pin = SD_PWR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SD_PWR_Port, &GPIO_InitStruct);
    
    /* User Button */
    GPIO_InitStruct.Pin = BTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BTN_Port, &GPIO_InitStruct);
}
void GPIO_DeInit(void)
{
    HAL_GPIO_DeInit(LED_G_Port, LED_G_Pin);
    HAL_GPIO_DeInit(LED_Y_Port, LED_Y_Pin);
    HAL_GPIO_DeInit(LED_R_Port, LED_R_Pin);
    HAL_GPIO_DeInit(BTN_Port, BTN_Pin);
    
    __HAL_RCC_GPIOE_CLK_DISABLE();
}

/*** System Clock Configuration ***/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /* Initializes the CPU, AHB and APB bus clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 24;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
    PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
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
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
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
#if (USE_SWO_TRACE)
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