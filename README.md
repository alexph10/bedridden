# Bedridden

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
cmake --build build --target Bedridden_Standalone
```

The standalone app launches without a host — fastest way to iterate.

## Layout

| Path                  | Purpose                                |
| --------------------- | -------------------------------------- |
| `src/PluginProcessor` | `AudioProcessor`, APVTS parameters     |
| `src/PluginEditor`    | Top-level GUI                          |
| `src/dsp/Oscillator`  | Phase-accumulator oscillators          |
| `src/dsp/Algorithm`   | 12 combine algorithms (RM/FM/PM/XOR…)  |
| `src/dsp/BBVoice`     | `SynthesiserVoice`: 3 osc + 2 ADSR     |
| `src/gui/OscPanel`    | Reusable A / B / C control column      |
| `tests/`              | CTest unit tests for the DSP           |

## Status

🚧 Project scaffolding — not yet implemented.

## License

MIT — see [LICENSE](LICENSE).
