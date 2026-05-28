#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "gui/Scope.h"

namespace bedridden {

class BedriddenProcessor : public juce::AudioProcessor
{
public:
    BedriddenProcessor();
    ~BedriddenProcessor() override = default;

    // ---- AudioProcessor ----
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Bedridden"; }
    bool acceptsMidi()  const override { return true;  }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int  getNumPrograms() override                { return 1; }
    int  getCurrentProgram() override             { return 0; }
    void setCurrentProgram (int) override         {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    // All knob state lives in here — the GUI binds to it via attachments,
    // and `getStateInformation` round-trips it for preset save/load.
    juce::AudioProcessorValueTreeState apvts;

    // Audio thread writes into this, the GUI Timer reads from it.
    // Public because the editor needs direct access to construct the Scope.
    ScopeBuffer scopeBuffer;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BedriddenProcessor)
};

} // namespace bedridden
