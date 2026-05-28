#include "BBVoice.h"
#include "BBSound.h"

#include <cmath>

namespace bedridden {

namespace {
    // Map APVTS choice index → our internal Wave enum.
    // Same order as PluginProcessor::createLayout() and
    // dsp::Wave in Oscillator.h.
    constexpr dsp::Wave kWaves[] = {
        dsp::Wave::Sine, dsp::Wave::Saw, dsp::Wave::Square,
        dsp::Wave::Triangle, dsp::Wave::Noise
    };
}

BBVoice::BBVoice (juce::AudioProcessorValueTreeState& state)
    : apvts (state)
{
    auto get = [&] (const char* id) { return apvts.getRawParameterValue (id); };

    pA = { get("oscA_wave"), get("oscA_coarse"), get("oscA_fine"), get("oscA_level") };
    pB = { get("oscB_wave"), get("oscB_coarse"), get("oscB_fine"), get("oscB_level") };
    pC = { get("oscC_wave"), get("oscC_coarse"), get("oscC_fine"), get("oscC_level") };

    eA = { get("envA_attack"), get("envA_decay"), get("envA_sustain"), get("envA_release") };
    eB = { get("envB_attack"), get("envB_decay"), get("envB_sustain"), get("envB_release") };

    pOutGain = get("out_gain");
}

bool BBVoice::canPlaySound (juce::SynthesiserSound* s)
{
    return dynamic_cast<BBSound*> (s) != nullptr;
}

dsp::Wave BBVoice::waveFromIndex (int idx) noexcept
{
    if (idx < 0) idx = 0;
    if (idx >= (int) std::size (kWaves)) idx = (int) std::size (kWaves) - 1;
    return kWaves[idx];
}

float BBVoice::pitchMultiplier (float coarseSemis, float fineCents) noexcept
{
    return std::pow (2.0f, (coarseSemis + fineCents * 0.01f) / 12.0f);
}

void BBVoice::startNote (int midiNote, float vel,
                         juce::SynthesiserSound*, int /*currentPitchWheel*/)
{
    noteFreq = (float) juce::MidiMessage::getMidiNoteInHertz (midiNote);
    velocity = vel;

    const auto sr = getSampleRate();
    oscA.prepare (sr); oscA.resetPhase();
    oscB.prepare (sr); oscB.resetPhase();
    oscC.prepare (sr); oscC.resetPhase();

    envA.setSampleRate (sr);
    envB.setSampleRate (sr);
    envAmp.setSampleRate (sr);

    algoState = {};
    refreshFromParams();

    envA.noteOn();
    envB.noteOn();
    envAmp.noteOn();
}

void BBVoice::stopNote (float /*vel*/, bool allowTailOff)
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

void BBVoice::refreshFromParams()
{
    // ---- Waves ----
    oscA.setWave (waveFromIndex ((int) pA.wave->load()));
    oscB.setWave (waveFromIndex ((int) pB.wave->load()));
    oscC.setWave (waveFromIndex ((int) pC.wave->load()));

    // ---- Frequencies (all three key-track the played note for now) ----
    const float fA = noteFreq * pitchMultiplier (pA.coarse->load(), pA.fine->load());
    const float fB = noteFreq * pitchMultiplier (pB.coarse->load(), pB.fine->load());
    const float fC = noteFreq * pitchMultiplier (pC.coarse->load(), pC.fine->load());
    oscA.setFrequency (fA);
    oscB.setFrequency (fB);
    oscC.setFrequency (fC);

    // ---- Envelopes ----
    juce::ADSR::Parameters a {
        eA.attack->load(), eA.decay->load(), eA.sustain->load(), eA.release->load() };
    juce::ADSR::Parameters b {
        eB.attack->load(), eB.decay->load(), eB.sustain->load(), eB.release->load() };

    envA.setParameters (a);
    envB.setParameters (b);
    // Step 1+2: the amp envelope mirrors Env A (Blood Bucket gates the
    // carrier off the modulator envelopes). Will become its own thing later.
    envAmp.setParameters (a);
}

void BBVoice::renderNextBlock (juce::AudioBuffer<float>& outBuffer,
                               int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    // Refresh params once per render call.
    refreshFromParams();

    const float lvlA = pA.level->load();
    const float lvlB = pB.level->load();
    const float lvlC = pC.level->load();
    const float gain = pOutGain ? pOutGain->load() : 0.7f;

    const int numCh = outBuffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        const float ampEnv = envAmp.getNextSample();

        // Advance modulator oscs + their envelopes so they stay in lockstep,
        // even though we don't use them in the audible signal yet (step 3).
        const float a = oscA.next() * envA.getNextSample() * lvlA;
        const float b = oscB.next() * envB.getNextSample() * lvlB;
        juce::ignoreUnused (a, b);
        // TODO (step 3): const float m = dsp::combine (algorithm, a, b, algoState);

        // Audible signal: just the carrier, enveloped + velocity-scaled.
        const float y = oscC.next() * lvlC * ampEnv * velocity * gain;

        for (int ch = 0; ch < numCh; ++ch)
            outBuffer.addSample (ch, startSample + i, y);

        if (! envAmp.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}

} // namespace bedridden
