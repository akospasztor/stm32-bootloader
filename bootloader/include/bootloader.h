/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the bootloader configuration parameters,
 *          function prototypes, and other required definitions.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup bl Bootloader
 * @{
 * @addtogroup bl_bootloader Bootloader
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/** Enable write protection after performing in-app-programming */
#define USE_WRITE_PROTECTION 0

/** Clear reset flags
 *  - If enabled: bootloader clears reset flags. (This occurs only when OBL RST
 * flag is active.)
 *  - If disabled: bootloader does not clear reset flags, not even when OBL RST
 * is active.
 */
#define CLEAR_RESET_FLAGS 1

/** File name of application image located on SD card */
#define APP_FILE_NAME "app-demo.bin"

/** File name that contains the application CRC located on SD card */
#define CRC_FILE_NAME "app-demo.crc"

/* External variables --------------------------------------------------------*/
/** Application area start address defined in the linker file */
extern const uint32_t LINKER_APP_AREA_ORIGIN;

/** Application area size address defined in the linker file */
extern const uint32_t LINKER_APP_AREA_LENGTH;

/* Defines -------------------------------------------------------------------*/
/** Helper define for the application area start address */
// Message:     A conversion should not be performed between a pointer to object
//              and an integer type [misra-c2012-11.4]
// Reason:      The address of the application area in the microcontroller flash
//              memory is provided by the linker in a form of a pointer. This
//              physical address value needs to be converted into an integer.
// Risk:        Conversion of a pointer to object into an integer may produce a
//              value that cannot be represented in the chosen integer type
//              resulting in undefined behavior.
// Prevention:  Code reviews.
// cppcheck-suppress-macro [misra-c2012-11.4]
#define APP_ADDRESS ((uint32_t)&LINKER_APP_AREA_ORIGIN)

/** Helper define for the application area size */
// Message:     A conversion should not be performed between a pointer to object
//              and an integer type [misra-c2012-11.4]
// Reason:      The size of the application area in the microcontroller flash
//              memory is provided by the linker in a form of a pointer. This
//              physical address value needs to be converted into an integer.
// Risk:        Conversion of a pointer to object into an integer may produce a
//              value that cannot be represented in the chosen integer type
//              resulting in undefined behavior.
// Prevention:  Code reviews.
// cppcheck-suppress-macro [misra-c2012-11.4]
#define APP_SIZE ((uint32_t)&LINKER_APP_AREA_LENGTH)

/* Functions -----------------------------------------------------------------*/
/**
 * @brief  This function performs the complete application update procedure.
 *
 * The function checks the application size, the application CRC; then carries
 * out the actual firmware update process and finally verifies if the update
 * was successful.
 *
 * @return True if the entire process was successful; otherwise false.
 */
bool BootloaderUpdateFirmware(void);

/**
 * @brief  This function checks whether a valid application exists in flash.
 *
 * The check is performed by checking the very first DWORD (4 bytes) of the
 * application firmware. In case of a valid application, this DWORD must
 * represent the initialization location of stack pointer - which must be within
 * the boundaries of RAM.
 *
 * @return True if there is a valid application found in the microcontroller
 *         flash application area.
 */
bool BootloaderCheckForApplication(void);

/**
 * @brief  This function performs the jump to the user application in flash.
 *
 * The function carries out the following operations:
 * - Sets the vector table location
 * - Sets the stack pointer location
 * - Performs the jump
 */
void BootloaderJumpToApplication(void);

/**
 * @brief  This function performs the jump to the microcontroller System Memory
 *         (ST built-in bootloader).
 *
 *  The function carries out the following operations:
 *  - Remaps the system flash memory
 *  - Performs the jump
 */
void BootloaderJumpToSysMem(void);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
