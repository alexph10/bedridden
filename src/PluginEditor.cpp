#include "PluginEditor.h"

namespace bedridden {

BedriddenEditor::BedriddenEditor (BedriddenProcessor& p)
    : juce::AudioProcessorEditor (&p), proc (p)
{
    // Blood-Bucket-tight: one screen, no tabs, no resize.
    setResizable (false, false);
    setSize (440, 300);
}

void BedriddenEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff141414));

    // brand strip
    g.setColour (juce::Colour (0xffe22d2d));
    g.setFont (juce::Font (juce::FontOptions (20.0f, juce::Font::bold)));
    g.drawFittedText ("bedridden", getLocalBounds().removeFromTop (28),
                      juce::Justification::centred, 1);

    g.setColour (juce::Colour (0xff808080));
    g.setFont (juce::Font (juce::FontOptions (10.0f)));
    g.drawFittedText ("a pointless experiment in synthetic stupidity",
                      getLocalBounds().removeFromTop (44).removeFromBottom (14),
                      juce::Justification::centred, 1);
}

void BedriddenEditor::resized()
{
    // TODO: lay out three OscPanels (A | B | C) + algorithm combo + envelopes.
}

} // namespace bedridden
