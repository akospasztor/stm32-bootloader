/**
 *******************************************************************************
 * STM32L4 Bootloader
 *******************************************************************************
 * @author Akos Pasztor
 * @file   stm32l4xx_it.c
 * @brief  Interrupt Service Routines
 *	       This file contains the exception and peripheral interrupt handlers.
 *
 *
 *******************************************************************************
 * Copyright (c) 2020 Akos Pasztor.                     https://akospasztor.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_it.h"
#include "stm32l4xx_hal.h"

/* External variables --------------------------------------------------------*/
extern SD_HandleTypeDef hsd1;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while(1)
    {
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    while(1)
    {
    }
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    while(1)
    {
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    while(1)
    {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
 * @brief DMA2 Channel5 ISR
 * @note  SDMMC DMA Tx, Rx
 */
void DMA2_Channel5_IRQHandler(void)
{
    if((hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_SINGLE_BLOCK)) ||
       (hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_MULTIPLE_BLOCK)))
    {
        HAL_DMA_IRQHandler(hsd1.hdmarx);
    }
    else if((hsd1.Context ==
             (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_SINGLE_BLOCK)) ||
            (hsd1.Context ==
             (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_MULTIPLE_BLOCK)))
    {
        HAL_DMA_IRQHandler(hsd1.hdmatx);
    }
}

/**
 * @brief SDMMC ISR
 */
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&hsd1);
}
