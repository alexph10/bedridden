#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace bedridden::gui {

/// Four vertical sliders for A/D/S/R. We instantiate this twice (once for
/// Env A, once for Env B); `prefix` is the APVTS id stem ("envA"/"envB").
class EnvelopePanel : public juce::Component
{
public:
    EnvelopePanel (juce::AudioProcessorValueTreeState& apvts,
                   juce::String prefix,    // e.g. "envA"
                   juce::String title);    // e.g. "[ ENV A ]"

    void paint   (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider a, d, s, r;
    juce::Label  la { {}, "A" }, ld { {}, "D" }, ls { {}, "S" }, lr { {}, "R" };

    using SAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SAtt> aAtt, dAtt, sAtt, rAtt;

    juce::String title;
};

} // namespace bedridden::gui
