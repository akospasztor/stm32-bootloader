/**
  ******************************************************************************
  * STM32L4 Bootloader
  ******************************************************************************
  * @author Akos Pasztor
  * @file   stm32l4xx_it.c
  * @brief  Interrupt Service Routines
  *	        This file contains the exception and peripheral interrupt handlers.
  *	
  * 
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"

/* External variables --------------------------------------------------------*/

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




