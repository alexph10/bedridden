# Reusable engine as a plain shared library — produces:
#   Windows : BedriddenDSP.dll  (+ BedriddenDSP.lib import lib)
#   macOS   : libBedriddenDSP.dylib
#   Linux   : libBedriddenDSP.so

add_library(BedriddenDSP SHARED
    BedriddenDSP.h
    BedriddenDSP.cpp
)

target_include_directories(BedriddenDSP
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}      # for BedriddenDSP.h
    PRIVATE ${CMAKE_SOURCE_DIR}/src           # for dsp/Algorithm.h
)

target_compile_features(BedriddenDSP PUBLIC cxx_std_20)

set_target_properties(BedriddenDSP PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    WINDOWS_EXPORT_ALL_SYMBOLS OFF    # we use __declspec(dllexport) explicitly
    SOVERSION 0
    VERSION   ${PROJECT_VERSION}
)

# Hide everything not explicitly exported (clean ABI surface).
if(NOT MSVC)
    target_compile_options(BedriddenDSP PRIVATE -fvisibility=hidden)
endif()
