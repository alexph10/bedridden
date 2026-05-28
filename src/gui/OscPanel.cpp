#include "OscPanel.h"

namespace bedridden::gui {

OscPanel::OscPanel (juce::AudioProcessorValueTreeState& apvts, juce::String prefix)
    : title (prefix)
{
    waveBox.addItemList ({ "Sine", "Saw", "Square", "Triangle", "Noise" }, 1);
    addAndMakeVisible (waveBox);

    for (auto* s : { &coarse, &fine, &level })
    {
        s->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
        addAndMakeVisible (*s);
    }

    waveAtt   = std::make_unique<BoxAttach> (apvts, prefix + "_wave",   waveBox);
    coarseAtt = std::make_unique<Attach>    (apvts, prefix + "_coarse", coarse);
    fineAtt   = std::make_unique<Attach>    (apvts, prefix + "_fine",   fine);
    levelAtt  = std::make_unique<Attach>    (apvts, prefix + "_level",  level);
}

void OscPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff1a1a1a));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 6.f);
    g.setColour (juce::Colours::lightgrey);
    g.drawFittedText (title, getLocalBounds().removeFromTop (20),
                      juce::Justification::centred, 1);
}

void OscPanel::resized()
{
    auto r = getLocalBounds().reduced (8);
    r.removeFromTop (20);
    waveBox.setBounds (r.removeFromTop (24));
    r.removeFromTop (6);
    const int h = r.getHeight() / 3;
    coarse.setBounds (r.removeFromTop (h));
    fine  .setBounds (r.removeFromTop (h));
    level .setBounds (r);
}

} // namespace bedridden::gui
