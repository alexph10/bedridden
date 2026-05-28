// Tiny hand-rolled test runner — no external test framework needed yet.
// Run via `ctest` after building, or just execute the binary directly.

#include "dsp/Algorithm.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace bedridden::dsp;

static int failures = 0;

#define EXPECT_NEAR(a, b, eps) do {                                           \
    const float _a = (a), _b = (b);                                           \
    if (std::fabs (_a - _b) > (eps)) {                                        \
        std::printf ("FAIL %s:%d  %s ≈ %s   (%.6f vs %.6f)\n",                \
                     __FILE__, __LINE__, #a, #b, _a, _b);                     \
        ++failures;                                                           \
    }                                                                         \
} while (0)

int main()
{
    AlgorithmState st {};

    EXPECT_NEAR (combine (Algorithm::Ring,  0.5f,  0.5f, st),  0.25f, 1e-6f);
    EXPECT_NEAR (combine (Algorithm::Add,   0.5f, -0.5f, st),  0.0f,  1e-6f);
    EXPECT_NEAR (combine (Algorithm::Sub,   0.5f,  0.25f, st), 0.25f, 1e-6f);
    EXPECT_NEAR (combine (Algorithm::Min,  -0.2f,  0.8f, st), -0.2f,  1e-6f);
    EXPECT_NEAR (combine (Algorithm::Max,  -0.2f,  0.8f, st),  0.8f,  1e-6f);
    EXPECT_NEAR (combine (Algorithm::Threshold, 0.6f, 0.4f, st), 1.f, 1e-6f);
    EXPECT_NEAR (combine (Algorithm::Threshold, 0.1f, 0.4f, st), -1.f, 1e-6f);

    // Sample & Hold: latches A on rising zero-crossing of B.
    st = {};
    combine (Algorithm::SampleHold, 0.42f, -0.1f, st); // arm
    const float h = combine (Algorithm::SampleHold, 0.42f, 0.1f, st); // trigger
    EXPECT_NEAR (h, 0.42f, 1e-6f);

    std::printf (failures == 0 ? "ALL TESTS PASSED\n"
                               : "%d FAILURE(S)\n", failures);
    return failures == 0 ? 0 : 1;
}
