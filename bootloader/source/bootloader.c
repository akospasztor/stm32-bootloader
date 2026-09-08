/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the bootloader implementation including firmware
 *          verification, flash programming, checksum validation, and
 *          application launch functions.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bl
 * @{
 * @addtogroup bl_bootloader
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bootloader.h"

#include "board.h"
#include "crc.h"
#include "ff.h"
#include "flash.h"
#include "led.h"
#include "log.h"
#include "mcu_hal.h"
#include "sd_diskio.h"
#include "sd_fatfs.h"
#include "sdmmc.h"
#include "systick.h"

#include <errno.h>
#include <stdlib.h>

/* Private type definitions --------------------------------------------------*/
typedef void (*Function)(void); /**< Function pointer type definition */

/* Private variables ---------------------------------------------------------*/
/** Read buffer for reading data from a file on the SD card */
static uint8_t readBuffer[FLASH_PAGE_SIZE] = {0};

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Check whether the application binary on the SD card fits into the
 *         available flash memory region designated for the application.
 *
 * @return True if the application size is within the allowed limit; false if
 *         the application file cannot be opened or exceeds the maximum size.
 */
static bool CheckApplicationSize(void);

/**
 * @brief  Validate the integrity of the application binary by comparing a
 *         CRC-32 value calculated over the file contents against the reference
 *         CRC value read from a separate file on the SD card.
 *
 * @return True if the CRC values match; false on CRC mismatch or if the CRC
 *         could not be calculated/read.
 */
static bool CheckApplicationCrc(void);

/**
 * @brief  Erase the application flash region and program it with the contents
 *         of the application binary file from the SD card, page by page.
 *
 * @return True if flash programming completed successfully; false on file
 *         read error or flash write error.
 */
static bool UpdateApplication(void);

/**
 * @brief  Verify the programmed flash contents against the original application
 *         binary file on the SD card by performing a word-by-word comparison.
 *
 * @return True if the flash contents match the file; false on verification
 *         mismatch or file read error.
 */
static bool VerifyApplication(void);

/**
 * @brief  Calculate the CRC-32 checksum of the application binary file on the
 *         SD card using the hardware CRC peripheral.
 *
 * @param  crc  Pointer to store the calculated CRC-32 value.
 * @return True if the CRC calculation completed successfully; false if the
 *         file could not be opened or read.
 */
static bool CalculateCrcOfFile(uint32_t* const crc);

/**
 * @brief  Read the expected CRC-32 value from a dedicated CRC file on the SD
 *         card and convert the hexadecimal string representation to an
 *         integer.
 *
 * @param  crc  Pointer to store the parsed CRC-32 value.
 * @return True if the CRC value was read and converted successfully; false if
 *         the file could not be opened, read, or the value could not be
 *         converted.
 */
static bool ReadCrcFromFile(uint32_t* const crc);

/* Functions -----------------------------------------------------------------*/
bool BootloaderUpdateFirmware(void)
{
    bool result = SdCardMount();

    result = result && CheckApplicationSize();
    result = result && CheckApplicationCrc();
    result = result && UpdateApplication();
    result = result && VerifyApplication();

    SdCardUnMount();
    return result;
}

bool BootloaderCheckForApplication(void)
{
    bool result = true;

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The first 32-bit value of the application represents the
    //              initial stack pointer value. The jump address (the starting
    //              address of the application - which is the address of the
    //              reset exception handler) is the second 32-bit value. These
    //              values need to be accessed and converted into integers.
    // Risk:        Conversion of an integer into a pointer to void may result
    //              in a pointer that is not correctly aligned, resulting in
    //              undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    const uint32_t jumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4U);
    // cppcheck-suppress [misra-c2012-11.4]
    const uint32_t stackPointer = *(__IO uint32_t*)APP_ADDRESS;

    // Check if the application reset handler is within the application space
    if ((jumpAddress < APP_ADDRESS) || (jumpAddress == 0xFFFFFFFFU))
    {
        result = false;
    }

    // Check if the stack pointer is within the RAM region
    if ((stackPointer < SRAM_BASE) ||
        (stackPointer > (SRAM_BASE + SRAM1_SIZE_MAX + SRAM2_SIZE)))
    {
        result = false;
    }

    return result;
}

void BootloaderJumpToApplication(void)
{
    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The jump address (the starting address of the application;
    //              the address of the reset handler) is the second 32-bit value
    //              of the application. This value needs to be accessed and
    //              converted into an integer.
    // Risk:        Conversion of an integer into a pointer to void may
    //              result in a pointer that is not correctly aligned,
    //              resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    uint32_t jumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4U);

    // Message:     A cast shall not be performed between pointer to void and
    //              an arithmetic type [misra-c2012-11.6]
    // Reason:      The jump address needs to be casted to a function pointer
    //              in order to perform the actual jump.
    // Risk:        Conversion of an integer into a pointer to void may result
    //              in a pointer that is not correctly aligned, resulting in
    //              undefined behavior.
    // Prevention:  Code reviews and testing.
    // cppcheck-suppress [misra-c2012-11.6]
    Function jump = (Function)jumpAddress;

    SetVectorTableLocation(APP_ADDRESS);

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The first 32-bit value of the application represents the
    //              initial stack pointer value. This values need to be accessed
    //              and converted into an integer.
    // Risk:        Conversion of an integer into a pointer to void may result
    //              in a pointer that is not correctly aligned, resulting in
    //              undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    __set_MSP(*(__IO uint32_t*)APP_ADDRESS);
    jump();
}

void BootloaderJumpToSysMem(void)
{
    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The jump address (the starting address of the application;
    //              the address of the reset handler) is the second 32-bit value
    //              of the application. This value needs to be accessed and
    //              converted into an integer.
    // Risk:        Conversion of an integer into a pointer to void may
    //              result in a pointer that is not correctly aligned,
    //              resulting in undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    uint32_t jumpAddress = *(__IO uint32_t*)(SYSTEM_MEMORY_ADDRESS + 4U);

    // Message:     A cast shall not be performed between pointer to void and
    //              an arithmetic type [misra-c2012-11.6]
    // Reason:      The jump address needs to be casted to a function pointer
    //              in order to perform the actual jump.
    // Risk:        Conversion of an integer into a pointer to void may result
    //              in a pointer that is not correctly aligned, resulting in
    //              undefined behavior.
    // Prevention:  Code reviews and testing.
    // cppcheck-suppress [misra-c2012-11.6]
    Function jump = (Function)jumpAddress;

    FlashRemapMemoryToSystemFlash();

    // Message:     A conversion should not be performed between a pointer to
    //              object and an integer type [misra-c2012-11.4]
    // Reason:      The first 32-bit value of the application represents the
    //              initial stack pointer value. This values need to be accessed
    //              and converted into an integer.
    // Risk:        Conversion of an integer into a pointer to void may result
    //              in a pointer that is not correctly aligned, resulting in
    //              undefined behavior.
    // Prevention:  Code reviews.
    // cppcheck-suppress [misra-c2012-11.4]
    __set_MSP(*(__IO uint32_t*)SYSTEM_MEMORY_ADDRESS);
    jump();
}

static bool CheckApplicationSize(void)
{
    FIL file;
    bool result = true;

    if (FR_OK == f_open(&file, APP_FILE_NAME, FA_READ))
    {
        const uint32_t appSize = f_size(&file);
        LogPrint("Size of application on the SD card: %u bytes\n", appSize);
        if (appSize > APP_SIZE)
        {
            LogPrint("Error: Application on SD card is too large.\n");
            LogPrint("Max allowed size: %u bytes\n", APP_SIZE);
            result = false;
        }
        (void)f_close(&file);
    }
    else
    {
        LogPrint("Error: Application file cannot be opened.\n");
        result = false;
    }

    return result;
}

static bool CheckApplicationCrc(void)
{
    uint32_t calculatedCrc = 0U;
    uint32_t providedCrc   = 0U;
    bool result;

    result = CalculateCrcOfFile(&calculatedCrc);
    result = result && ReadCrcFromFile(&providedCrc);

    // Compare CRC values
    if (result)
    {
        if (providedCrc == calculatedCrc)
        {
            LogPrint("CRC matches. CRC value: 0x%x\n", calculatedCrc);
        }
        else
        {
            LogPrint("Error: CRC mismatch.\n");
            LogPrint("Provided application CRC: 0x%08x\n", providedCrc);
            LogPrint("Calculated application CRC: 0x%08x\n", calculatedCrc);
            result = false;
        }
    }

    return result;
}

bool UpdateApplication(void)
{
    FIL file;
    bool result = false;

    LogPrint("Starting programming...\n");
    LedGreen2On();

    if (FR_OK == f_open(&file, APP_FILE_NAME, FA_READ))
    {
        UINT bytesRead;
        FRESULT status;
        uint32_t bytesFlashed = 0;

        FlashClearErrorFlags();

        // Message:     There should be no more than one break or goto statement
        //              used to terminate any iteration statement
        //              [misra-c2012-15.4]
        // Reason:      Multiple and early break statements are used for
        //              avoiding a complex implementation of checking the
        //              results of the intermediate operations. It also allows
        //              better code readability.
        // Risk:        A single point of exit is required by IEC 61508 and ISO
        //              26262 as part of the requirements. Early returns may
        //              lead to the unintentional omission of function
        //              termination code. If a function has exit points
        //              interspersed with statements that produce persistent
        //              side effects, it is not easy to determine which side
        //              effects will occur when the function is executed.
        // Prevention:  Code reviews and testing.
        // cppcheck-suppress-begin [misra-c2012-15.4]
        do
        {
            status = f_read(&file, readBuffer, FLASH_PAGE_SIZE, &bytesRead);
            if (status != FR_OK)
            {
                LogPrint("Error: Application file read error at: %u byte\n",
                         (uint32_t)(file.fptr));
                break;
            }

            if (bytesRead > 0U)
            {
                if (FlashWrite(readBuffer, (APP_ADDRESS + bytesFlashed),
                               (uint32_t)bytesRead) != (uint32_t)bytesRead)
                {
                    LogPrint("Error: Programming error at: %u byte\n",
                             bytesFlashed);
                    break;
                }

                bytesFlashed += bytesRead;
            }

            // Toggle LED during programming
            LedGreen1Toggle();
        } while (bytesRead > 0U);
        // cppcheck-suppress-end [misra-c2012-15.4]

        (void)f_close(&file);

        if (status == FR_OK)
        {
            LogPrint("Programming successful. Flashed: %u bytes\n",
                     bytesFlashed);
            result = true;
        }
    }
    else
    {
        LogPrint("Error: Application file cannot be opened.\n");
    }

    LedGreen1Off();
    LedGreen2Off();
    return result;
}

static bool VerifyApplication(void)
{
    FIL file;
    bool result = true;

    LogPrint("Starting verification...\n");

    /* Open file for verification */
    if (FR_OK == f_open(&file, APP_FILE_NAME, FA_READ))
    {
        UINT bytesRead;
        uint32_t addressToVerify = APP_ADDRESS;

        // Message:     There should be no more than one break or goto statement
        //              used to terminate any iteration statement
        //              [misra-c2012-15.4]
        // Reason:      Multiple and early break statements are used for
        //              avoiding a complex implementation of checking the
        //              results of the intermediate operations. It also allows
        //              better code readability.
        // Risk:        A single point of exit is required by IEC 61508 and ISO
        //              26262 as part of the requirements. Early returns may
        //              lead to the unintentional omission of function
        //              termination code. If a function has exit points
        //              interspersed with statements that produce persistent
        //              side effects, it is not easy to determine which side
        //              effects will occur when the function is executed.
        // Prevention:  Code reviews and testing.
        // cppcheck-suppress-begin [misra-c2012-15.4]
        do
        {
            uint32_t dataChunkFromFile = 0U;
            if (FR_OK != f_read(&file, &dataChunkFromFile,
                                sizeof(dataChunkFromFile), &bytesRead))
            {
                LogPrint("Error: Application file read error at: %u byte\n",
                         (uint32_t)(file.fptr));
                result = false;
                break;
            }

            if (bytesRead > 0U)
            {
                uint32_t dataChunkFromFlash = 0U;
                (void)FlashRead((uint8_t*)&dataChunkFromFlash, addressToVerify,
                                sizeof(dataChunkFromFlash));
                if (dataChunkFromFlash != dataChunkFromFile)
                {
                    LogPrint("Error: Verification error at: 0x%x\n",
                             addressToVerify);
                    result = false;
                    break;
                }

                addressToVerify += bytesRead;
            }
        } while (bytesRead > 0U);
        // cppcheck-suppress-end [misra-c2012-15.4]

        (void)f_close(&file);

        if (result)
        {
            LogPrint("Verification successful. Verified: %u bytes\n",
                     (addressToVerify - APP_ADDRESS));
        }
    }
    else
    {
        LogPrint("Error: Application file cannot be opened.\n");
    }

    return result;
}

static bool CalculateCrcOfFile(uint32_t* const crc)
{
    assert_param(crc != NULL);

    FIL file;
    bool result            = true;
    uint32_t calculatedCrc = 0U;

    CrcInit();

    // Open application file and calculate CRC
    if (FR_OK == f_open(&file, APP_FILE_NAME, FA_READ))
    {
        UINT bytesRead = 0U;
        do
        {
            if (FR_OK != f_read(&file, readBuffer, FLASH_PAGE_SIZE, &bytesRead))
            {
                LogPrint("Error: Application file read error at: %u byte\n",
                         (uint32_t)(file.fptr));
                result = false;
                break;
            }
            if (bytesRead > 0U)
            {
                calculatedCrc = CrcAccumulate(readBuffer, (uint32_t)bytesRead);
            }
        } while (bytesRead > 0U);
        (void)f_close(&file);
    }
    else
    {
        LogPrint("Error: Application file cannot be opened.\n");
        result = false;
    }

    CrcDeInit();

    *crc = calculatedCrc;
    return result;
}

static bool ReadCrcFromFile(uint32_t* const crc)
{
    assert_param(crc != NULL);

    FIL file;
    bool result = false;

    if (FR_OK == f_open(&file, CRC_FILE_NAME, FA_READ))
    {
        UINT bytesRead = 0U;
        if (FR_OK == f_read(&file, readBuffer, 8U, &bytesRead))
        {
            readBuffer[bytesRead] = 0x00U;
            errno                 = 0;
            *crc                  = strtoul((const char*)readBuffer, NULL, 16);
            if (errno == 0)
            {
                result = true;
            }
            else
            {
                LogPrint(
                    "Error: CRC cannot be converted to numerical value.\n");
            }
        }
        else
        {
            LogPrint("Error: CRC file cannot be read.\n");
        }
        (void)f_close(&file);
    }
    else
    {
        LogPrint("Error: CRC file cannot be opened.\n");
    }

    return result;
}

/**
 * @}
 * @}
 */
