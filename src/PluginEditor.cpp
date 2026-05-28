#include "PluginEditor.h"

namespace bedridden {

BedriddenEditor::BedriddenEditor (BedriddenProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p)
{
    setSize (720, 420);
}

void BedriddenEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff141414));
    g.setColour (juce::Colours::red.withAlpha (0.85f));
    g.setFont (juce::Font (juce::FontOptions (28.0f, juce::Font::bold)));
    g.drawFittedText ("BEDRIDDEN", getLocalBounds().removeFromTop (50),
                      juce::Justification::centred, 1);

    g.setColour (juce::Colours::grey);
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawFittedText ("a pointless experiment in synthetic stupidity",
                      getLocalBounds().removeFromTop (80).removeFromBottom (20),
                      juce::Justification::centred, 1);
}

void BedriddenEditor::resized()
{
    // TODO: lay out three OscPanels (A | B | C) + algorithm combo + envelopes.
}

} // namespace bedridden
