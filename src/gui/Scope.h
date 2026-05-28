#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <atomic>

namespace bedridden {

/// One-writer, one-reader ring buffer. The audio thread pushes samples,
/// the GUI thread takes snapshots — no locks, just one atomic index. The
/// reader might occasionally see a sample that's a tick old, which doesn't
/// matter for a scope at 30 fps.
class ScopeBuffer
{
public:
    static constexpr int kSize = 1024;

    /// Called from the audio thread. Allocation-free.
    void push (float sample) noexcept
    {
        const int w = writeIdx.load (std::memory_order_relaxed);
        data[w] = sample;
        writeIdx.store ((w + 1) % kSize, std::memory_order_release);
    }

    /// Called from the GUI. Copies the most recent N samples into `dest`.
    void snapshot (float* dest, int n) const noexcept
    {
        const int w = writeIdx.load (std::memory_order_acquire);
        for (int i = 0; i < n; ++i)
            dest[i] = data[((w - n + i) + kSize * 2) % kSize];
    }

private:
    std::array<float, kSize> data {};
    std::atomic<int>         writeIdx { 0 };
};

namespace gui {

/// The little waveform in the bottom-right corner. Repaints itself 30x a
/// second by snapshotting the processor's ring buffer.
class Scope : public juce::Component, private juce::Timer
{
public:
    explicit Scope (const ScopeBuffer& src) : source (src)
    {
        startTimerHz (30);
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colour (0xff0a0a0a));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.f);

        constexpr int N = 256;
        float buf[N];
        source.snapshot (buf, N);

        juce::Path p;
        const auto b = getLocalBounds().toFloat().reduced (2.f);
        const float w = b.getWidth(), h = b.getHeight();
        const float cy = b.getCentreY();

        p.startNewSubPath (b.getX(), cy);
        for (int i = 0; i < N; ++i)
        {
            const float x = b.getX() + (i / (float) (N - 1)) * w;
            const float y = cy - juce::jlimit (-1.f, 1.f, buf[i]) * (h * 0.45f);
            p.lineTo (x, y);
        }
        g.setColour (juce::Colour (0xffe22d2d));
        g.strokePath (p, juce::PathStrokeType (1.0f));
    }

private:
    void timerCallback() override { repaint(); }

    const ScopeBuffer& source;
};

} // namespace gui
} // namespace bedridden
