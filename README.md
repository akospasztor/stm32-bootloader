# STM32 Bootloader
Customizable Bootloader for STM32 microcontrollers. This example demonstrates how to perform in-application-programming of a firmware located on external SD card with FAT32 file system.

## Table of Contents
- [Bootloader Features](#bootloader-features)
- [Description](#description)
- [How to Use](#how-to-use)
- [Configuration](#configuration)
- [References](#references)

## Bootloader Features
- Configurable application space
- Checksum verification
- Flash protection check, write protection enable/disable
- Extended error handling, fail-safe design
- Bootloader firmware update and the ability to perform full chip re-programming: enter ST's built-in bootloader from software (without triggering the BOOT pin)
- SWO tracing for easier debugging and development
- Easy to customize and port to other microcontrollers

## Description
This demo is implemented on a custom hardware [see FIGURE] equipped with a STM32L476VG microcontroller [[1]](#references). The microSD card is connected to the MCU over SDIO interface. The example software uses the official HAL library of ST [[3]](#references) and is compiled with IAR EWARM. Programming and debugging is performed over SWD with a SEGGER J-Link debug probe.

[FIGURE]



## How to Use

## Configuration

## References
[1] STM32L476VG, http://www.st.com/en/microcontrollers/stm32l476vg.html

[2] RM0351, “STM32L4x5 and STM32L4x6 advanced ARM®-based 32-bit MCUs Reference Manual”, http://www.st.com/resource/en/reference_manual/dm00083560.pdf

[3] UM1884, “Description of STM32L4 HAL and Low Layer drivers”, http://www.st.com/resource/en/user_manual/dm00173145.pdf

[4] AN4657, "STM32 in-application programming (IAP) using USART", http://www.st.com/resource/en/application_note/dm00161366.pdf

[5] AN3156, "USB DFU protocol used in the STM32 bootloader", http://www.st.com/resource/en/application_note/cd00264379.pdf

[6] UM0412, "Getting started with DfuSe USB device firmware upgrade", http://www.st.com/resource/en/user_manual/cd00155676.pdf
