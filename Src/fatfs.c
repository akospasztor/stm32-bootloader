/**
  ******************************************************************************
  * STM32L4 Bootloader
  ******************************************************************************
  * @author Akos Pasztor
  * @file   fatfs.c
  * @brief  FatFS application
  *	        Code for FatFS application
  *         
  *
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

#include "fatfs.h"
#include "main.h"

char SD_Path[4];        /* SD logical drive path */   


uint8_t FATFS_Init(void) 
{
    return FATFS_LinkDriver(&SD_Driver, SD_Path);
}

uint8_t FATFS_DeInit(void)
{
    return FATFS_UnLinkDriver(SD_Path);
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{    
    return (DWORD)0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
