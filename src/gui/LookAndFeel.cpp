#include "LookAndFeel.h"

namespace bedridden::gui {

BBLookAndFeel::BBLookAndFeel()
{
    setColour (juce::Slider::rotarySliderFillColourId,    juce::Colour (palette::blood));
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (palette::hairline));
    setColour (juce::Slider::thumbColourId,               juce::Colour (palette::ink));
    setColour (juce::Slider::backgroundColourId,          juce::Colour (palette::panelHi));
    setColour (juce::Slider::trackColourId,               juce::Colour (palette::blood));
    setColour (juce::Slider::textBoxTextColourId,         juce::Colour (palette::ink));
    setColour (juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId,   juce::Colours::transparentBlack);

    setColour (juce::ComboBox::backgroundColourId,        juce::Colour (palette::panelHi));
    setColour (juce::ComboBox::outlineColourId,           juce::Colour (palette::bloodDk));
    setColour (juce::ComboBox::textColourId,              juce::Colour (palette::ink));
    setColour (juce::ComboBox::arrowColourId,             juce::Colour (palette::blood));
    setColour (juce::ComboBox::buttonColourId,            juce::Colour (palette::panel));

    setColour (juce::PopupMenu::backgroundColourId,       juce::Colour (palette::panel));
    setColour (juce::PopupMenu::textColourId,             juce::Colour (palette::ink));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (palette::bloodDk));
    setColour (juce::PopupMenu::highlightedTextColourId,  juce::Colour (palette::ink));

    setColour (juce::Label::textColourId,                 juce::Colour (palette::grime));
}

// The signature Blood Bucket knob: a flat white square with one short red
// tick mark drawn from the centre, pointing at the current value. No
// gradients, no anti-aliased rings, no skeuomorphism — just a square.
void BBLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                      float sliderPos, float startA, float endA,
                                      juce::Slider&)
{
    const auto bounds = juce::Rectangle<float> ((float) x, (float) y,
                                                (float) w, (float) h).reduced (4.f);
    const float side  = juce::jmin (bounds.getWidth(), bounds.getHeight());
    const auto  sq    = juce::Rectangle<float> (0, 0, side, side)
                            .withCentre (bounds.getCentre());

    // Recessed socket behind the knob — gives the square something to sit in.
    g.setColour (juce::Colour (palette::panelHi));
    g.fillRect (sq.expanded (2.f));
    g.setColour (juce::Colour (palette::hairline));
    g.drawRect (sq.expanded (2.f), 1.f);

    // The square "cap".
    g.setColour (juce::Colour (palette::ink));
    g.fillRect (sq);
    g.setColour (juce::Colour (palette::bloodDk));
    g.drawRect (sq, 1.f);

    // The single red direction tick. From centre out to ~80% of the radius.
    const float angle  = startA + sliderPos * (endA - startA);
    const auto  centre = sq.getCentre();
    const float r0     = side * 0.18f;
    const float r1     = side * 0.46f;

    juce::Path tick;
    tick.startNewSubPath (centre.x + std::sin (angle) * r0,
                          centre.y - std::cos (angle) * r0);
    tick.lineTo          (centre.x + std::sin (angle) * r1,
                          centre.y - std::cos (angle) * r1);

    g.setColour (juce::Colour (palette::blood));
    g.strokePath (tick, juce::PathStrokeType (2.4f, juce::PathStrokeType::mitered,
                                                    juce::PathStrokeType::square));
}

// Envelope sliders: a thin vertical channel with a chunky square thumb.
// We only use LinearVertical; LookAndFeel_V4 handles anything else fine.
void BBLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                                      float sliderPos, float minPos, float maxPos,
                                      juce::Slider::SliderStyle style, juce::Slider& s)
{
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, sliderPos, minPos, maxPos, style, s);
        return;
    }

    const auto bounds = juce::Rectangle<float> ((float) x, (float) y,
                                                (float) w, (float) h);

    const float trackW = 4.f;
    auto track = juce::Rectangle<float> (trackW, bounds.getHeight() - 8.f)
                     .withCentre ({ bounds.getCentreX(), bounds.getCentreY() });

    g.setColour (juce::Colour (palette::panelHi));
    g.fillRect (track);
    g.setColour (juce::Colour (palette::hairline));
    g.drawRect (track, 1.f);

    // Filled portion from the bottom up to the thumb.
    auto fill = track.withTop (sliderPos);
    g.setColour (juce::Colour (palette::blood));
    g.fillRect (fill);

    // Square thumb sitting on the track.
    const float thumbW = juce::jmin (bounds.getWidth() - 4.f, 14.f);
    auto thumb = juce::Rectangle<float> (thumbW, 6.f)
                     .withCentre ({ bounds.getCentreX(), sliderPos });
    g.setColour (juce::Colour (palette::ink));
    g.fillRect (thumb);
    g.setColour (juce::Colour (palette::bloodDk));
    g.drawRect (thumb, 1.f);
}

void BBLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool,
                                  int, int, int, int, juce::ComboBox& box)
{
    const auto bounds = juce::Rectangle<float> (0, 0, (float) w, (float) h);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRect (bounds);
    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRect (bounds, 1.f);

    // Little red arrow on the right edge — square-ish, in keeping with the rest.
    const float ah = 4.f, aw = 6.f;
    const auto cx = bounds.getRight() - 10.f;
    const auto cy = bounds.getCentreY();
    juce::Path arrow;
    arrow.addTriangle (cx - aw * 0.5f, cy - ah * 0.5f,
                       cx + aw * 0.5f, cy - ah * 0.5f,
                       cx,             cy + ah * 0.5f);
    g.setColour (box.findColour (juce::ComboBox::arrowColourId));
    g.fillPath (arrow);
}

juce::Font BBLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (11.0f, juce::Font::bold));
}

juce::Font BBLookAndFeel::getLabelFont (juce::Label& l)
{
    return l.getFont().getHeight() > 0.f ? l.getFont()
                                         : juce::Font (juce::FontOptions (10.0f));
}

} // namespace bedridden::gui
