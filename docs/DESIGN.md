# Bedridden — Engine Design

Bedridden is a clone-in-spirit of **Blood Bucket** by *Insert Piz Here*
(thepiz.org, 2009). The point is not faithful 1:1 modelling — it's the same
*idea*: two oscillators are mashed together by a selectable algorithm and the
result modulates a third "carrier" oscillator.

## Signal flow

```
  Osc A ──▶ ADSR A ─┐
                    ├─▶ COMBINE ─▶ M ──▶ MOD TARGET ─▶ Osc C ──▶ amp env ──▶ out
  Osc B ──▶ ADSR B ─┘
```

- **Osc A / B / C** — phase-accumulator oscillators (Sine, Saw, Square, Tri, Noise).
  Each has coarse (semitones) and fine (cents). Only Osc C is key-tracked by
  default; A and B can be free-running for that classic metallic Blood Bucket
  vibe.
- **ADSR A / B** — gate the modulator amplitudes.
- **COMBINE** — see algorithm table below; pure function of `(A, B)` plus a
  small `AlgorithmState` for memory (S&H, sync).
- **MOD TARGET** — how `M` is applied to Osc C:
  - `PM` — phase modulation: `oscC.nextPM(M * depth * 2π)`
  - `FM` — frequency modulation: `oscC.setFrequency(f0 * (1 + depth*M))`
  - `RM` — ring: `oscC.next() * M`
  - `AM` — amplitude: `oscC.next() * (0.5 + 0.5*M*depth)`

## The 12 algorithms

| # | Name        | Formula                              | Character          |
|---|-------------|--------------------------------------|--------------------|
| 1 | Ring        | `A * B`                              | Bell, metallic     |
| 2 | Add         | `(A + B) * 0.5`                      | Beating, chorus-y  |
| 3 | Sub         | `A - B`                              | Phasey             |
| 4 | AbsMul      | `|A| * sign(B)`                      | Half-wave grit     |
| 5 | Min         | `min(A, B)`                          | Folded             |
| 6 | Max         | `max(A, B)`                          | Folded             |
| 7 | XOR         | bitwise XOR of quantised samples     | Digital crunch     |
| 8 | AND         | bitwise AND of quantised samples     | Glitchy lo-fi      |
| 9 | Sample&Hold | latch A on rising zero-X of B        | Aliased noise      |
| 10| PhaseMod    | `sin(A + B)`                         | DX-style FM        |
| 11| HardSync    | `sgn(A) * B`                         | Sync-ish lead      |
| 12| Threshold   | `(A > B) ? +1 : -1`                  | Square / PWM-ish   |

## Parameters (APVTS ids)

| Group      | IDs                                                   |
|------------|-------------------------------------------------------|
| Osc A      | `oscA_wave`, `oscA_coarse`, `oscA_fine`, `oscA_level` |
| Osc B      | `oscB_wave`, `oscB_coarse`, `oscB_fine`, `oscB_level` |
| Osc C      | `oscC_wave`, `oscC_coarse`, `oscC_fine`, `oscC_level` |
| Env A      | `envA_{attack,decay,sustain,release}`                 |
| Env B      | `envB_{attack,decay,sustain,release}`                 |
| Routing    | `algorithm`, `mod_target`, `mod_depth`, `out_gain`    |

## Roadmap

1. Wire `BBVoice::renderNextBlock` — read APVTS once per block, then loop.
2. Add a DC blocker + soft tanh on the output stage.
3. Build the GUI: three `OscPanel`s + algorithm `ComboBox` + envelope panels.
4. Polyphony (8 voices).
5. Presets bank.
6. Validate with `pluginval` strictness 8.
