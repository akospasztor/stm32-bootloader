/**
 *******************************************************************************
 * STM32 Bootloader
 *******************************************************************************
 * @file
 * @author  Akos Pasztor
 * @brief   This file contains the main bootloader application entry point,
 *          including system initialization, boot sequence management, and
 *          firmware validation and launch logic logic.
 *******************************************************************************
 * @copyright (c) 2026 Akos Pasztor.                    https://akospasztor.com
 *            This software is licensed under terms that can be found in the
 *            LICENSE file in the root directory of this software component.
 *******************************************************************************
 */

/**
 * @addtogroup bl
 * @{
 * @addtogroup bl_main
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "board.h"
#include "bootloader.h"
#include "button.h"
#include "crc.h"
#include "delay.h"
#include "error_handler.h"
#include "flash.h"
#include "led.h"
#include "log.h"
#include "rcc.h"
#include "sd_diskio.h"
#include "sdmmc.h"

#include <stdbool.h>

/* Private enumerations ------------------------------------------------------*/
/** Enum for bootloader command determined by the user actions. */
typedef enum
{
    BL_CMD_LAUNCH_APPLICATION = 0U, /**< Launch the user application. */
    BL_CMD_ENTER_SYSMEM_BOOT, /**< Enter ST built-in system memory boot. */
    BL_CMD_UPDATE_FIRMWARE,   /**< Perform firmware update from SD card. */
} BootloaderCommand;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Wait for a user command by monitoring the button press duration.
 *
 * The button press duration determines the bootloader action:
 * - Pressed >= 1 second:  firmware update.
 * - Pressed >= 4 seconds: enter ST system memory boot mode.
 * - Pressed >= 9 seconds: do nothing, launch application.
 *
 * @return The ::BootloaderCommand corresponding to the button press duration.
 */
static BootloaderCommand WaitForUserCommand(void);

/**
 * @brief  Wait for a single button press within a timeout period.
 *
 * Toggles LEDs while waiting for up to 5 seconds. Returns as soon as the
 * button is pressed or the timeout expires.
 *
 * @return True if the button was pressed within the timeout; otherwise false.
 */
static bool WaitForSingleButtonPress(void);

/* Functions -----------------------------------------------------------------*/
/**
 * @brief  The main function.
 *
 * @return This function shall never return.
 */
int main(void)
{
    BoardInit();

    LogPrint("\n\n--------------------------------------------\n");
    LogPrint("Power up, Boot started\n");
    LogPrint("Bootloader version: %u.%u.%u\n", BOOTLOADER_VERSION_MAJOR,
             BOOTLOADER_VERSION_MINOR, BOOTLOADER_VERSION_PATCH);
    LogPrint("Application area: 0x%x-0x%x\n", APP_ADDRESS,
             (APP_ADDRESS + APP_SIZE));
    LogPrint("--------------------------------------------\n");

    /* Check system reset flags */
    if (RccResetFlagOblIsSet())
    {
        LogPrint("Note: OBL flag is active\n");
#if (CLEAR_RESET_FLAGS)
        /* Clear system reset flags */
        RccResetFlagsClear();
        LogPrint("Reset flags cleared\n");
#endif
    }

    /* Perform required actions based on button press duration */
    switch (WaitForUserCommand())
    {
        case BL_CMD_LAUNCH_APPLICATION:
            // Do nothing, launch the application
            break;

        case BL_CMD_ENTER_SYSMEM_BOOT:
            LogPrint("Entering the built-in system memory boot mode...\n");
            DelayMs(1000);
            BoardDeInit();
            BootloaderJumpToSysMem();
            break;

        case BL_CMD_UPDATE_FIRMWARE:
            LogPrint("Starting the firmware update process...\n");
            // Check for flash write protection
            if (FlashCheckIfWriteProtectionEnabled(APP_ADDRESS, APP_SIZE))
            {
                LogPrint("Application space in flash is write protected\n");
                LogPrint("Press button to disable flash write protection...\n");

                if (WaitForSingleButtonPress())
                {
                    LogPrint("Disabling write protection and generating system "
                             "reset...\n");
                    LogPrint("Note: The button must be pressed again after "
                             "the reset to perform the update\n");
                    // Disable flash write protection of the application area
                    if (!FlashDisableWriteProtection())
                    {
                        LogPrint(
                            "Error: Write protection could not be disabled\n");
                        break;
                    }
                }
                else
                {
                    LogPrint("Warning: Button was not pressed, write "
                             "protection is still active\n");
                    LogPrint("Note: Firmware update is skipped\n");
                    break;
                }
            }

            // Update the firmware
            if (!BootloaderUpdateFirmware())
            {
                LogPrint("Error: Firmware update failed\n");
                break;
            }

#if (USE_WRITE_PROTECTION)
            // Enable write protection of application area
            LogPrint("Enablig flash write protection and generating system "
                     "reset...\n");
            if (FlashEnableWriteProtection(APP_ADDRESS, APP_SIZE))
            {
                LogPrint("Error: Failed to enable write protection\n");
            }
#endif
            break;

        default:
            LogPrint("Error: Invalid user command\n");
            ErrorHandler();
            break;
    }

    /* Check if there is application in user flash area */
    if (BootloaderCheckForApplication())
    {
        LogPrint("Launching application...\n");
        LedGreen1On();
        DelayMs(200);
        LedGreen1Off();
        LedGreen2On();
        DelayMs(200);
        LedGreen2Off();
        DelayMs(1000);

        /* De-initialize bootloader hardware & peripherals */
        BoardDeInit();

        /* Launch application */
        BootloaderJumpToApplication();
    }

    /* No application found */
    LogPrint("Error: No application in flash\n");
    while (1)
    {
        ErrorHandler();
    }
}

