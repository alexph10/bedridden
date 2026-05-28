#include "BBVoice.h"
#include "BBSound.h"

namespace bedridden {

BBVoice::BBVoice (juce::AudioProcessorValueTreeState& state)
    : apvts (state)
{
}

bool BBVoice::canPlaySound (juce::SynthesiserSound* s)
{
    return dynamic_cast<BBSound*> (s) != nullptr;
}

void BBVoice::startNote (int midiNote, float velocity,
                         juce::SynthesiserSound*, int /*currentPitchWheel*/)
{
    noteFreq = (float) juce::MidiMessage::getMidiNoteInHertz (midiNote);
    level    = velocity;

    const auto sr = getSampleRate();
    oscA.prepare (sr);
    oscB.prepare (sr);
    oscC.prepare (sr);
    envA.setSampleRate (sr);
    envB.setSampleRate (sr);
    envAmp.setSampleRate (sr);

    // Simple amp envelope mirrors env A for now.
    envA.noteOn();
    envB.noteOn();
    envAmp.noteOn();
}

void BBVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        envA.noteOff();
        envB.noteOff();
        envAmp.noteOff();
    }
    else
    {
        envA.reset(); envB.reset(); envAmp.reset();
        clearCurrentNote();
    }
}

void BBVoice::renderNextBlock (juce::AudioBuffer<float>& outBuffer,
                               int startSample, int numSamples)
{
    // TODO: wire up parameter reads + algorithm dispatch.
    // Scaffold only — leaves the buffer untouched.
    juce::ignoreUnused (outBuffer, startSample, numSamples);
}

void BBVoice::applyParameters()
{
    // TODO: pull values from apvts and set osc freqs, waves, ADSR params, etc.
}

} // namespace bedridden
