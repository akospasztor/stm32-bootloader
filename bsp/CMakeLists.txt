cmake_minimum_required(VERSION 3.22)

project(bsp)
enable_language(C CXX ASM)

# Set target-specific BSP paths and source files
if(BSP STREQUAL "STM32L496-DISCOVERY")
    set(BSP_INCLUDES
        stm32l496-discovery/include
    )
    set(BSP_SOURCES_ASM
        stm32l496-discovery/gcc-arm/startup_stm32l496xx.s
    )
    set(BSP_SOURCES_C
        stm32l496-discovery/source/board.c
        stm32l496-discovery/source/button.c
        stm32l496-discovery/source/crc.c
        stm32l496-discovery/source/delay.c
        stm32l496-discovery/source/flash.c
        stm32l496-discovery/source/led.c
        stm32l496-discovery/source/log.c
        stm32l496-discovery/source/rcc.c
        stm32l496-discovery/source/sdmmc.c
        stm32l496-discovery/source/stm32l4xx_it.c
        stm32l496-discovery/source/system_stm32l4xx.c
        stm32l496-discovery/source/systick.c
    )
else()
    message(FATAL_ERROR "Unsupported BSP: ${BSP}")
endif()

add_library(bsp INTERFACE)

target_include_directories(bsp INTERFACE
    ${BSP_INCLUDES}
)

target_sources(bsp INTERFACE
    ${BSP_SOURCES_ASM}
    ${BSP_SOURCES_C}
)

target_link_libraries(bsp INTERFACE
    lib
)

# Prepend the include list and source files with the current directory path
list(TRANSFORM BSP_INCLUDES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM BSP_SOURCES_C PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

# Export the collected includes and source files to the global lists
set(INCLUDES
    ${INCLUDES}
    ${BSP_INCLUDES}
    PARENT_SCOPE
)
set(SOURCES_C
    ${SOURCES_C}
    ${BSP_SOURCES_C}
    PARENT_SCOPE
)
