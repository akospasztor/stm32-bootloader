#ifndef BOOTLOADER_HOST_MOCKS_H
#define BOOTLOADER_HOST_MOCKS_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FLASH_PAGE_SIZE         16U
#define APP_ADDRESS             0x08010000U
#define APP_SIZE                256U
#define SRAM_BASE               0x20000000U
#define SRAM1_SIZE_MAX          65536U
#define SRAM2_SIZE              16384U
#define SYSTEM_MEMORY_ADDRESS   0x1FFF0000U
#define APP_FILE_NAME           "app.bin"
#define CRC_FILE_NAME           "app.crc"
#define __IO                    volatile
#define assert_param(condition) assert(condition)
#define FA_READ                 1U

// Minimal FatFs and board interfaces used by the production bootloader.
typedef unsigned int UINT;
typedef enum
{
    FR_OK,
    FR_DISK_ERR
} FRESULT;
typedef struct
{
    uint32_t fptr;
    bool crcFile;
    unsigned int openNumber;
} FIL;

FRESULT f_open(FIL* file, const char* name, unsigned char mode);
FRESULT f_read(FIL* file, void* data, UINT length, UINT* bytesRead);
FRESULT f_close(FIL* file);
uint32_t f_size(const FIL* file);
bool SdCardMount(void);
void SdCardUnMount(void);
void CrcInit(void);
void CrcDeInit(void);
uint32_t CrcAccumulate(const uint8_t* data, uint32_t size);
void FlashClearErrorFlags(void);
uint32_t FlashWrite(const uint8_t* data, uint32_t address, uint32_t length);
uint32_t FlashRead(uint8_t* data, uint32_t address, uint32_t length);
void FlashRemapMemoryToSystemFlash(void);
void SetVectorTableLocation(uint32_t address);
void __set_MSP(uint32_t address);
void LedGreen1Toggle(void);
void LedGreen1Off(void);
void LedGreen2On(void);
void LedGreen2Off(void);
void LogPrint(const char* format, ...);
bool BootloaderUpdateFirmware(void);

#endif
