#pragma once

#include "PluginProcessor.h"
#include "gui/AlgorithmPanel.h"
#include "gui/EnvelopePanel.h"
#include "gui/LookAndFeel.h"
#include "gui/OscPanel.h"
#include "gui/Scope.h"

namespace bedridden {

class BedriddenEditor : public juce::AudioProcessorEditor
{
public:
    explicit BedriddenEditor (BedriddenProcessor&);
    ~BedriddenEditor() override;

    void paint   (juce::Graphics&) override;
    void resized() override;

private:
    BedriddenProcessor& proc;

    gui::BBLookAndFeel  lnf;

    gui::OscPanel       oscA, oscB, oscC;
    gui::AlgorithmPanel algo;
    gui::EnvelopePanel  envA, envB;
    gui::Scope          scope;

    juce::Slider        master;
    juce::Label         masterLbl { {}, "MASTER" };
    juce::ComboBox      presetBox;

    using SAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SAtt> masterAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BedriddenEditor)
};

} // namespace bedridden
