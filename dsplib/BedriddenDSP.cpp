#define BEDRIDDEN_DSP_BUILD
#include "BedriddenDSP.h"
#include "dsp/Algorithm.h"

using namespace bedridden::dsp;

static inline Algorithm to_cpp(BBAlgorithm a) noexcept
{
    return static_cast<Algorithm>(static_cast<int>(a));
}

static inline AlgorithmState& to_cpp(BBAlgorithmState* s) noexcept
{
    // Layout-compatible by design (two floats, same order).
    return *reinterpret_cast<AlgorithmState*>(s);
}

extern "C" {

BB_API unsigned int bb_version(void)
{
    return (0u << 16) | (1u << 8) | 0u; // 0.1.0
}

BB_API float bb_combine(BBAlgorithm alg, float a, float b, BBAlgorithmState* state)
{
    AlgorithmState local{};
    AlgorithmState& s = state ? to_cpp(state) : local;
    return combine(to_cpp(alg), a, b, s);
}

BB_API void bb_combine_buffer(BBAlgorithm alg,
                              const float* a, const float* b,
                              float* out, int num_samples,
                              BBAlgorithmState* state)
{
    if (!a || !b || !out || num_samples <= 0) return;
    AlgorithmState local{};
    AlgorithmState& s = state ? to_cpp(state) : local;
    const auto cpp_alg = to_cpp(alg);
    for (int i = 0; i < num_samples; ++i)
        out[i] = combine(cpp_alg, a[i], b[i], s);
}

} // extern "C"
