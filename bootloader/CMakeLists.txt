cmake_minimum_required(VERSION 3.22)

project(bootloader)
enable_language(C ASM)

# Set bootloader include paths
set(BOOTLOADER_INCLUDES
    include
)

# Set bootloader source files
set(BOOTLOADER_SOURCES
    source/bootloader.c
    source/error_handler.c
    source/main.c
    source/sd_diskio.c
    source/sd_fatfs.c
)

add_library(bootloader INTERFACE)

target_include_directories(bootloader INTERFACE
    ${BOOTLOADER_INCLUDES}
)

target_sources(bootloader INTERFACE
    ${BOOTLOADER_SOURCES}
)

target_link_libraries(bootloader INTERFACE
    bsp
    lib
)

# Prepend the include list and source files with the current directory path
list(TRANSFORM BOOTLOADER_INCLUDES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM BOOTLOADER_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

# Export the collected includes and source files to the global lists
set(INCLUDES
    ${INCLUDES}
    ${BOOTLOADER_INCLUDES}
    PARENT_SCOPE
)
set(SOURCES_C
    ${SOURCES_C}
    ${BOOTLOADER_SOURCES}
    PARENT_SCOPE
)
