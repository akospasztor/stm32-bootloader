# Changelog

## [Unreleased]
- Check checksum of application found on SD card before programming


## [1.08] - 2018-04-18
### Added
- Verify flash content after programming
- Introduced changelog file

### Changed
- The bootloader is now optimized for a new hardware. Due to new pinout of LEDs, user button and SD card switch, the appropriate defines have been changed in main.h file.
- Bootloader sequence with richer error and debug messages
- Updated SD driver with DMA support
- Updated ST HAL library

### Fixed
- Project now uses IAR CMSIS pack
- Changed appropriate variable types
- Updated README and sequence graph
- Updated header files


## [1.07] - 2017-12-08
### Added
- SD card power on/off support
### Changed
- As a result of recent hardware changes introduced in a device that is developed as part of our ongoing projects, the SD card is now powered on/off with a FET controlled by the MCU to minimize energy consumption. Therefore, the SD card has to be manually powered on during initialization.
