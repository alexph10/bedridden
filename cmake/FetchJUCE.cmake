# Pulls JUCE at configure time so contributors don't need it installed.
include(FetchContent)

set(JUCE_TAG "8.0.4" CACHE STRING "JUCE git tag to fetch")

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        ${JUCE_TAG}
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(JUCE)
