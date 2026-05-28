#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace bedridden::dsp {

/// how Osc A and Osc B are combined into a
/// single modulator that drives Osc C.
enum class Algorithm : int
{
    Ring = 0,   // M = A * B                — bell / metallic
    Add,        // M = (A + B) * 0.5        — beating
    Sub,        // M = A - B                — phasey
    AbsMul,     // M = |A| * sign(B)        — half-wave grit
    Min,        // M = min(A, B)            — folded
    Max,        // M = max(A, B)            — folded
    Xor,        // bitwise XOR on quantised samples — digital crunch
    And,        // bitwise AND              — glitchy lo-fi
    SampleHold, // sample A whenever B crosses zero — aliased noise
    PhaseMod,   // M = sin(A + B)           — DX-style FM feel
    HardSync,   // sgn(A) gates B (sync-ish)
    Threshold,  // M = (A > B) ? +1 : -1
    Count
};

namespace detail {
    inline int32_t quantise (float x) noexcept
    {
        // Map [-1, 1] -> int16 range for bitwise ops.
        x = std::clamp (x, -1.f, 1.f);
        return (int32_t) std::lrintf (x * 32767.f);
    }

    inline float dequantise (int32_t q) noexcept
    {
        return (float) q / 32767.f;
    }
}

/// Stateful per-voice context for algorithms that need memory (e.g. S&H).
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
            // Latch A on rising zero-crossing of B.
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
