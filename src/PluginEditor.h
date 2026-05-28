#pragma once

#include "PluginProcessor.h"

namespace bedridden {

class BedriddenEditor : public juce::AudioProcessorEditor
{
public:
    explicit BedriddenEditor (BedriddenProcessor&);
    ~BedriddenEditor() override = default;

    void paint   (juce::Graphics&) override;
    void resized() override;

private:
    BedriddenProcessor& proc;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BedriddenEditor)
};

} // namespace bedridden
