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
    // Grab the latest knob positions and push them into the oscs and envs.
    void refreshFromParams();

    // The wave dropdown gives us an int; clamp it so a bad state file can't
    // run us off the end of the table.
    static dsp::Wave waveFromIndex (int idx) noexcept;

    // (coarse semitones, fine cents) → frequency multiplier.
    static float pitchMultiplier (float coarseSemis, float fineCents) noexcept;

    // We cache the raw param pointers once at construction so the audio
    // thread never has to do a string lookup. These are atomic reads, which
    // is the cheapest synchronisation we can get.
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
    std::atomic<float>* pAlgo      = nullptr;
    std::atomic<float>* pModTarget = nullptr;
    std::atomic<float>* pModDepth  = nullptr;
    std::atomic<float>* pOutGain   = nullptr;

    // ----- DSP state -----
    dsp::Oscillator     oscA, oscB, oscC;
    dsp::AlgorithmState algoState {};

    juce::ADSR envA, envB, envAmp;

    // Simple high-pass to kill the DC offset that XOR / AND / threshold
    // love to introduce. One pole, one pair of state vars — cheap.
    float dcPrevIn  = 0.0f;
    float dcPrevOut = 0.0f;

    float noteFreq = 440.0f;
    float velocity = 0.0f;
};

} // namespace bedridden
