# Minimal, dependency-free unit tests for the pure-DSP headers.
# We don't link JUCE here — the algorithm/oscillator code is header-only and
# self-contained so the tests stay fast.

add_executable(bedridden_tests
    test_algorithms.cpp
)

target_include_directories(bedridden_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(bedridden_tests PRIVATE cxx_std_20)

add_test(NAME algorithms COMMAND bedridden_tests)
