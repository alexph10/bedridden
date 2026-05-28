#include "LookAndFeel.h"

namespace bedridden::gui {

BBLookAndFeel::BBLookAndFeel()
{
    setColour (juce::Slider::rotarySliderFillColourId,    juce::Colours::red);
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff202020));
    setColour (juce::Slider::thumbColourId,               juce::Colours::white);
    setColour (juce::ComboBox::backgroundColourId,        juce::Colour (0xff1a1a1a));
    setColour (juce::Label::textColourId,                 juce::Colours::lightgrey);
}

} // namespace bedridden::gui
