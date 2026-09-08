/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the MCU internal flash driver implementation
 *          for read, write, erase, and write protection operations.
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
 * @addtogroup bsp_stm32l496_discovery_flash
 * @{
 */

#include "flash.h"

#include "mcu_hal.h"
#include "rcc.h"

#include <string.h>

/* Private defines -----------------------------------------------------------*/
// Message:     A conversion should not be performed between a pointer to object
//              and an integer type [misra-c2012-11.4]
// Reason:      The wrapped register defines from the vendor are addressing the
//              memory mapped base register of the FLASH hardware peripheral.
//              In order not to supress the message every time the defines are
//              used, they are wrapped with defines for which the message is
//              suppressed; resulting in a single suppression.
// Risk:        Conversion of a pointer to object into an integer may
//              produce a value that cannot be represented in the chosen
//              integer type resulting in undefined behavior.
// Prevention:  Code reviews.

/** Wrapper define for the flash size. */
// cppcheck-suppress [misra-c2012-11.4]
#define FLASH_SIZE_WRAPPER (uint32_t)(FLASH_SIZE)

/** Wrapper define for the flash bank size. */
// cppcheck-suppress [misra-c2012-11.4]
#define FLASH_BANK_SIZE_WRAPPER (uint32_t)(FLASH_BANK_SIZE)

/** Number of flash pages per bank. */
// cppcheck-suppress [misra-c2012-11.4]
#define FLASH_PAGE_NB_PER_BANK \
    (uint32_t)(FLASH_BANK_SIZE_WRAPPER / FLASH_PAGE_SIZE)

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Private helper function to perform the actual erase of the required
 *         flash pages.
 *
 * @warning This function does NOT check whether the provided data is within the
 *          range of the physical flash area!
 *
 * @param  address  Physical flash address where the data is erased.
 * @param  length   The length of the data in bytes.
 *
 * @return True if all the required pages are successfully erased; otherwise
 *         false.
 */
static bool FlashPerformErase(const uint32_t address, const uint32_t length);

/**
 * @brief  Private helper function to perform the actual writing of the provided
 *         data into the flash.
 *
 * @warning This function does NOT check whether the provided data is within the
 *          range of the physical flash area!
 *
 * @note   This function does not automatically erase the required flash pages.
 *
 * @param  data     Buffer containing the data to be written.
 * @param  address  Physical flash address where the data is written.
 * @param  length   The length of the data to be written in bytes.
 *
 * @return The number of bytes written to the flash.
 */
static uint32_t FlashPerformWrite(const uint8_t* const data,
                                  const uint32_t address,
                                  const uint32_t length);

/**
 * @brief  Get the bank where the provided address is located.
 *
 * @param  address  Physical address to be checked.
 *
 * @return FLASH_BANK_1 if the address is located in bank 1.
 *         FLASH_BANK_2 if the address is located in bank 2.
 */
static uint32_t FlashGetBank(const uint32_t address);

/**
 * @brief  Get the page number where the provided address is located.
 *
 * @note  The returned page number is relative to the actual bank where the page
 *        is located. For instance, calling the function with the starting
 *        address of the second bank will return the page number 0.
 *
 * @param  address  Physical address to be checked.
 *
 * @return The page number ranging from 0 to FLASH_PAGE_NB_PER_BANK where the
 *         provided address is located.
 */
static uint32_t FlashGetPage(const uint32_t address);

/**
 * @brief  Check if two flash page ranges overlap.
 *
 * @param  range1Start  Start page of the first range.
 * @param  range1End    End page of the first range.
 * @param  range2Start  Start page of the second range.
 * @param  range2End    End page of the second range.
 *
 * @return True if the two page ranges overlap; otherwise false.
 */
static bool FlashCheckIfPageRangesOverlap(const uint32_t range1Start,
                                          const uint32_t range1End,
                                          const uint32_t range2Start,
                                          const uint32_t range2End);

/* Functions -----------------------------------------------------------------*/
void FlashClearErrorFlags(void)
{
    (void)HAL_FLASH_Unlock();
    // cppcheck-suppress [misra-c2012-11.4]
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    (void)HAL_FLASH_Lock();
}

bool FlashCheckIfDataFits(const uint32_t address, const uint32_t length)
{
    return (address >= FLASH_BASE) &&
           ((address + length) <= (FLASH_BASE + FLASH_SIZE_WRAPPER));
}

bool FlashCheckIfWriteProtectionEnabled(const uint32_t address,
                                        const uint32_t length)
{
    bool result = false;

    FLASH_OBProgramInitTypeDef obProgram = {0};

    const uint32_t bankStart = FlashGetBank(address);
    const uint32_t bankEnd   = FlashGetBank(address + length - 1U);
    uint32_t pageStart       = FlashGetPage(address);
    uint32_t pageEnd         = FlashGetPage(address + length - 1U);

    // Check Bank 1
    if (bankStart == FLASH_BANK_1)
    {
        // If the address range to be checked spans over both banks, pin the
        // ending page number to the max value of the bank
        if (bankStart != bankEnd)
        {
            pageEnd = FLASH_PAGE_NB_PER_BANK - 1U;
        }

        // Check Bank Area A
        obProgram.WRPArea = OB_WRPAREA_BANK1_AREAA;
        HAL_FLASHEx_OBGetConfig(&obProgram);
        if (FlashCheckIfPageRangesOverlap(obProgram.WRPStartOffset,
                                          obProgram.WRPEndOffset, pageStart,
                                          pageEnd))
        {
            result = true;
        }

        // Check Bank Area B
        obProgram.WRPArea = OB_WRPAREA_BANK1_AREAB;
        HAL_FLASHEx_OBGetConfig(&obProgram);
        if (FlashCheckIfPageRangesOverlap(obProgram.WRPStartOffset,
                                          obProgram.WRPEndOffset, pageStart,
                                          pageEnd))
        {
            result = true;
        }
    }

    // Check Bank 2
    if (bankEnd == FLASH_BANK_2)
    {
        // If the address range to be checked spans over both banks, pin the
        // starting page number to 0
        if (bankStart != bankEnd)
        {
            pageStart = 0U;
        }

        // Check Bank Area A
        obProgram.WRPArea = OB_WRPAREA_BANK2_AREAA;
        HAL_FLASHEx_OBGetConfig(&obProgram);
        if (FlashCheckIfPageRangesOverlap(obProgram.WRPStartOffset,
                                          obProgram.WRPEndOffset, pageStart,
                                          pageEnd))
        {
            result = true;
        }

        // Check Bank Area B
        obProgram.WRPArea = OB_WRPAREA_BANK2_AREAB;
        HAL_FLASHEx_OBGetConfig(&obProgram);
        if (FlashCheckIfPageRangesOverlap(obProgram.WRPStartOffset,
                                          obProgram.WRPEndOffset, pageStart,
                                          pageEnd))
        {
            result = true;
        }
    }

    return result;
}

bool FlashDisableWriteProtection(void)
{
    FLASH_OBProgramInitTypeDef obProgram = {0};

    // Unlock the flash control & flash option bytes registers
    bool result = (HAL_OK == HAL_FLASH_Unlock());
    if (result)
    {
        result = (HAL_OK == HAL_FLASH_OB_Unlock());
    }

    /* Remove write protection in bank 1 area A */
    obProgram.WRPArea        = OB_WRPAREA_BANK1_AREAA;
    obProgram.OptionType     = OPTIONBYTE_WRP;
    obProgram.WRPStartOffset = 0xFF;
    obProgram.WRPEndOffset   = 0x00;
    if (result)
    {
        result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
    }

    /* Remove write protection in bank 1 area B */
    obProgram.WRPArea        = OB_WRPAREA_BANK1_AREAB;
    obProgram.OptionType     = OPTIONBYTE_WRP;
    obProgram.WRPStartOffset = 0xFF;
    obProgram.WRPEndOffset   = 0x00;
    if (result)
    {
        result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
    }

    /* Remove write protection in bank 2 area A */
    obProgram.WRPArea        = OB_WRPAREA_BANK2_AREAA;
    obProgram.OptionType     = OPTIONBYTE_WRP;
    obProgram.WRPStartOffset = 0xFF;
    obProgram.WRPEndOffset   = 0x00;
    if (result)
    {
        result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
    }

    /* Remove write protection in bank 2 area B */
    obProgram.WRPArea        = OB_WRPAREA_BANK2_AREAB;
    obProgram.OptionType     = OPTIONBYTE_WRP;
    obProgram.WRPStartOffset = 0xFF;
    obProgram.WRPEndOffset   = 0x00;
    if (result)
    {
        result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
    }

    // Launch the loading of flash option bytes
    // Note: this generates a system reset!
    if (result)
    {
        result = (HAL_OK == HAL_FLASH_OB_Launch());
    }

    // Lock the flash control & flash option bytes registers
    // Note: the functions always return HAL_OK; therefore their return values
    // are ignored
    (void)HAL_FLASH_OB_Lock();
    (void)HAL_FLASH_Lock();

    return result;
}

bool FlashEnableWriteProtection(const uint32_t address, const uint32_t length)
{
    FLASH_OBProgramInitTypeDef obProgram = {0};

    const uint32_t bankStart = FlashGetBank(address);
    const uint32_t bankEnd   = FlashGetBank(address + length - 1U);
    uint32_t pageStart       = FlashGetPage(address);
    uint32_t pageEnd         = FlashGetPage(address + length - 1U);

    // Unlock the flash control & flash option bytes registers
    bool result = (HAL_OK == HAL_FLASH_Unlock());
    if (result)
    {
        result = (HAL_OK == HAL_FLASH_OB_Unlock());
    }

    // Check if address range is part of bank 1
    if (bankStart == FLASH_BANK_1)
    {
        // If the address range to be checked spans over both banks, pin the
        // ending page number to the max value of the bank
        if (bankStart != bankEnd)
        {
            pageEnd = FLASH_PAGE_NB_PER_BANK - 1U;
        }

        // Enable write protection in bank 1 using area A
        // Note: area B is not used
        obProgram.WRPArea        = OB_WRPAREA_BANK1_AREAA;
        obProgram.OptionType     = OPTIONBYTE_WRP;
        obProgram.WRPStartOffset = pageStart;
        obProgram.WRPEndOffset   = pageEnd;
        if (result)
        {
            result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
        }
    }

    // Check if address range is part of bank 2
    if (bankEnd == FLASH_BANK_2)
    {
        // If the address range to be checked spans over both banks, pin the
        // starting page number to 0
        if (bankStart != bankEnd)
        {
            pageStart = 0U;
        }

        // Enable write protection in bank 1 using area A
        // Note: area B is not used
        obProgram.WRPArea        = OB_WRPAREA_BANK2_AREAA;
        obProgram.OptionType     = OPTIONBYTE_WRP;
        obProgram.WRPStartOffset = pageStart;
        obProgram.WRPEndOffset   = pageEnd;
        if (result)
        {
            result = (HAL_OK == HAL_FLASHEx_OBProgram(&obProgram));
        }
    }

    // Launch the loading of flash option bytes
    // Note: this generates a system reset!
    if (result)
    {
        result = (HAL_OK == HAL_FLASH_OB_Launch());
    }

    // Lock the flash control & flash option bytes registers
    // Note: the functions always return HAL_OK; therefore their return values
    // are ignored
    (void)HAL_FLASH_OB_Lock();
    (void)HAL_FLASH_Lock();

    return result;
}

bool FlashCompare(const uint8_t* const data,
                  const uint32_t address,
                  const uint32_t length)
{
    bool result = false;
    if ((length > 0U) && FlashCheckIfDataFits(address, length))
    {
        // Message:     A conversion should not be performed between a pointer
        //              to object and an integer type [misra-c2012-11.4]
        // Reason:      The memcmp function expects its arguments as pointers,
        //              meanwhile the address is contained in an integer.
        // Risk:        Conversion of an integer into a pointer to void may
        //              result in a pointer that is not correctly aligned,
        //              resulting in undefined behavior.
        // Prevention:  Code reviews.
        // cppcheck-suppress [misra-c2012-11.4]
        result = (memcmp(data, (const uint8_t*)address, length) == 0);
    }
    return result;
}

uint32_t FlashRead(uint8_t* const data,
                   const uint32_t address,
                   const uint32_t length)
{
    uint32_t readBytes = 0U;
    if ((length > 0U) && FlashCheckIfDataFits(address, length))
    {
        // Message:     A cast shall not be performed between pointer to void
        //              and an arithmetic type [misra-c2012-11.6]
        // Reason:      The memcpy function expects its argument(s) as pointer
        //              to void, meanwhile the address is contained in an
        //              integer.
        // Risk:        Conversion of an integer into a pointer to void may
        //              result in a pointer that is not correctly aligned,
        //              resulting in undefined behavior.
        // Prevention:  Code reviews.
        // cppcheck-suppress [misra-c2012-11.6]
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        (void)memcpy((void*)data, (void*)address, length);
        readBytes = length;
    }
    return readBytes;
}

uint32_t FlashWrite(const uint8_t* const data,
                    const uint32_t address,
                    const uint32_t length)
{
    size_t writtenBytes = 0U;

    if ((length > 0U) && FlashCheckIfDataFits(address, length))
    {
        // Write to flash only if the flash contents do not match with the
        // contents of the buffer to be written
        if (!FlashCompare(data, address, length))
        {
            // Unlock flash
            (void)HAL_FLASH_Unlock();

            // Erase page(s) and perform writing data into flash
            if (FlashPerformErase(address, length))
            {
                writtenBytes = FlashPerformWrite(data, address, length);
            }

            // Lock flash
            (void)HAL_FLASH_Lock();
        }
        else
        {
            writtenBytes += length;
        }
    }

    return writtenBytes;
}

void FlashRemapMemoryToSystemFlash(void)
{
    RccEnableSysCfg();
    // cppcheck-suppress [misra-c2012-11.4]
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
}

static bool FlashPerformErase(const uint32_t address, const uint32_t length)
{
    bool result = false;

    HAL_StatusTypeDef status         = HAL_OK;
    FLASH_EraseInitTypeDef eraseInit = {0U};
    uint32_t pageError               = 0U;

    const uint32_t bankStart = FlashGetBank(address);
    const uint32_t bankEnd   = FlashGetBank(address + length - 1U);
    const uint32_t pageStart = FlashGetPage(address);
    const uint32_t pageEnd   = FlashGetPage(address + length - 1U);

    /* Check if the data spans over both banks */
    if (bankStart != bankEnd)
    {
        /* Erase first bank */
        eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseInit.Banks     = FLASH_BANK_1;
        eraseInit.Page      = pageStart;
        eraseInit.NbPages   = FLASH_PAGE_NB_PER_BANK - pageStart;

        status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
        if ((status == HAL_OK) && (pageError == 0xFFFFFFFFU))
        {
            /* Erase second bank */
            eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
            eraseInit.Banks     = FLASH_BANK_2;
            eraseInit.Page      = 0U;
            eraseInit.NbPages   = pageEnd + 1U;

            status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
            if ((status == HAL_OK) && (pageError == 0xFFFFFFFFU))
            {
                result = true;
            }
        }
    }
    else
    {
        /* Erase pages in a single bank */
        eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseInit.Banks     = bankStart;
        eraseInit.Page      = pageStart;
        eraseInit.NbPages   = pageEnd - pageStart + 1U;

        status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
        if ((status == HAL_OK) && (pageError == 0xFFFFFFFFU))
        {
            result = true;
        }
    }

    return result;
}

static uint32_t FlashPerformWrite(const uint8_t* const data,
                                  const uint32_t address,
                                  const uint32_t length)
{
    uint32_t writtenBytes = 0U;

    for (uint32_t i = 0U; i < length; (i = i + 8U))
    {
        HAL_StatusTypeDef status = HAL_OK;
        uint64_t dataChunkValue  = 0U;
        // The length of the data buffer might not be the multiplication of 8
        // (bytes). In this case, buffer overflow must be avoided during the
        // writing of the last 8-byte chunk.
        const uint8_t dataChunkSize =
            ((length - i) < 8U) ? (uint8_t)(length - i) : 8U;
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        (void)memcpy(&dataChunkValue, (const void*)&data[i], dataChunkSize);

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (address + i),
                                   dataChunkValue);

        if (status == HAL_OK)
        {
            writtenBytes += dataChunkSize;
        }
        else
        {
            break;
        }
    }

    return writtenBytes;
}

static uint32_t FlashGetBank(const uint32_t address)
{
    return (address < (FLASH_BASE + FLASH_BANK_SIZE_WRAPPER)) ? FLASH_BANK_1
                                                              : FLASH_BANK_2;
}

static uint32_t FlashGetPage(const uint32_t address)
{
    return (address < (FLASH_BASE + FLASH_BANK_SIZE_WRAPPER))
               ? ((address - FLASH_BASE) / FLASH_PAGE_SIZE)
               : ((address - (FLASH_BASE + FLASH_BANK_SIZE_WRAPPER)) /
                  FLASH_PAGE_SIZE);
}

static bool FlashCheckIfPageRangesOverlap(const uint32_t range1Start,
                                          const uint32_t range1End,
                                          const uint32_t range2Start,
                                          const uint32_t range2End)
{
    bool isOverlap = false;
    if ((range1End > range1Start) && (range2End > range2Start))
    {
        isOverlap = ((range1Start <= range2End) && (range2Start <= range1End));
    }
    return isOverlap;
}

/**
 * @}
 * @}
 * @}
 */
