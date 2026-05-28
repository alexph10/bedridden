#include "PluginEditor.h"
#include "Presets.h"

namespace bedridden {

BedriddenEditor::BedriddenEditor (BedriddenProcessor& p)
    : juce::AudioProcessorEditor (&p),
      proc  (p),
      oscA  (p.apvts, "oscA"),
      oscB  (p.apvts, "oscB"),
      oscC  (p.apvts, "oscC"),
      algo  (p.apvts),
      envA  (p.apvts, "envA", "[ ENV A ]"),
      envB  (p.apvts, "envB", "[ ENV B ]"),
      scope (p.scopeBuffer)
{
    setLookAndFeel (&lnf);

    addAndMakeVisible (oscA);
    addAndMakeVisible (oscB);
    addAndMakeVisible (oscC);
    addAndMakeVisible (algo);
    addAndMakeVisible (envA);
    addAndMakeVisible (envB);
    addAndMakeVisible (scope);

    master.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    master.setTextBoxStyle (juce::Slider::TextBoxRight, false, 40, 14);
    addAndMakeVisible (master);
    masterLbl.setJustificationType (juce::Justification::centredRight);
    masterLbl.setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
    addAndMakeVisible (masterLbl);
    masterAtt = std::make_unique<SAtt> (p.apvts, "out_gain", master);

    // ---- Preset dropdown ----
    const auto presets = getFactoryPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox.addItem (presets[i].name, (int) i + 1);
    presetBox.setSelectedId (1, juce::dontSendNotification);
    presetBox.onChange = [this, presets]
    {
        const int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0 && idx < (int) presets.size())
            applyPreset (proc.apvts, presets[idx]);
    };
    addAndMakeVisible (presetBox);

    setResizable (false, false);
    setSize (560, 360);
}

BedriddenEditor::~BedriddenEditor()
{
    setLookAndFeel (nullptr);
}

void BedriddenEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff141414));

    g.setColour (juce::Colour (0xffe22d2d));
    g.setFont (juce::Font (juce::FontOptions (22.0f, juce::Font::bold)));
    g.drawFittedText ("bedridden", getLocalBounds().removeFromTop (30),
                      juce::Justification::centred, 1);

    g.setColour (juce::Colour (0xff808080));
    g.setFont (juce::Font (juce::FontOptions (10.0f)));
    g.drawFittedText ("a pointless experiment in synthetic stupidity",
                      getLocalBounds().removeFromTop (46).removeFromBottom (14),
                      juce::Justification::centred, 1);
}

void BedriddenEditor::resized()
{
    // Carve up the editor top-down, then left-to-right within each strip.
    auto r = getLocalBounds();
    auto brand = r.removeFromTop (46);
    auto bottom = r.removeFromBottom (50);

    // Preset dropdown sits in the corner of the title bar.
    presetBox.setBounds (brand.removeFromRight (120).reduced (8, 12));

    r.removeFromTop (4);
    r.removeFromBottom (4);
    r.reduce (6, 0);

    // Top: three osc strips, then the algorithm panel takes whatever's left.
    auto top = r.removeFromTop (130);
    const int osw = 100;
    oscA.setBounds (top.removeFromLeft (osw).reduced (3));
    oscB.setBounds (top.removeFromLeft (osw).reduced (3));
    oscC.setBounds (top.removeFromLeft (osw).reduced (3));
    algo.setBounds (top.reduced (3));

    // Middle: two envelopes side by side, splitting the remaining row.
    r.removeFromTop (4);
    auto envRow = r.removeFromTop (110);
    const int ew = envRow.getWidth() / 2;
    envA.setBounds (envRow.removeFromLeft (ew).reduced (3));
    envB.setBounds (envRow.reduced (3));

    // Bottom strip: master knob on the left, scope on the right.
    bottom.reduce (6, 6);
    auto masterArea = bottom.removeFromLeft (180);
    masterLbl.setBounds (masterArea.removeFromLeft (60));
    master   .setBounds (masterArea);
    scope    .setBounds (bottom);
}

} // namespace bedridden
