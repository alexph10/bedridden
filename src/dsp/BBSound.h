#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace bedridden {

/// Says "yes" to every note on every channel. We don't need split/layer
/// behaviour, so this is as simple as it gets.
class BBSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote    (int /*midiNote*/)    override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};

} // namespace bedridden
