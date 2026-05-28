#pragma once

#include <cmath>
#include <cstdint>

namespace bedridden::dsp {

enum class Wave { Sine = 0, Saw, Square, Triangle, Noise };

/// Bare-bones phase-accumulator osc. No anti-aliasing, no oversampling —
/// the gnarly aliased character is half the appeal of a Blood Bucket-style
/// synth. If we ever want a clean mode, chowdsp_utils has a drop-in PolyBLEP.
class Oscillator
{
public:
    void prepare (double sampleRate) noexcept
    {
        sr = sampleRate;
        updateIncrement();
    }

    void setFrequency (double hz) noexcept
    {
        freq = hz;
        updateIncrement();
    }

    void setWave (Wave w) noexcept { wave = w; }

    void resetPhase (double p = 0.0) noexcept { phase = p; }

    /// One sample, no modulation.
    float next() noexcept
    {
        const auto y = shape (phase);
        advance();
        return y;
    }

    /// One sample, but the phase is nudged by `phaseMod` radians first.
    /// This is the workhorse for PM and "FM" (which is really PM).
    float nextPM (float phaseMod) noexcept
    {
        const auto p = wrap (phase + phaseMod * (1.0 / juce_two_pi));
        const auto y = shape (p);
        advance();
        return y;
    }

private:
    static constexpr double juce_two_pi = 6.283185307179586;

    void updateIncrement() noexcept
    {
        inc = (sr > 0.0) ? (freq / sr) : 0.0;
    }

    void advance() noexcept
    {
        phase += inc;
        phase = wrap (phase);
    }

    static double wrap (double p) noexcept
    {
        p -= std::floor (p);
        return p;
    }

    float shape (double p) noexcept
    {
        switch (wave)
        {
            case Wave::Sine:     return (float) std::sin (juce_two_pi * p);
            case Wave::Saw:      return (float) (2.0 * p - 1.0);
            case Wave::Square:   return p < 0.5 ? 1.f : -1.f;
            case Wave::Triangle: return (float) (4.0 * std::abs (p - 0.5) - 1.0);
            case Wave::Noise:    return noise();
        }
        return 0.f;
    }

    float noise() noexcept
    {
        // xorshift32 — three shifts and a few XORs. No allocations,
        // no syscalls, safe to call from the audio thread.
        rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
        return ((int32_t) rng) / (float) 0x7fffffff;
    }

    double sr     = 44100.0;
    double freq   = 440.0;
    double inc    = 0.0;
    double phase  = 0.0;
    Wave   wave   = Wave::Sine;
    uint32_t rng  = 0x12345678u;
};

} // namespace bedridden::dsp
