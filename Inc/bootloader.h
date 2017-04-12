#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

/*** Bootloader configuration *************************************************/
#define USE_CHECKSUM    1
#define APP_ADDRESS     (uint32_t)0x08008000    /* Start addr. of APP */
#define END_ADDRESS     (uint32_t)0x080FFFFB    /* End addr. of APP (addr. of last byte) */
#define CRC_ADDRESS     (uint32_t)0x080FFFFC    /* Start addr. of CRC in ROM */
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
    BL_WRITE_ERROR
};

/* Functions -----------------------------------------------------------------*/
uint8_t Bootloader_CheckSize(uint32_t appsize);
uint8_t Bootloader_Erase(void);
void    Bootloader_FlashInit(void);
uint8_t Bootloader_FlashNext(uint64_t data);
void    Bootloader_FlashEnd(void);
uint8_t Bootloader_VerifyChecksum(void);
uint8_t Bootloader_CheckForApplication(void);
void    Bootloader_JumpToApplication(void);

#endif /* __BOOTLOADER_H */