################################################################################
# STM32 Bootloader
################################################################################
# @author    Akos Pasztor
# @brief     This file contains the common build flags for CMake.
################################################################################
# @copyright (c) 2026 Akos Pasztor.                     https://akospasztor.com
#            This software is licensed under terms that can be found in the
#            LICENSE file in the root directory of this software component.
################################################################################

# Set debug flags (Debug build type)
set(DEBUG_FLAGS
    "-Og"
    "-g"
)

# Set release flags (Release build type)
set(RELEASE_FLAGS
    "-O3"
)

# Set flags for release with optimization for minimum size
# (MinSizeRel build type)
set(MINSIZEREL_FLAGS
    "-Os"
)

# Set flags for release with debug info but no debug (output) code or asserts
# (RelWithDebInfo build type)
set(RELWITHDEBINFO_FLAGS
    "-O2"
    "-g"
)

# For all build types:
# Join the build type flags (separated by space character) into one string
# Set the created string containing the build type flags for all languages
list(JOIN DEBUG_FLAGS " " DEBUG_FLAGS)
set(CMAKE_ASM_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})

list(JOIN RELEASE_FLAGS " " RELEASE_FLAGS)
set(CMAKE_ASM_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})

list(JOIN MINSIZEREL_FLAGS " " MINSIZEREL_FLAGS)
set(CMAKE_ASM_FLAGS_MINSIZEREL ${MINSIZEREL_FLAGS})
set(CMAKE_C_FLAGS_MINSIZEREL ${MINSIZEREL_FLAGS})
set(CMAKE_CXX_FLAGS_MINSIZEREL ${MINSIZEREL_FLAGS})

list(JOIN RELWITHDEBINFO_FLAGS " " RELWITHDEBINFO_FLAGS)
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO ${RELWITHDEBINFO_FLAGS})
set(CMAKE_C_FLAGS_RELWITHDEBINFO ${RELWITHDEBINFO_FLAGS})
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${RELWITHDEBINFO_FLAGS})
