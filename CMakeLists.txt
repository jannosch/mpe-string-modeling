cmake_minimum_required(VERSION 3.22)
project(mpesynth)

set(CMAKE_CXX_STANDARD 20)

# setup guide:
# https://www.youtube.com/watch?v=Uq7Hwt18s3s
# https://github.com/JanWilczek/audio-plugin-template

IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs-debug)
ELSE()
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs-release)
ENDIF()

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/get_cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
CPMAddPackage(
        NAME JUCE
        GIT_TAG 7.0.12
        VERSION 7.0.12
        GITHUB_REPOSITORY juce-framework/JUCE
        SOURCE_DIR ${LIB_DIR}/juce
)

# Adds googletest.
#CPMAddPackage(
#        NAME GOOGLETEST
#        GITHUB_REPOSITORY google/googletest
#        GIT_TAG v1.13.0
#        VERSION 1.13.0
#        SOURCE_DIR ${LIB_DIR}/googletest
#        OPTIONS
#        "INSTALL_GTEST OFF"
#        "gtest_force_shared_crt ON"
#)

# Adds Eigen
CPMAddPackage(
        NAME Eigen
        GIT_TAG 3.4.0
        VERSION 3.4.0
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen
        DOWNLOAD_ONLY true
        SOURCE_DIR ${LIB_DIR}/eigen
)
include_directories(${LIB_DIR}/eigen)

# This command allows running tests from the "build" folder (the one where CMake generates the project to).
enable_testing()


IF(MSVC)
    #
ELSE()
    IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(
                -O0
                -march=native       # vectorization
        )
    ENDIF()
ENDIF()


# Adds all the targets configured in the "plugin" folder.
add_subdirectory(plugin)
