# STM32 Bootloader

Customizable Bootloader for STM32 microcontrollers. This project includes demonstrations for various hardware how to perform in-application-programming of a firmware located on external SD card with FAT32 file system.

Each example uses the same bootloader library located in the `lib/stm32-bootloader` folder. The examples are located in the `projects` folder and they come with a separate, dedicated README file with description related to that specific implementation.

**Update:** the `STM32L496-Discovery` example supports compiling and building the project with the GNU Arm Embedded Toolchain (ARM GCC) out-of-the-box, in addition to IAR EWARM. Check out the project README for further information.

Please refer to <https://akospasztor.github.io/stm32-bootloader> for complete documentation of the bootloader library source code.

![Build Status](https://dev.azure.com/akospasztor/stm32-bootloader/_apis/build/status/akospasztor.stm32-bootloader?branchName=master)

## Table of Contents
- [Bootloader features](#bootloader-features)
- [Source code organization](#source-code-organization)
- [Examples](#examples)
- [How to use](#how-to-use)
- [Configuration](#configuration)
- [References](#references)

## Bootloader features
- Configurable application space
- Flash erase
- Flash programming
- Flash verification after programming
- Checksum verification
- Flash protection check, write protection enable/disable
- Extended error handling, fail-safe design
- Bootloader firmware update and the ability to perform full chip re-programming: enter ST's built-in bootloader from software (without triggering the BOOT pin)
- Serial tracing over SWO for easier debugging and development
- Easy to customize and port to other microcontrollers

## Source code organization
```
Repository
├── docs
├── drivers
│   ├── CMSIS
│   └── STM32L4xx_HAL_Driver
├── lib
│   ├── fatfs
│   └── stm32-bootloader
└── projects
    ├── STM32L476-CustomHw
    ├── STM32L496-CustomHw
    └── STM32L496-Discovery
```
The `docs` folder contains the generated documentation of the bootloader source code and other documentation-related static files.

The `drivers` folder contains the CMSIS (Cortex Microcontroller Software Interface Standard) as well as the HAL (Hardware Abstraction Layer) drivers from ST.

The bootloader source code and corresponding header file can be found in `lib/stm32-bootloader` folder. Additionally, the `lib` folder contains the FatFs library as well.

The various demonstrations reside in the `projects` folder. Each example project contains an `include` and `source` folder where the header and source files are located respectively. The compiler and SDK-specific files are located in their respective subfolders. Furthermore, every example project has a dedicated README file explaining its functionality in detail.

## Examples
This repository contains the following examples.

|Microcontroller |Hardware           |Project path                 |
|----------------|-------------------|-----------------------------|
|STM32L476VG     |Custom             |projects/STM32L476-CustomHw  |
|STM32L496VG     |Custom             |projects/STM32L496-CustomHw  |
|STM32L496AG     |[32L496GDISCOVERY] |projects/STM32L496-Discovery |

[32L496GDISCOVERY]: https://www.st.com/en/evaluation-tools/32l496gdiscovery.html

## How to use
The bootloader can be easily customized and tailored to the required hardware and environment, i.e. to perform firmware updates over various interfaces or even to implement over-the-air (OTA) updates if the hardware incorporates wireless communication modules. In order to perform successful in-application-programming, the following sequence has to be kept:
1. Check for flash write protection and disable it if necessary.
2. Initialize flash with `Bootloader_Init()`.
3. Erase application space with `Bootloader_Erase()`.
4. Prepare for programming by calling `Bootloader_FlashBegin()`.
5. Perform programming by repeatedly calling the `Bootloader_FlashNext()` function. The programming procedure requires 8 bytes of data (double word) to be programmed at once into the flash. This function automatically increases the address where the data is being written.
6. Finalize programming by calling `Bootloader_FlashEnd()`.

The application image has to be in binary format. If the checksum verification is enabled, the binary must include the checksum value at the end of the image. When creating the application image, the checksum has to be calculated over the entire image (except the checksum area) with the following parameters:
- Algorithm: CRC32
- Size: 4 bytes
- Initial value: 0xFFFFFFFF
- Bit order: MSB first

__Important notes__:
- In order to perform a successful application jump from the bootloader, the vector table of the application firmware should be relocated. On system reset, the vector table is fixed at address 0x00000000. When creating an application, the microcontroller startup code sets the vector table offset to 0x0000 in the `system_stm32xxxx.c` file by default. This has to be either disabled (the bootloader can be configured to perform the vector table relocation before the jump) or manually set the vector table offset register (VTOR) to the appropriate offset value which is the start address of the application space. For more information, please refer to [[1]](#references).
- The linker settings of the application firmware need to be adjusted from their default settings so that the start address of flash reflects the actual start address of the application space.

## Configuration
The bootloader can be widely configured in the `bootloader.h` file. The file includes detailed comments and descriptions related to the configurable parameters and definitions.

## References
[1] PM0214, "STM32F3 Series, STM32F4 Series, STM32L4 Series and STM32L4+ Series Cortex®-M4 Programming Manual", http://www.st.com/resource/en/programming_manual/dm00046982.pdf
