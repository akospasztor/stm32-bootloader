# Changelog for STM32 Bootloader

## [Unreleased]
- Check checksum of application found on SD card before programming
- Switch to semantic versioning


## [1.08](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.08) - 2018-04-18
### Added
- Verify flash content after programming
- Introduced changelog file
### Changed
- The bootloader is now optimized for a new hardware. Due to new pinout of LEDs, user button and SD card switch, the appropriate defines have been changed in main.h file.
- Bootloader sequence with richer error and debug messages
- Updated SD card driver with DMA support
- Project now uses IAR CMSIS pack
- Updated STM32L4xx CMSIS to 1.4.2
- Updated STM32L4xx HAL library to 1.8.2
### Fixed
- Changed appropriate variable types
- Updated README and sequence graph
- Updated header files


## [1.07](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.07) - 2017-12-08
### Added
- SD card power on/off support
### Changed
- As a result of recent hardware changes introduced in a device that is developed as part of our ongoing projects, the SD card is now powered on/off with a FET controlled by the MCU to minimize energy consumption. Therefore, the SD card has to be manually powered on during initialization.


## [1.06](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.06) - 2017-11-13
### Added
- Introduced RAM_SIZE define to precisely check whether the flash contains valid application upon startup
### Fixed
- CheckForApplication() function


## [1.05](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.05) - 2017-10-30
### Added
- Application-specific configuration defines
### Changed
- Updated STM32L4xx CMSIS to 1.4.1
- Updated STM32L4xx HAL library to 1.8.1


## [1.04](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.04) - 2017-10-16
### Added
- Option to skip programming after flash erase operation
### Changed
- Better visual feedback when no application is found in flash
### Fixed
- General improvements


## [1.03](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.03) - 2017-09-21
### Added
- Support for STM32L496VG MCU
- Multiple build configurations for each supported microcontroller
- Change between different builds with a single click
### Changed
- Updated FatFs to R0.12c
- Updated Cortex-M CMSIS to 4.5
- Updated STM32L4xx CMSIS to 1.4.0
- Updated STM32L4xx HAL library to 1.8.0


## [1.02](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.02) - 2017-08-11
### Fixed
- General improvements


## [1.01](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.01) - 2017-07-25
### Added
- Option for clearing reset flags after startup
### Fixed
- Bootloader sequence graph
- Readme updates
- General improvements


## [1.00](https://github.com/akospasztor/stm32-bootloader/releases/tag/v1.00) - 2017-05-21
### Added
Initial release
