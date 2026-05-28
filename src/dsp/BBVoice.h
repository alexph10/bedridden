#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Algorithm.h"
#include "Oscillator.h"

namespace bedridden {

class BBVoice : public juce::SynthesiserVoice
{
public:
    explicit BBVoice (juce::AudioProcessorValueTreeState& state);

    bool canPlaySound (juce::SynthesiserSound*) override;

    void startNote        (int midiNote, float velocity,
                           juce::SynthesiserSound*, int currentPitchWheel) override;
    void stopNote         (float velocity, bool allowTailOff) override;
    void pitchWheelMoved  (int) override {}
    void controllerMoved  (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

private:
    void applyParameters();

    juce::AudioProcessorValueTreeState& apvts;

    dsp::Oscillator oscA, oscB, oscC;
    dsp::AlgorithmState algoState {};

    juce::ADSR envA, envB, envAmp;
    juce::ADSR::Parameters paramsA, paramsB;

    float noteFreq = 440.f;
    float level    = 0.f;
};

} // namespace bedridden
