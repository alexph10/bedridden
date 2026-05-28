#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace bedridden {

/// A factory preset is just a name and a list of (param-id, value) pairs.
/// Deliberately not JSON, not files, not anything I/O — they get compiled
/// into the binary so they can never go missing on the user's machine.
struct FactoryPreset
{
    juce::String name;
    std::vector<std::pair<juce::String, float>> values;
};

inline std::vector<FactoryPreset> getFactoryPresets()
{
    return {
        {
            "INIT",
            {
                { "oscA_wave", 0 }, { "oscA_coarse", 0 }, { "oscA_fine", 0 }, { "oscA_level", 1.0f },
                { "oscB_wave", 0 }, { "oscB_coarse", 0 }, { "oscB_fine", 0 }, { "oscB_level", 1.0f },
                { "oscC_wave", 0 }, { "oscC_coarse", 0 }, { "oscC_fine", 0 }, { "oscC_level", 1.0f },
                { "envA_attack", 0.01f }, { "envA_decay", 0.20f }, { "envA_sustain", 0.7f }, { "envA_release", 0.30f },
                { "envB_attack", 0.01f }, { "envB_decay", 0.20f }, { "envB_sustain", 0.7f }, { "envB_release", 0.30f },
                { "algorithm", 0 }, { "mod_target", 0 }, { "mod_depth", 0.5f }, { "out_gain", 0.7f }
            }
        },
        {
            // Classic Blood Bucket digital crunch.
            "XOR CRUNCH",
            {
                { "oscA_wave", 2 }, { "oscA_coarse",  0 },  { "oscA_level", 1.0f },
                { "oscB_wave", 1 }, { "oscB_coarse",  7 },  { "oscB_level", 0.9f },
                { "oscC_wave", 0 }, { "oscC_coarse",  0 },  { "oscC_level", 1.0f },
                { "algorithm", 6 }, { "mod_target",   0 },  { "mod_depth",  0.6f },
                { "envA_attack", 0.002f }, { "envA_decay", 0.15f }, { "envA_sustain", 0.3f }, { "envA_release", 0.25f },
                { "envB_attack", 0.002f }, { "envB_decay", 0.40f }, { "envB_sustain", 0.5f }, { "envB_release", 0.40f },
                { "out_gain", 0.65f }
            }
        },
        {
            // Bell / ring-mod metallic.
            "BELL RING",
            {
                { "oscA_wave", 0 }, { "oscA_coarse",  7 }, { "oscA_level", 1.0f },
                { "oscB_wave", 0 }, { "oscB_coarse", 12 }, { "oscB_level", 1.0f },
                { "oscC_wave", 0 }, { "oscC_coarse",  0 }, { "oscC_level", 1.0f },
                { "algorithm", 0 }, { "mod_target",   2 }, { "mod_depth",  0.85f },
                { "envA_attack", 0.001f }, { "envA_decay", 1.50f }, { "envA_sustain", 0.0f }, { "envA_release", 1.50f },
                { "envB_attack", 0.001f }, { "envB_decay", 2.00f }, { "envB_sustain", 0.0f }, { "envB_release", 2.00f },
                { "out_gain", 0.7f }
            }
        },
        {
            // S&H glitch lead.
            "S&H GLITCH",
            {
                { "oscA_wave", 4 }, { "oscA_level", 0.9f },
                { "oscB_wave", 1 }, { "oscB_coarse", -5 }, { "oscB_level", 1.0f },
                { "oscC_wave", 2 }, { "oscC_coarse", 0 }, { "oscC_level", 1.0f },
                { "algorithm", 8 }, { "mod_target", 0 }, { "mod_depth", 0.8f },
                { "envA_attack", 0.001f }, { "envA_decay", 0.10f }, { "envA_sustain", 0.6f }, { "envA_release", 0.20f },
                { "envB_attack", 0.001f }, { "envB_decay", 0.05f }, { "envB_sustain", 0.8f }, { "envB_release", 0.20f },
                { "out_gain", 0.6f }
            }
        },
        {
            // DX-style PM stack.
            "DX STACK",
            {
                { "oscA_wave", 0 }, { "oscA_coarse",  0 }, { "oscA_level", 1.0f },
                { "oscB_wave", 0 }, { "oscB_coarse", 12 }, { "oscB_level", 1.0f },
                { "oscC_wave", 0 }, { "oscC_coarse",  0 }, { "oscC_level", 1.0f },
                { "algorithm", 9 }, { "mod_target",   0 }, { "mod_depth",  0.5f },
                { "envA_attack", 0.005f }, { "envA_decay", 0.80f }, { "envA_sustain", 0.4f }, { "envA_release", 0.60f },
                { "envB_attack", 0.005f }, { "envB_decay", 0.80f }, { "envB_sustain", 0.4f }, { "envB_release", 0.60f },
                { "out_gain", 0.7f }
            }
        }
    };
}

/// Apply a preset to APVTS. Anything not listed in the preset stays put,
/// which is handy when you only want to tweak a subset of params.
inline void applyPreset (juce::AudioProcessorValueTreeState& apvts,
                         const FactoryPreset& preset)
{
    for (const auto& [id, value] : preset.values)
    {
        if (auto* p = apvts.getParameter (id))
        {
            const auto range = p->getNormalisableRange();
            p->setValueNotifyingHost (range.convertTo0to1 (value));
        }
    }
}

} // namespace bedridden
