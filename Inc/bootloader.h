#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

/*** Bootloader configuration *************************************************/
#define USE_SWO_TRACE           1
#define USE_CHECKSUM            0
#define USE_WRITE_PROTECTION    0
#define SET_VECTOR_TABLE        1

#define APP_FILENAME    "image.bin"
#define APP_ADDRESS     (uint32_t)0x08008000    /* Start addr. of Application */
#define END_ADDRESS     (uint32_t)0x080FFFFB    /* End addr. of Application (addr. of last byte) */
#define CRC_ADDRESS     (uint32_t)0x080FFFFC    /* Start addr. of CRC in ROM */

#define SYSMEM_ADDRESS  (uint32_t)0x1FFF0000    /* Address of System Memory (ST Bootloader) */
/*** End of configuration *****************************************************/

/* Defines -------------------------------------------------------------------*/
#define FLASH_PAGE_NBPERBANK    256             /* Number of Pages per Bank in Flash */
#define APP_SIZE        (uint32_t)(((END_ADDRESS - APP_ADDRESS) + 3) / 4) /* Size of APP in DWORD (32bits/4bytes) */

/* Bootloader Error Codes */
enum
{
    BL_OK = 0,
    BL_NO_APP,
    BL_SIZE_ERROR,
    BL_CHKS_ERROR,
    BL_ERASE_ERROR,
    BL_WRITE_ERROR,
    BL_PROTECTION_ERROR
};

/* Flash Protection Types */
enum
{
    BL_FLASH_PROT_NONE  = 0,
    BL_FLASH_PROT_WRP   = 0x1,
    BL_FLASH_PROT_RDP   = 0x2,
    BL_FLASH_PROT_PCROP = 0x4,
};

/* Functions -----------------------------------------------------------------*/
uint8_t  Bootloader_CheckSize(uint32_t appsize);
uint8_t  Bootloader_Erase(void);
void     Bootloader_FlashInit(void);
uint8_t  Bootloader_FlashNext(uint64_t data);
void     Bootloader_FlashEnd(void);

uint32_t Bootloader_GetProtectionStatus(void); 
uint32_t Bootloader_ConfigWriteProtection(uint32_t protection);

uint8_t  Bootloader_VerifyChecksum(void);
uint8_t  Bootloader_CheckForApplication(void);
void     Bootloader_JumpToApplication(void);
void     Bootloader_JumpToSysMem(void);

#endif /* __BOOTLOADER_H */