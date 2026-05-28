#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bedridden::gui {

/// Blood-Bucket-flavoured look: square white knobs with a single red tick
/// for direction, square switches, hard edges, no gloss. Everything looks
/// like it was drawn in MS Paint at 3am, on purpose.
class BBLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BBLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getLabelFont    (juce::Label&)    override;
};

// Shared palette so every panel paints from the same bottle.
namespace palette {
    inline constexpr juce::uint32 bg        = 0xff141414; // editor background
    inline constexpr juce::uint32 panel     = 0xff1a1a1a; // panel fill
    inline constexpr juce::uint32 panelHi   = 0xff242424; // raised inset
    inline constexpr juce::uint32 ink       = 0xfff0e8e0; // off-white "paint"
    inline constexpr juce::uint32 grime     = 0xff808080; // dim labels
    inline constexpr juce::uint32 blood     = 0xffe22d2d; // accent red
    inline constexpr juce::uint32 bloodDk   = 0xff8a1818; // shadowed red
    inline constexpr juce::uint32 hairline  = 0xff2c2c2c; // panel border
}

} // namespace bedridden::gui
