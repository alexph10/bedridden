#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace bedridden {

/// A trivial sound that says "yes, play me on any MIDI channel/note".
class BBSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote    (int /*midiNote*/)    override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};

} // namespace bedridden
