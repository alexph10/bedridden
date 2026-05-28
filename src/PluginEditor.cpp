#include "PluginEditor.h"
#include "Presets.h"
#include "gui/LookAndFeel.h"

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
    using namespace gui::palette;

    g.fillAll (juce::Colour (bg));

    // Top nameplate band — a darker strip with a red hairline under it, the
    // way old SynthEdit synths used to slap a header on.
    auto band = getLocalBounds().removeFromTop (46);
    g.setColour (juce::Colour (panel));
    g.fillRect (band);
    g.setColour (juce::Colour (bloodDk));
    g.fillRect (band.removeFromBottom (1));

    // Wordmark.
    g.setColour (juce::Colour (blood));
    g.setFont (juce::Font (juce::FontOptions (24.0f, juce::Font::bold)));
    g.drawFittedText ("BEDRIDDEN", getLocalBounds().removeFromTop (30),
                      juce::Justification::centred, 1);

    // Tagline — straight quote from the Blood Bucket KVR page, because it's
    // the whole spiritual mission statement of this plugin.
    g.setColour (juce::Colour (grime));
    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::italic)));
    g.drawFittedText ("a pointless experiment in synthetic stupidity",
                      getLocalBounds().removeFromTop (46).removeFromBottom (14),
                      juce::Justification::centred, 1);

    // Small "v1" stamp in the top-left, just to make it feel like an artifact.
    g.setColour (juce::Colour (grime));
    g.setFont (juce::Font (juce::FontOptions (8.0f, juce::Font::bold)));
    g.drawText ("v1.0", getLocalBounds().removeFromTop (46).reduced (10, 16),
                juce::Justification::left, false);
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
