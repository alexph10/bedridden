# bedridden — Visual Interface Intuition

We're cloning the *vibe* of Blood Bucket's UI, not the pixels. This document
captures why Blood Bucket's panel works and the rules we'll follow.

## What Blood Bucket's panel actually communicates

Blood Bucket's window is small (~400 × 280 px), one screen, no tabs, no
menus. We match that: **bedridden is 440 × 300 px, non-resizable**. Looking at it, you instantly understand:

1. There are **three oscillators**, laid out as **three vertical lanes**.
2. There's **one algorithm choice** that connects them — and it sits in
   the middle, because it's the personality of the synth.
3. There are **two envelopes**, one per modulator (A and B), nothing for C.
4. Nothing else exists. No filter, no LFO, no effects. The constraint *is*
   the instrument.

This is the entire intuition. If a user can read the signal flow off the
panel in under three seconds, the UI succeeded.

## Layout rules we'll follow

```
 ╭────────────────── bedridden ──────────────────╮
 │  a pointless experiment in synthetic stupidity │
 ├─────────┬──────────┬─────────┬─────────────────┤
 │[ OSC A ]│[ OSC B ] │[ OSC C ]│   [ ALGO ]      │
 │ wave ▾  │ wave  ▾  │ wave  ▾ │  Ring     ▾     │
 │ crse ◯  │ crse  ◯  │ crse  ◯ │  depth    ◯     │
 │ fine ◯  │ fine  ◯  │ fine  ◯ │  target   ▾     │
 │ lvl  ◯  │ lvl   ◯  │ ── ── ─ │                 │
 ├─────────┴──────────┼─────────┴─────────────────┤
 │ [ ENV A ] A D S R  │ [ ENV B ] A D S R         │
 │           │ │ │ │  │           │ │ │ │         │
 │           ▽ ▽ ▽ ▽  │           ▽ ▽ ▽ ▽         │
 ├────────────────────┴───────────────────────────┤
 │ master ◯                  [tiny scope ░░░░]    │
 ╰────────────────────────────────────────────────╯
```

### 1. Three lanes mirror the signal path

Left → right reads as Osc A → Combine → Osc C, with Osc B + Algorithm in
the middle. The user's eye traces the audio path.

### 2. One accent colour
Pure dark grey background (`#141414`) with a single saturated red
(`#e22d2d`) accent on active controls, value indicators, and the brand
strip. Everything else is one of three greys. No gradients, no shadows.

### 3. Sliders vs knobs
- **Knobs** for tuning and depth (continuous, no precision needed).
- **Vertical sliders** for ADSR — the parallel A/D/S/R layout makes the
  envelope shape readable at a glance.

### 4. Algorithm is the hero
The middle column is **wider** than the others and has a thin red border.
It's the only control that justifies its own panel.

### 5. Chunky bracketed labels
`[ OSC A ]`, `[ ALGO ]`, `[ ENV B ]` — monospaced, uppercase, square
brackets. It's the 2009 freeware-VST house style and it suits the
"stupid experiment" tone.

### 6. Tiny scope (optional, our addition)
Blood Bucket has no visual feedback whatsoever. We add a 120×40 px
oscilloscope in the bottom-right strip. It's the smallest possible
concession to modern UX and lets the user *see* what the algorithm does
when they sweep depth.

### 7. Fixed window size
**440 × 300 px**, non-resizable. Same footprint as Blood Bucket. Saves us
from layout headaches and matches the all-on-one-screen ethos.

## Palette

| Role             | Hex       | Where                         |
| ---------------- | --------- | ----------------------------- |
| Background       | `#141414` | window fill                   |
| Panel            | `#1a1a1a` | osc / env panels              |
| Border           | `#202020` | panel outlines                |
| Text dim         | `#808080` | secondary labels              |
| Text bright      | `#d0d0d0` | values, primary labels        |
| Accent           | `#e22d2d` | active control, brand, scope  |

(These are already set in [`src/gui/LookAndFeel.cpp`](../src/gui/LookAndFeel.cpp).)

## Typography

- **Brand:** bold 28 pt sans (system default; matches JUCE defaults).
- **Section labels:** mono 11 pt uppercase, e.g. `[ OSC A ]`.
- **Values:** mono 10 pt.

That's it. The whole UI is built from rectangles, three font sizes, and
one accent colour.

## Implementation order

1. `BBLookAndFeel` — colours done; add font metrics + slider drawing.
2. `OscPanel` — already a column; refine paint to draw the bracketed
   header and panel rectangle.
3. `AlgorithmPanel` — new component, wider column with algorithm picker,
   `mod_depth`, `mod_target`.
4. `EnvelopePanel` — two of these, A and B, with 4 vertical sliders.
5. `Scope` — `juce::Timer` + 30 fps redraw on a small ring buffer.
6. `BedriddenEditor::resized()` — wire the layout to the diagram above.
