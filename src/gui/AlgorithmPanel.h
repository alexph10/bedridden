#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace bedridden::gui {

/// The widest column in the editor, on purpose — this panel decides what
/// the synth actually sounds like. Algorithm choice + mod target + depth.
class AlgorithmPanel : public juce::Component
{
public:
    explicit AlgorithmPanel (juce::AudioProcessorValueTreeState& apvts);

    void paint   (juce::Graphics&) override;
    void resized() override;

private:
    juce::ComboBox algoBox, targetBox;
    juce::Slider   depth;
    juce::Label    algoLbl { {}, "ALG" }, targetLbl { {}, "TGT" }, depthLbl { {}, "DEPTH" };

    using SAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CAtt = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<CAtt> algoAtt, targetAtt;
    std::unique_ptr<SAtt> depthAtt;
};

} // namespace bedridden::gui
