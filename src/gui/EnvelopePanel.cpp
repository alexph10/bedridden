#include "EnvelopePanel.h"

namespace bedridden::gui {

EnvelopePanel::EnvelopePanel (juce::AudioProcessorValueTreeState& apvts,
                              juce::String prefix,
                              juce::String t)
    : title (std::move (t))
{
    auto setup = [&] (juce::Slider& sl)
    {
        sl.setSliderStyle (juce::Slider::LinearVertical);
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (sl);
    };
    setup (a); setup (d); setup (s); setup (r);

    for (auto* l : { &la, &ld, &ls, &lr })
    {
        l->setJustificationType (juce::Justification::centred);
        l->setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
        addAndMakeVisible (*l);
    }

    aAtt = std::make_unique<SAtt> (apvts, prefix + "_attack",  a);
    dAtt = std::make_unique<SAtt> (apvts, prefix + "_decay",   d);
    sAtt = std::make_unique<SAtt> (apvts, prefix + "_sustain", s);
    rAtt = std::make_unique<SAtt> (apvts, prefix + "_release", r);
}

void EnvelopePanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff1a1a1a));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 4.f);

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    g.drawFittedText (title, getLocalBounds().removeFromTop (16),
                      juce::Justification::centred, 1);
}

void EnvelopePanel::resized()
{
    auto r = getLocalBounds().reduced (6);
    r.removeFromTop (14);

    auto labelStrip = r.removeFromBottom (12);
    const int w = r.getWidth() / 4;

    juce::Rectangle<int> aR = r.removeFromLeft (w);
    juce::Rectangle<int> dR = r.removeFromLeft (w);
    juce::Rectangle<int> sR = r.removeFromLeft (w);
    juce::Rectangle<int> rR = r;
    a.setBounds (aR);
    d.setBounds (dR);
    s.setBounds (sR);
    this->r.setBounds (rR);

    juce::Rectangle<int> laR = labelStrip.removeFromLeft (w);
    juce::Rectangle<int> ldR = labelStrip.removeFromLeft (w);
    juce::Rectangle<int> lsR = labelStrip.removeFromLeft (w);
    juce::Rectangle<int> lrR = labelStrip;
    la.setBounds (laR);
    ld.setBounds (ldR);
    ls.setBounds (lsR);
    lr.setBounds (lrR);
}

} // namespace bedridden::gui