static BootloaderCommand WaitForUserCommand(void)
{
    /* Check for user action:
            - button is pressed >= 1 second:  Enter Bootloader. LD2 is blinking.
            - button is pressed >= 4 seconds: Enter ST System Memory. LD3 is
              blinking.
            - button is pressed >= 9 seconds: Do nothing, launch application.
        */

    BootloaderCommand result = BL_CMD_LAUNCH_APPLICATION;
    uint8_t buttonCounter    = 0U;

    while ((ButtonIsPressed()) && (buttonCounter < 90U))
    {
        if (buttonCounter == 10U)
        {
            LogPrint("Release button now to update the firmware...\n");
        }
        if (buttonCounter == 40U)
        {
            LogPrint(
                "Release button now to enter the built-in system memory boot "
                "mode...\n");
        }

        if (buttonCounter < 10U)
        {
            LedGreen1On();
            LedGreen2On();
        }
        else if (buttonCounter == 10U)
        {
            LedGreen1Off();
            LedGreen2Off();
        }
        else if (buttonCounter < 40U)
        {
            LedGreen1Toggle();
        }
        else if (buttonCounter == 40U)
        {
            LedGreen1Off();
            LedGreen2On();
        }
        else
        {
            LedGreen2Toggle();
        }

        buttonCounter++;
        DelayMs(100U);
    }

    LedGreen1Off();
    LedGreen2Off();

    if (buttonCounter < 90U)
    {
        if (buttonCounter > 40U)
        {
            result = BL_CMD_ENTER_SYSMEM_BOOT;
        }
        else if (buttonCounter > 10U)
        {
            result = BL_CMD_UPDATE_FIRMWARE;
        }
        else
        {
            // Button was pressed too short, do nothing
        }
    }
    else
    {
        LogPrint("Note: Button is pressed too long\n");
    }

    return result;
}

static bool WaitForSingleButtonPress(void)
{
    bool isPressed = false;

    LedGreen1On();
    LedGreen2On();
    for (uint_fast8_t i = 0U; i < 100U; ++i)
    {
        LedGreen1Toggle();
        LedGreen2Toggle();
        DelayMs(50);
        if (ButtonIsPressed())
        {
            isPressed = true;
            break;
        }
    }
    LedGreen1Off();
    LedGreen2Off();

    return isPressed;
}

/**
 * @}
 * @}
 */
