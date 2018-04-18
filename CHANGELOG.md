# Changelog

## [Unreleased]
- Check checksum of application found on SD card before programming

## [1.08] - 2018-04-18
### Added
- Verify flash content after programming
- Introduced changelog

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
