juce_add_plugin(Bedridden
    COMPANY_NAME              "alexph10"
    BUNDLE_ID                 "com.alexph10.bedridden"
    PLUGIN_MANUFACTURER_CODE  Alxp
    PLUGIN_CODE               Bdrn
    FORMATS                   VST3 Standalone
    PRODUCT_NAME              "Bedridden"
    IS_SYNTH                  TRUE
    NEEDS_MIDI_INPUT          TRUE
    NEEDS_MIDI_OUTPUT         FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD   FALSE
    VST3_CATEGORIES           "Instrument" "Synth"
)

target_sources(Bedridden PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h

    dsp/Oscillator.h
    dsp/Algorithm.h
    dsp/BBSound.h
    dsp/BBVoice.h
    dsp/BBVoice.cpp

    gui/LookAndFeel.h
    gui/LookAndFeel.cpp
    gui/OscPanel.h
    gui/OscPanel.cpp
)

target_include_directories(Bedridden PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(Bedridden PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Bedridden
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
