/*  BedriddenDSP — pure-C ABI for the Bedridden engine.
 *
 *  This header is intentionally C-only so the resulting BedriddenDSP.dll can
 *  be consumed from C, C++, Python (ctypes), Rust (libc), etc. without
 *  pulling in JUCE.
 */

#ifndef BEDRIDDEN_DSP_H
#define BEDRIDDEN_DSP_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
  #if defined(BEDRIDDEN_DSP_BUILD)
    #define BB_API __declspec(dllexport)
  #else
    #define BB_API __declspec(dllimport)
  #endif
#else
  #define BB_API __attribute__((visibility("default")))
#endif

/* Must match bedridden::dsp::Algorithm in Algorithm.h */
typedef enum {
    BB_ALG_RING       = 0,
    BB_ALG_ADD        = 1,
    BB_ALG_SUB        = 2,
    BB_ALG_ABSMUL     = 3,
    BB_ALG_MIN        = 4,
    BB_ALG_MAX        = 5,
    BB_ALG_XOR        = 6,
    BB_ALG_AND        = 7,
    BB_ALG_SAMPLEHOLD = 8,
    BB_ALG_PHASEMOD   = 9,
    BB_ALG_HARDSYNC   = 10,
    BB_ALG_THRESHOLD  = 11
} BBAlgorithm;

typedef struct {
    float sample_held;
    float prev_b;
} BBAlgorithmState;

/* Library version (semver-packed: major<<16 | minor<<8 | patch). */
BB_API unsigned int bb_version(void);

/* Combine A and B with the given algorithm. */
BB_API float bb_combine(BBAlgorithm alg, float a, float b, BBAlgorithmState* state);

/* Convenience: process a buffer of (A, B) pairs into a modulator buffer. */
BB_API void bb_combine_buffer(BBAlgorithm alg,
                              const float* a, const float* b,
                              float* out, int num_samples,
                              BBAlgorithmState* state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BEDRIDDEN_DSP_H */
