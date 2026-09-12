################################################################################
# STM32 Bootloader
################################################################################
# @author    Akos Pasztor
# @brief     This file contains the python toolchain configuration for CMake.
################################################################################
# @copyright (c) 2026 Akos Pasztor.                     https://akospasztor.com
#            This software is licensed under terms that can be found in the
#            LICENSE file in the root directory of this software component.
################################################################################

find_package(Python)

# If virtualenv is installed, then try to use the virtualenv instead of the
# system python interpreter. For this, the VIRTUAL_ENV environmental variable
# must be set to an existing virtualenv.
find_program(VIRTUALENV virtualenv)
if(VIRTUALENV AND DEFINED ENV{VIRTUAL_ENV})
    message(NOTICE
        "Python virtualenv found, attempting to use the virtualenv "
        "$ENV{VIRTUAL_ENV} as the python interpreter.")
    set(Python_FIND_VIRTUALENV FIRST)
    unset(Python_EXECUTABLE)
    find_package(Python)
endif()
