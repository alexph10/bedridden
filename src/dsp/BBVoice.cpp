#include "BBVoice.h"
#include "BBSound.h"

#include <cmath>

namespace bedridden {

namespace {
    // Keep this in lockstep with the "wave" choice list in
    // PluginProcessor::createLayout() and the Wave enum in Oscillator.h.
    // If you reorder one, you have to reorder all three.
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

    pAlgo      = get("algorithm");
    pModTarget = get("mod_target");
    pModDepth  = get("mod_depth");
    pOutGain   = get("out_gain");
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

    algoState  = {};
    dcPrevIn   = 0.0f;
    dcPrevOut  = 0.0f;
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
    oscA.setWave (waveFromIndex ((int) pA.wave->load()));
    oscB.setWave (waveFromIndex ((int) pB.wave->load()));
    oscC.setWave (waveFromIndex ((int) pC.wave->load()));

    // All three oscs key-track for now. Letting A/B float free (like the
    // original Blood Bucket) would give us that metallic, atonal vibe —
    // worth exposing as a per-osc "track" toggle later.
    const float fA = noteFreq * pitchMultiplier (pA.coarse->load(), pA.fine->load());
    const float fB = noteFreq * pitchMultiplier (pB.coarse->load(), pB.fine->load());
    const float fC = noteFreq * pitchMultiplier (pC.coarse->load(), pC.fine->load());
    oscA.setFrequency (fA);
    oscB.setFrequency (fB);
    oscC.setFrequency (fC);

    juce::ADSR::Parameters a {
        eA.attack->load(), eA.decay->load(), eA.sustain->load(), eA.release->load() };
    juce::ADSR::Parameters b {
        eB.attack->load(), eB.decay->load(), eB.sustain->load(), eB.release->load() };

    envA.setParameters (a);
    envB.setParameters (b);
    // The amp envelope piggy-backs on Env A — Blood Bucket has no dedicated
    // VCA env either. If we ever want one, this is the line to change.
    envAmp.setParameters (a);
}

void BBVoice::renderNextBlock (juce::AudioBuffer<float>& outBuffer,
                               int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    // Re-read the knobs once per block. Per-sample reads would be wasteful
    // and per-note reads would feel laggy when the user is automating.
    refreshFromParams();

    const float lvlA  = pA.level->load();
    const float lvlB  = pB.level->load();
    const float lvlC  = pC.level->load();
    const float depth = pModDepth ? pModDepth->load() : 0.5f;
    const float gain  = pOutGain  ? pOutGain->load()  : 0.7f;

    const auto algo   = static_cast<dsp::Algorithm> (pAlgo
                          ? juce::jlimit (0, (int) dsp::Algorithm::Count - 1,
                                          (int) pAlgo->load())
                          : 0);
    // 0=PM, 1=FM, 2=RM, 3=AM — order matches the dropdown in createLayout().
    const int  target = pModTarget ? (int) pModTarget->load() : 0;

    const int numCh = outBuffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        const float ampEnv = envAmp.getNextSample();

        // Mix the two modulator oscs through the chosen combine function
        // — this is the actual "Blood Bucket" personality knob.
        const float a = oscA.next() * envA.getNextSample() * lvlA;
        const float b = oscB.next() * envB.getNextSample() * lvlB;
        const float m = dsp::combine (algo, a, b, algoState);

        // Then send the modulator into the carrier the way the user asked.
        float c;
        switch (target)
        {
            case 0: // PM: nudge the carrier's phase by m*depth*π radians.
                c = oscC.nextPM (m * depth * 3.14159265f);
                break;
            case 1: // FM: every "FM synth" since the DX7 is really PM —
                    // here we just use twice the index range.
                c = oscC.nextPM (m * depth * 6.28318530f);
                break;
            case 2: // RM: ring mod, with depth as a dry↔ring crossfade.
            {
                const float dry = oscC.next();
                c = dry * (1.0f - depth) + (dry * m) * depth;
                break;
            }
            case 3: // AM: unipolar tremolo.
            default:
            {
                const float dry  = oscC.next();
                const float mod  = 0.5f * (m + 1.0f);     // [-1,1] → [0,1]
                c = dry * (1.0f - depth + depth * mod);
                break;
            }
        }

        float y = c * lvlC * ampEnv * velocity * gain;

        // Strip DC. The bitwise algorithms (XOR/AND) and Threshold will
        // happily push the signal off-centre and pop the speakers if we
        // don't catch them.
        constexpr float R = 0.995f;
        const float yDC = y - dcPrevIn + R * dcPrevOut;
        dcPrevIn  = y;
        dcPrevOut = yDC;
        y = yDC;

        // Polite ceiling so loud algorithm + depth combinations don't
        // blow the user's ears off.
        y = std::tanh (y);

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
