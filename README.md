# STM32 Bootloader
Customizable Bootloader for STM32 microcontrollers. This example demonstrates how to perform in-application-programming of a firmware located on external SD card with FAT32 file system.

## Table of Contents
- [Bootloader features](#bootloader-features)
- [Description](#description)
- [Source code organization](#source-code-organization)
- [How to use](#how-to-use)
- [Configuration](#configuration)
- [References](#references)

## Bootloader features
- Configurable application space
- Checksum verification
- Flash protection check, write protection enable/disable
- Extended error handling, fail-safe design
- Bootloader firmware update and the ability to perform full chip re-programming: enter ST's built-in bootloader from software (without triggering the BOOT pin)
- SWO tracing for easier debugging and development
- Easy to customize and port to other microcontrollers

## Description
This demo is implemented on a custom hardware [see FIGURE] equipped with a STM32L476VG microcontroller [[1, 2]](#references). The microSD card is connected to the MCU over SDIO interface. The example software uses the official HAL library of ST [[3]](#references) and is compiled with IAR EWARM. Programming and debugging is performed over SWD with a SEGGER J-Link debug probe.

![System overview](system-overview.png)

*Figure 1: System overview*

The microcontroller flash is organized as follows: by default the first 32kBytes (16 pages) of the flash is reserved for the bootloader and the rest of the flash is the application space. 

![Flash organization](flash-organization.png)

*Figure 2: Flash organization*

After power-up, the bootloader starts. All three LEDs are flashed for a second, then the bootloader checks for user-interaction:

- If the button is not pressed, then the bootloader tries to launch the application: First it checks the application space. If there is an application, the bootloader calculates the checksum over the application space and compares with the application checksum (if the checksum feature is enabled). Finally, the bootloader prepares for the jump by resetting the peripherals, disabling the SysTick, setting the vector table and stack pointer, then the bootloader performs a jump to the application.

- If the button is pressed and released within 4 seconds: the bootloader tries to update the application by performing the following sequence:

    1. Checks for write-protection. If the application space is write-protected, then the red LED is switched on and the yellow LED is flashed for five seconds. If the button is pressed within this interval, the bootloader disables the write protection and performs a system reset (required after flash option bytes programming). Please note that after disabling the write protection, the user has to invoke the application update procedure again by pressing the button in order to continue the update.
    2. Initializes SD card, looks for application binary and opens the file.
    3. Checks the file size whether it fits the application space in the microcontroller flash.
    4. Initializes microcontroller flash.
    5. Erases the application space.
    6. Performs flash programming.
    7. Enables write protection of application space if this feature is enabled in the configuration.
    8. After successful in-application-programming, the bootloader launches the application.

- If the button is pressed for more than 4 seconds: the bootloader launches ST's built-in bootloader located in the internal boot ROM (system memory) of the chip. For more information, please refer to [[4]](#references). With this method, the bootloader can be updated or even a full chip re-programming can be performed easily, for instance by connecting the hardware to the computer via USB and using the DFU mode [[5, 6]](#references).

[FIGURE: sequence]

## Source code organization
```
stm32-dma-uart/
  |—— Drivers/
  |—— EWARM/
  |—— Inc/
  |—— Middlewares/
  `—— Src/
```
`Drivers` and `Middlewares` folders contain the CMSIS, HAL and FatFs libraries for the microcontroller. The source code and corresponding header files can be found in `Src` and `Inc` folders respectively.

## How to use
The bootloader can be easily customized and tailored to the required hardware and environment, i.e. to perform application updates over various interfaces and even to implement over-the-air (OTA) updates. In order to perform successful in-application-programming, the following sequence has to be kept:
1. Check for write protection and disable it if necessary.
2. Initialize flash with `Bootloader_Init()`.
3. Erase application space with `Bootloader_Erase()` (optional, but recommended).
4. Prepare for programming with `Bootloader_FlashBegin()`.
5. Perform programming by repeatedly calling the `Bootloader_FlashNext(uint64_t data)` function. The flashing procedure requires 8 bytes of data (double word) to be programmed at once into the flash. This function automatically increases the address where the data is being written.
6. Finalize programming by calling `Bootloader_FlashEnd()`.

The application image has to be in binary format. If the checksum verification is enabled, the binary must include the checksum value at the end of the image. When creating the application image, the checksum has to be calculated over the entire image (except the checksum area) with the following parameters:
- Algorithm: CRC32
- Size: 4 bytes
- Initial value: 0xFFFFFFFF
- Bit order: MSB first

__Important notice__: in order to perform a successful application jump from the bootloader, the vector table of the application should be relocated. By default, the startup code of the microcontroller sets the vector table location to 0x00000000 in the `system_stm32xxxx.c` file. This has to be either disabled (the bootloader can be configured to perform the vector table relocation before the jump) or manually set to the appropriate value which is the start address of the application space.

## Configuration
The bootloader can be widely configured in the `bootloader.h` file. The file includes detailed descriptions and comments for the configurable parameters and definitions.

## References
[1] STM32L476VG, http://www.st.com/en/microcontrollers/stm32l476vg.html

[2] RM0351, “STM32L4x5 and STM32L4x6 advanced ARM®-based 32-bit MCUs Reference Manual”, http://www.st.com/resource/en/reference_manual/dm00083560.pdf

[3] UM1884, “Description of STM32L4 HAL and Low Layer drivers”, http://www.st.com/resource/en/user_manual/dm00173145.pdf

[4] AN2606, "STM32 microcontroller system memory boot mode", http://www.st.com/resource/en/application_note/cd00167594.pdf

[5] AN3156, "USB DFU protocol used in the STM32 bootloader", http://www.st.com/resource/en/application_note/cd00264379.pdf

[6] UM0412, "Getting started with DfuSe USB device firmware upgrade", http://www.st.com/resource/en/user_manual/cd00155676.pdf
