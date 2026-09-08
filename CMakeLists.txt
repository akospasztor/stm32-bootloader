################################################################################
# STM32 Bootloader
################################################################################
# @author    Akos Pasztor
# @brief     This file is the central configuration script used by CMake to
#            define how this firmware project is built. It also contains custom
#            target definitions to execute checks, static code analysis, to
#            build the documentation and more.
################################################################################
# @copyright (c) 2026 Akos Pasztor.                     https://akospasztor.com
#            This software is licensed under terms that can be found in the
#            LICENSE file in the root directory of this software component.
################################################################################

cmake_minimum_required(VERSION 3.22)

project(stm32-bootloader)
enable_language(C CXX ASM)

# Set the following variable to `ON` to print all cmake variables
set(CMAKE_DUMP_VARIABLES            OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              14)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)

# Define the default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message(NOTICE "Build type: ${CMAKE_BUILD_TYPE}")

# Get the number of processors available
include(ProcessorCount)
ProcessorCount(NPROC)

# Include the required build tools
include(cmake/tools/clang-tools.cmake)
include(cmake/tools/python.cmake)

# Include common microcontroller parameters
include(cmake/microcontrollers/common.cmake)

# Include the microcontroller-specific parameters
if(BSP STREQUAL "STM32L496-DISCOVERY")
    # Set linker script
    set(LINKER_SCRIPT
        ${PROJECT_SOURCE_DIR}/bsp/stm32l496-discovery/gcc-arm/stm32l496xx_flash.ld
    )
    include(cmake/microcontrollers/stm32l4-gcc.cmake)
else()
    message(FATAL_ERROR "Unsupported BSP: ${BSP}")
endif()

# Set common defines
set(DEFINES
    USE_HAL_DRIVER
    USE_SDMMC_DMA
    # $<$<CONFIG:Debug>:USE_FULL_ASSERT>
    USE_FULL_ASSERT
    $<IF:$<CONFIG:Debug>,DEBUG,NDEBUG>
)

# Set target-specific defines
if(BSP STREQUAL "STM32L496-DISCOVERY")
    set(DEFINES
        ${DEFINES}
        STM32L496xx
    )
else()
    message(FATAL_ERROR "Unsupported BSP: ${BSP}")
endif()

# Initialize common includes and source files lists for the clang tools
set(INCLUDES    "")
set(SOURCES_C   "")

# Add firmware components
add_subdirectory(bootloader)
add_subdirectory(bsp)
add_subdirectory(lib)

# Specify target executable
add_executable(${CMAKE_PROJECT_NAME})

# Specify compile definitions
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ${DEFINES}
)

# Specify firmware components to use when linking the target
# Note: the requirements from linked components are propagated.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
   bootloader
)

# Generate size information
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)

# Generate binary output
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_COMMAND} -E echo "Create bin output: ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin"
)

# Generate hex output
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_COMMAND} -E echo "Create hex output: ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex"
)

# Add custom target to run dos2unix
add_custom_target(dos2unix
    COMMAND find ${PROJECT_SOURCE_DIR}
            -not -path '${PROJECT_SOURCE_DIR}/build/*'
            -name '*.[ch]'
            -exec dos2unix --add-eol {} +
    COMMENT "Running dos2unix..."
)

# Add custom target to check source code formatting with clang-format
add_custom_target(check-format
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/script/clang_format.py
            --check
            --style=file
            --executable=${CLANG_FORMAT}
            ${INCLUDES} ${SOURCES_C}
    COMMENT "Check source code formatting..."
)

# Add custom target to run clang-format
add_custom_target(run-format
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/script/clang_format.py
            --style=file
            --executable=${CLANG_FORMAT}
            ${INCLUDES} ${SOURCES_C}
    COMMENT "Run source code formatting..."
)

# Add custom target to run clang-tidy
set(CLANG_TIDY_DEFINES
    ${DEFINES}
)
set(CLANG_TIDY_INCLUDES
    ${INCLUDES}
)
list(TRANSFORM CLANG_TIDY_DEFINES PREPEND -D)
list(TRANSFORM CLANG_TIDY_INCLUDES PREPEND -I)
list(TRANSFORM CLANG_TIDY_SYSTEM_INCLUDES PREPEND -isystem)
add_custom_target(tidy
    COMMAND ${CLANG_TIDY}
            ${CLANG_TIDY_ARGS}
            ${SOURCES_C}
            --
            ${CLANG_TIDY_EXTRA_ARGS}
            ${CLANG_TIDY_DEFINES}
            ${CLANG_TIDY_INCLUDES}
            ${CLANG_TIDY_SYSTEM_INCLUDES}
    COMMENT "Run clang-tidy..."
)

# Add custom target to run cloc
add_custom_target(cloc
    COMMAND cloc
            --match-f="\.[ch]$$"
            --exclude-dir="build"
            --by-file-by-lang
            --report-file=${PROJECT_SOURCE_DIR}/build/cloc/cloc.txt
            ${PROJECT_SOURCE_DIR}
    COMMENT "Running cloc..."
)

# Create directories for cppcheck because they are not created automatically
set(CPPCHECK_BUILD_DIR ${CMAKE_BINARY_DIR}/cppcheck)
set(CPPCHECK_REPORT_DIR ${CMAKE_BINARY_DIR}/cppcheck/report)
file(MAKE_DIRECTORY ${CPPCHECK_BUILD_DIR})
file(MAKE_DIRECTORY ${CPPCHECK_REPORT_DIR})

# Add custom target to run cppcheck
add_custom_target(cppcheck
    COMMAND cppcheck
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --cppcheck-build-dir=${CPPCHECK_BUILD_DIR}
            --checkers-report=${CPPCHECK_REPORT_DIR}/checkers.txt
            --addon=${PROJECT_SOURCE_DIR}/lint/misra.json
            --check-level=exhaustive
            --enable=all
            --error-exitcode=1
            --inconclusive
            --inline-suppr
            --max-ctu-depth=10
            --safety
            --std=c11
            --suppressions-list=${PROJECT_SOURCE_DIR}/lint/suppressions.txt
            -D__GNUC__
            -j${NPROC}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Running cppcheck..."
)


# Add custom target to generate doxygen documentation
add_custom_target(doxygen
    COMMAND doxygen docs/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generate doxygen documentation..."
)

# Specify additional clean files
set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    ${CMAKE_PROJECT_NAME}.map
    ${CMAKE_PROJECT_NAME}.bin
    ${CMAKE_PROJECT_NAME}.hex
)

# Dump all cmake variables
if(CMAKE_DUMP_VARIABLES)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
