# bedridden

A free, weird 3-oscillator VST3/AU instrument plugin inspired by
**Blood Bucket** by *Insert Piz Here*.

> "A pointless experiment in synthetic stupidity."
> Two oscillators are combined in a variety of ways to modulate a third.

## Engine in one picture

```
  Osc A ──▶ ADSR A ─┐
                    ├─▶ COMBINE (RM / FM / PM / XOR / S&H / ...) ─▶ Osc C ─▶ out
  Osc B ──▶ ADSR B ─┘
```

See [docs/DESIGN.md](docs/DESIGN.md) for the full algorithm table and signal flow.

## Tech

- **Framework:** JUCE 8 (fetched via CMake `FetchContent`)
- **Build:** CMake ≥ 3.22, C++20
- **Formats:** VST3, AU, Standalone
- **Platforms:** Windows, macOS, Linux

## Build

```bash
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

This produces three artefacts under `build/`:

| Artefact                                    | What it is                                              |
| ------------------------------------------- | ------------------------------------------------------- |
| `Bedridden_artefacts/Release/VST3/Bedridden.vst3` | The plugin (on Windows this *is* a DLL with a `.vst3` extension) |
| `Bedridden_artefacts/Release/Standalone/Bedridden.exe` | Host-less standalone for fast iteration               |
| `bin/BedriddenDSP.dll` (+ `lib/BedriddenDSP.lib`) | Reusable engine with a pure C ABI (see `dsplib/BedriddenDSP.h`) |

CMake options (all default `ON`):
`-DBEDRIDDEN_BUILD_PLUGIN=ON  -DBEDRIDDEN_BUILD_DLL=ON  -DBEDRIDDEN_BUILD_TESTS=ON`

## Layout

| Path                       | Purpose                                              |
| -------------------------- | ---------------------------------------------------- |
| `src/PluginProcessor`      | `AudioProcessor`, APVTS parameters                   |
| `src/PluginEditor`         | Top-level GUI                                        |
| `src/dsp/Oscillator`       | Phase-accumulator oscillators                        |
| `src/dsp/Algorithm`        | 12 combine algorithms (RM/FM/PM/XOR…)                |
| `src/dsp/BBVoice`          | `SynthesiserVoice`: 3 osc + 2 ADSR                   |
| `src/gui/OscPanel`         | Reusable A / B / C control column                    |
| `dsplib/BedriddenDSP.{h,cpp}` | Plain-C ABI wrapper → `BedriddenDSP.dll`          |
| `tests/`                   | CTest unit tests for the DSP                         |
| `docs/DESIGN.md`           | Engine design notes                                  |
| `docs/UI.md`               | Visual interface intuition                           |

## Status

🚧 Project scaffolding — not yet implemented.

## License

MIT — see [LICENSE](LICENSE).
