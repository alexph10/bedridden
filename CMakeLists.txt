cmake_minimum_required(VERSION 3.22)

project(Bedridden
    VERSION     0.1.0
    DESCRIPTION "A weird 3-osc VST/AU instrument inspired by Blood Bucket"
    LANGUAGES   C CXX)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Place built plugin bundles in a predictable place.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ---- Options ----
option(BEDRIDDEN_BUILD_PLUGIN "Build the VST3/Standalone JUCE plugin" ON)
option(BEDRIDDEN_BUILD_DLL    "Build the reusable BedriddenDSP shared library" ON)
option(BEDRIDDEN_BUILD_TESTS  "Build unit tests"                                ON)

# Reusable DSP shared library (BedriddenDSP.dll on Windows).
if(BEDRIDDEN_BUILD_DLL)
    add_subdirectory(dsplib)
endif()

# JUCE plugin (Bedridden.vst3 + standalone). VST3 is itself a DLL on Windows.
if(BEDRIDDEN_BUILD_PLUGIN)
    include(cmake/FetchJUCE.cmake)
    add_subdirectory(src)
endif()

# Tests.
if(BEDRIDDEN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
