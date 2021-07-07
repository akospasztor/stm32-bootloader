/**
 *******************************************************************************
 * STM32L4 Bootloader
 *******************************************************************************
 * @author Akos Pasztor
 * @file   fatfs.c
 * @brief  FatFS application
 *         Code for FatFS application
 *
 *
 *******************************************************************************
 * Copyright (c) 2020 Akos Pasztor.                     https://akospasztor.com
 *******************************************************************************
 */

#include "fatfs.h"
#include "main.h"

/* Variables -----------------------------------------------------------------*/
char SDPath[4]; /* SD logical drive path */
FATFS SDFatFs;  /* File system object for SD logical drive */
FIL SDFile;     /* File object for SD */

/* Functions -----------------------------------------------------------------*/
uint8_t FATFS_Init(void)
{
    return FATFS_LinkDriver(&SD_Driver, SDPath);
}

uint8_t FATFS_DeInit(void)
{
    return FATFS_UnLinkDriver(SDPath);
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
