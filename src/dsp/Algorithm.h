#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace bedridden::dsp {

/// The whole personality of the synth lives here: 12 ways to smash Osc A
/// and Osc B together into a single value that drives Osc C.
/// Reordering this list is a breaking change — the index is what gets
/// stored in the preset/state and what the GUI dropdown maps to.
enum class Algorithm : int
{
    Ring = 0,   // M = A * B                          bell-y, metallic
    Add,        // M = (A + B) * 0.5                  beating, chorus-y
    Sub,        // M = A - B                          phasey
    AbsMul,     // M = |A| * sign(B)                  half-wave grit
    Min,        // M = min(A, B)                      folded
    Max,        // M = max(A, B)                      folded the other way
    Xor,        // bitwise XOR on the quantised ints  digital crunch
    And,        // bitwise AND                        glitchy and lo-fi
    SampleHold, // grab A every time B crosses zero   aliased noise
    PhaseMod,   // M = sin(A + B)                     DX-style FM feel
    HardSync,   // sgn(A) flips B                     sync-ish
    Threshold,  // M = (A > B) ? +1 : -1              square/PWM hybrid
    Count
};

namespace detail {
    // Bitwise ops on floats don't really mean anything, so we squash the
    // signal into int16 range first and then untangle it on the way out.
    inline int32_t quantise (float x) noexcept
    {
        x = std::clamp (x, -1.f, 1.f);
        return (int32_t) std::lrintf (x * 32767.f);
    }

    inline float dequantise (int32_t q) noexcept
    {
        return (float) q / 32767.f;
    }
}

/// Per-voice scratch space for the algorithms that need to remember
/// something between samples — S&H needs the last latched value, hard
/// sync needs B's previous sign, etc.
struct AlgorithmState
{
    float  sampleHeld = 0.f;
    float  prevB      = 0.f;
};

/// Combine A and B into a modulator value M ∈ [-1, 1] (mostly).
inline float combine (Algorithm a, float A, float B, AlgorithmState& st) noexcept
{
    using detail::quantise;
    using detail::dequantise;

    switch (a)
    {
        case Algorithm::Ring:    return A * B;
        case Algorithm::Add:     return (A + B) * 0.5f;
        case Algorithm::Sub:     return A - B;
        case Algorithm::AbsMul:  return std::abs (A) * (B >= 0.f ? 1.f : -1.f);
        case Algorithm::Min:     return std::min (A, B);
        case Algorithm::Max:     return std::max (A, B);
        case Algorithm::Xor:     return dequantise (quantise (A) ^ quantise (B));
        case Algorithm::And:     return dequantise (quantise (A) & quantise (B));
        case Algorithm::SampleHold:
        {
            // Whenever B swings from negative to positive, freeze A.
            // The held value is what we hand back until the next crossing.
            if (st.prevB <= 0.f && B > 0.f) st.sampleHeld = A;
            st.prevB = B;
            return st.sampleHeld;
        }
        case Algorithm::PhaseMod: return std::sin (A + B);
        case Algorithm::HardSync: return (A >= 0.f ? B : -B);
        case Algorithm::Threshold: return (A > B) ? 1.f : -1.f;
        case Algorithm::Count: break;
    }
    return 0.f;
}

} // namespace bedridden::dsp
