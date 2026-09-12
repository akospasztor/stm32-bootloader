################################################################################
# STM32 Bootloader
################################################################################
# @author    Akos Pasztor
# @brief     This file contains the cross-compiler toolchain configuration for
#            CMake.
################################################################################
# @copyright (c) 2026 Akos Pasztor.                     https://akospasztor.com
#            This software is licensed under terms that can be found in the
#            LICENSE file in the root directory of this software component.
################################################################################

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_VERSION            1)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_FORCED         TRUE)
set(CMAKE_CXX_COMPILER_FORCED       TRUE)

set(CROSS_COMPILER_PREFIX           arm-none-eabi)

# Specify the toolchain binary folder if it does not exist in PATH
# Note: the path must be specified without trailing slash
set(CROSS_COMPILER_BIN_PATH
    # "C:/gcc/10-2020-q4/bin"
    "/opt/gcc/10-2020-q4/bin"
)

if(EXISTS ${CROSS_COMPILER_BIN_PATH})
    list(APPEND CMAKE_PREFIX_PATH
        ${CROSS_COMPILER_BIN_PATH}
    )
    message(STATUS "Custom cross compiler path has been specified.")
endif()

find_program(CMAKE_C_COMPILER       ${CROSS_COMPILER_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER     ${CROSS_COMPILER_PREFIX}-g++)
find_program(CMAKE_ASM_COMPILER     ${CROSS_COMPILER_PREFIX}-gcc)
find_program(CMAKE_LINKER           ${CROSS_COMPILER_PREFIX}-g++)
find_program(CMAKE_OBJCOPY          ${CROSS_COMPILER_PREFIX}-objcopy)
find_program(CMAKE_OBJDUMP          ${CROSS_COMPILER_PREFIX}-objdump)
find_program(CMAKE_SIZE             ${CROSS_COMPILER_PREFIX}-size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

# Export the normalized toolchain sysroot path (required for clang-tidy)
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE TOOLCHAIN_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
cmake_path(SET TOOLCHAIN_SYSROOT NORMALIZE ${TOOLCHAIN_SYSROOT})
