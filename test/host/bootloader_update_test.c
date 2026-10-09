#include "mocks.h"

#include <stdio.h>
#include <string.h>

static const uint8_t application[16] = {1U, 2U,  3U,  4U,  5U,  6U,  7U,  8U,
                                        9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U};
static unsigned int applicationOpens;
static unsigned int closes;
static unsigned int unmounts;
static bool shortWrite;
static bool failVerificationOpen;
static bool failProgrammingRead;

FRESULT f_open(FIL* file, const char* name, unsigned char mode)
{
    (void)mode;
    *file         = (FIL){0};
    file->crcFile = strcmp(name, CRC_FILE_NAME) == 0;
    if (!file->crcFile)
    {
        file->openNumber = ++applicationOpens;
        if (failVerificationOpen && file->openNumber == 4U)
        {
            return FR_DISK_ERR;
        }
    }
    return FR_OK;
}

FRESULT f_read(FIL* file, void* data, UINT length, UINT* bytesRead)
{
    static const char crc[] = "12345678";
    const uint8_t* source   = file->crcFile ? (const uint8_t*)crc : application;
    const UINT size         = file->crcFile ? 8U : sizeof(application);
    if (failProgrammingRead && file->openNumber == 3U)
    {
        return FR_DISK_ERR;
    }
    *bytesRead = size - file->fptr;
    if (*bytesRead > length)
    {
        *bytesRead = length;
    }
    memcpy(data, source + file->fptr, *bytesRead);
    file->fptr += *bytesRead;
    return FR_OK;
}

FRESULT f_close(FIL* file)
{
    (void)file;
    ++closes;
    return FR_OK;
}

uint32_t f_size(const FIL* file)
{
    (void)file;
    return sizeof(application);
}

bool SdCardMount(void)
{
    return true;
}
void SdCardUnMount(void)
{
    ++unmounts;
}
void CrcInit(void)
{
}
void CrcDeInit(void)
{
}
uint32_t CrcAccumulate(const uint8_t* data, uint32_t size)
{
    (void)data;
    (void)size;
    return 0x12345678U;
}
void FlashClearErrorFlags(void)
{
}
uint32_t FlashWrite(const uint8_t* data, uint32_t address, uint32_t length)
{
    (void)data;
    (void)address;
    return shortWrite ? length - 1U : length;
}
uint32_t FlashRead(uint8_t* data, uint32_t address, uint32_t length)
{
    // Flash may already contain matching data even when programming reports
    // an error. The bootloader must still propagate the write failure.
    memcpy(data, application + (address - APP_ADDRESS), length);
    return length;
}
void FlashRemapMemoryToSystemFlash(void)
{
}
void SetVectorTableLocation(uint32_t address)
{
    (void)address;
}
void __set_MSP(uint32_t address)
{
    (void)address;
}
void LedGreen1Toggle(void)
{
}
void LedGreen1Off(void)
{
}
void LedGreen2On(void)
{
}
void LedGreen2Off(void)
{
}
void LogPrint(const char* format, ...)
{
    (void)format;
}

static int RunCase(const char* name,
                   bool expected,
                   unsigned int expectedOpens,
                   unsigned int expectedCloses)
{
    applicationOpens  = 0U;
    closes            = 0U;
    unmounts          = 0U;
    const bool result = BootloaderUpdateFirmware();
    if (result != expected || applicationOpens != expectedOpens ||
        closes != expectedCloses || unmounts != 1U)
    {
        fprintf(stderr, "%s failed: result=%d opens=%u closes=%u unmounts=%u\n",
                name, result, applicationOpens, closes, unmounts);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failures = RunCase("successful update", true, 4U, 5U);
    shortWrite   = true;
    failures += RunCase("short flash write", false, 3U, 4U);
    shortWrite           = false;
    failVerificationOpen = true;
    failures += RunCase("verification file open error", false, 4U, 4U);
    failVerificationOpen = false;
    failProgrammingRead  = true;
    failures += RunCase("programming file read error", false, 3U, 4U);
    return failures;
}
