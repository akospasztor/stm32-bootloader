################################################################################
# STM32 Bootloader
################################################################################
# @author    Akos Pasztor
# @brief     This file contains the clang-tools toolchain configuration for
#            CMake.
################################################################################
# @copyright (c) 2026 Akos Pasztor.                     https://akospasztor.com
#            This software is licensed under terms that can be found in the
#            LICENSE file in the root directory of this software component.
################################################################################

# Specify the clang tools binary folder if it does not exist in PATH
# Note: the path must be specified without trailing slash
set(CLANG_TOOLS_BIN_PATH
    "C:/llvm/10.0.0/bin"
)

if(EXISTS ${CLANG_TOOLS_BIN_PATH})
    list(APPEND CMAKE_PREFIX_PATH
        ${CLANG_TOOLS_BIN_PATH}
    )
    message(STATUS "Custom clang tools path has been specified.")
    message(STATUS "Updated CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif()

find_program(CLANG_FORMAT   clang-format)
find_program(CLANG_TIDY     clang-tidy)

# Set the arguments for clang-tidy
set(CLANG_TIDY_ARGS
    -format-style=file
)

# Set the extra arguments (clang compilation options) for clang-tidy
set(CLANG_TIDY_EXTRA_ARGS
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Weverything  # This flag turns on *all* diagnostic checks. Use with care.
    --target=arm-none-eabi
    --sysroot=${TOOLCHAIN_SYSROOT}  # This must be set by the used toolchain.
)
