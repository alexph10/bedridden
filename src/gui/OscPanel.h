#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace bedridden::gui {

/// One oscillator's worth of controls (wave / coarse / fine / level)
/// stacked in a vertical strip. We use three of these side-by-side.
class OscPanel : public juce::Component
{
public:
    OscPanel (juce::AudioProcessorValueTreeState& apvts, juce::String prefix);

    void paint   (juce::Graphics&) override;
    void resized() override;

private:
    juce::ComboBox waveBox;
    juce::Slider   coarse, fine, level;

    using Attach   = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BoxAttach = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<Attach>    coarseAtt, fineAtt, levelAtt;
    std::unique_ptr<BoxAttach> waveAtt;

    juce::String title;
};

} // namespace bedridden::gui
