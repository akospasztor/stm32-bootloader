cmake_minimum_required(VERSION 3.22)

project(lib)
enable_language(C CXX ASM)

# Set common library include paths
set(LIB_INCLUDES
    CMSIS/Include
)

# Set target-specific library include paths and source files
if(BSP MATCHES "^STM32L4")
    set(LIB_INCLUDES
        ${LIB_INCLUDES}
        CMSIS/Device/ST/STM32L4xx/Include
        FatFs
        STM32L4xx_HAL_Driver/Inc
    )
    set(LIB_SOURCES
        FatFs/option/unicode.c
        FatFs/diskio.c
        FatFs/ff_gen_drv.c
        FatFs/ff.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
        STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c
    )
else()
    message(FATAL_ERROR "Unsupported BSP: ${BSP}")
endif()

add_library(lib INTERFACE)

target_include_directories(lib INTERFACE
    ${LIB_INCLUDES}
)

target_sources(lib INTERFACE
    ${LIB_SOURCES}
)

target_link_libraries(lib INTERFACE
)

# Collect include paths for clang-tidy as system includes
set(LIB_CLANG_TIDY_SYSTEM_INCLUDES
    ${LIB_INCLUDES}
)

# Prepend the include list with the path of the current directory
list(TRANSFORM LIB_CLANG_TIDY_SYSTEM_INCLUDES
     PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/
)

# Export the collected system includes to the clang-tidy system includes list
set(CLANG_TIDY_SYSTEM_INCLUDES
    ${CLANG_TIDY_SYSTEM_INCLUDES}
    ${LIB_CLANG_TIDY_SYSTEM_INCLUDES}
    PARENT_SCOPE
)
