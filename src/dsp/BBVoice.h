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
    // Pull current APVTS values into local fields + update ADSR + osc settings.
    void refreshFromParams();

    // Convert an APVTS choice index to our Wave enum, defensively clamped.
    static dsp::Wave waveFromIndex (int idx) noexcept;

    // Convert (coarse semitones, fine cents) to a frequency multiplier.
    static float pitchMultiplier (float coarseSemis, float fineCents) noexcept;

    // ----- Cached raw param pointers (cheap atomic reads on the audio thread) -----
    struct OscParams
    {
        std::atomic<float>* wave   = nullptr;
        std::atomic<float>* coarse = nullptr;
        std::atomic<float>* fine   = nullptr;
        std::atomic<float>* level  = nullptr;
    };
    struct EnvParams
    {
        std::atomic<float>* attack  = nullptr;
        std::atomic<float>* decay   = nullptr;
        std::atomic<float>* sustain = nullptr;
        std::atomic<float>* release = nullptr;
    };

    juce::AudioProcessorValueTreeState& apvts;

    OscParams pA, pB, pC;
    EnvParams eA, eB;
    std::atomic<float>* pOutGain = nullptr;

    // ----- DSP state -----
    dsp::Oscillator     oscA, oscB, oscC;
    dsp::AlgorithmState algoState {};

    juce::ADSR envA, envB, envAmp;

    float noteFreq = 440.0f;
    float velocity = 0.0f;
};

} // namespace bedridden
