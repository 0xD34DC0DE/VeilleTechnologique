#
# Genebits
#

cmake_minimum_required(VERSION 3.20)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Preventing in-source building. Please run cmake in a separate build directory.")
endif ()

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

#
# Project Configuration
#

set(ASMLGEN_VERSION_REGEX "#define ASMLGEN_VERSION_.*[ \t]+(.+)")
file(STRINGS "application/include/asmlgen/application/config/version.h" ASMLGEN_VERSION REGEX ${ASMLGEN_VERSION_REGEX})
list(TRANSFORM ASMLGEN_VERSION REPLACE ${ASMLGEN_VERSION_REGEX} "\\1")
string(JOIN "." ASMLGEN_VERSION ${ASMLGEN_VERSION})

project(
        ASMLGen
        VERSION ${ASMLGEN_VERSION}
        DESCRIPTION "Automatic small machine learning model generator"
        HOMEPAGE_URL "https://github.com/0xD34DC0DE/VeilleTechnologique"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR})

include(cmake/Testing.cmake)
include(cmake/CompilerWarnings.cmake)


#
# Install Packages
#

include(cmake/Conan.cmake)

if (BUILD_TESTS)
    add_conan_package(gtest/1.11.0)
endif ()

run_conan()

#
# Application
#

add_subdirectory(application)

#
# Sandboxes
#

add_subdirectory(sandbox)