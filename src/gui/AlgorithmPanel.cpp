#include "AlgorithmPanel.h"

namespace bedridden::gui {

AlgorithmPanel::AlgorithmPanel (juce::AudioProcessorValueTreeState& apvts)
{
    // These strings have to line up exactly with createLayout()'s "algos"
    // list and the Algorithm enum in dsp/Algorithm.h — three places, same
    // order. Worth a comment because it's easy to drift.
    algoBox.addItemList ({ "Ring", "Add", "Sub", "AbsMul", "Min", "Max",
                           "XOR", "AND", "S&H", "PhaseMod",
                           "HardSync", "Threshold" }, 1);
    targetBox.addItemList ({ "PM", "FM", "RM", "AM" }, 1);

    addAndMakeVisible (algoBox);
    addAndMakeVisible (targetBox);

    depth.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    depth.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 14);
    addAndMakeVisible (depth);

    for (auto* l : { &algoLbl, &targetLbl, &depthLbl })
    {
        l->setJustificationType (juce::Justification::centred);
        l->setFont (juce::Font (juce::FontOptions (9.0f)));
        addAndMakeVisible (*l);
    }

    algoAtt   = std::make_unique<CAtt> (apvts, "algorithm",  algoBox);
    targetAtt = std::make_unique<CAtt> (apvts, "mod_target", targetBox);
    depthAtt  = std::make_unique<SAtt> (apvts, "mod_depth",  depth);
}

void AlgorithmPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff1a1a1a));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 4.f);
    g.setColour (juce::Colour (0xffe22d2d));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (0.5f), 4.f, 1.f);

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    g.drawFittedText ("[ ALGO ]", getLocalBounds().removeFromTop (16),
                      juce::Justification::centred, 1);
}

void AlgorithmPanel::resized()
{
    auto r = getLocalBounds().reduced (6);
    r.removeFromTop (14);

    algoLbl  .setBounds (r.removeFromTop (10));
    algoBox  .setBounds (r.removeFromTop (20));
    r.removeFromTop (4);
    targetLbl.setBounds (r.removeFromTop (10));
    targetBox.setBounds (r.removeFromTop (20));
    r.removeFromTop (4);
    depthLbl .setBounds (r.removeFromTop (10));
    depth    .setBounds (r);
}

} // namespace bedridden::gui
