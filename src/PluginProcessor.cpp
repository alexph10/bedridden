#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/BBSound.h"
#include "dsp/BBVoice.h"

namespace bedridden {

BedriddenProcessor::BedriddenProcessor()
    : juce::AudioProcessor (BusesProperties()
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    // Monophonic for now — polyphony lands in a later step.
    synth.addVoice (new BBVoice (apvts));
    synth.addSound (new BBSound());
}

void BedriddenProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

bool BedriddenProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::stereo()
        || out == juce::AudioChannelSet::mono();
}

void BedriddenProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* BedriddenProcessor::createEditor()
{
    return new BedriddenEditor (*this);
}

void BedriddenProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void BedriddenProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout
BedriddenProcessor::createLayout()
{
    using P = juce::AudioParameterFloat;
    using C = juce::AudioParameterChoice;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const juce::StringArray waves { "Sine", "Saw", "Square", "Triangle", "Noise" };
    const juce::StringArray algos {
        "Ring", "Add", "Sub", "AbsMul", "Min", "Max",
        "XOR", "AND", "S&H", "PhaseMod", "HardSync", "Threshold"
    };
    const juce::StringArray targets { "PM", "FM", "RM", "AM" };

    auto addOsc = [&] (juce::String id, juce::String name) {
        layout.add (std::make_unique<C> (id + "_wave",   name + " Wave",   waves, 0));
        layout.add (std::make_unique<P> (id + "_coarse", name + " Coarse",
                                         juce::NormalisableRange<float> (-24.f, 24.f, 1.f), 0.f));
        layout.add (std::make_unique<P> (id + "_fine",   name + " Fine",
                                         juce::NormalisableRange<float> (-100.f, 100.f, 1.f), 0.f));
        layout.add (std::make_unique<P> (id + "_level",  name + " Level",
                                         juce::NormalisableRange<float> (0.f, 1.f, 0.001f), 1.f));
    };
    addOsc ("oscA", "Osc A");
    addOsc ("oscB", "Osc B");
    addOsc ("oscC", "Osc C");

    auto addAdsr = [&] (juce::String id, juce::String name) {
        layout.add (std::make_unique<P> (id + "_attack",  name + " Attack",
                                         juce::NormalisableRange<float> (0.001f, 5.f, 0.001f, 0.3f), 0.01f));
        layout.add (std::make_unique<P> (id + "_decay",   name + " Decay",
                                         juce::NormalisableRange<float> (0.001f, 5.f, 0.001f, 0.3f), 0.20f));
        layout.add (std::make_unique<P> (id + "_sustain", name + " Sustain",
                                         juce::NormalisableRange<float> (0.f, 1.f, 0.001f), 0.7f));
        layout.add (std::make_unique<P> (id + "_release", name + " Release",
                                         juce::NormalisableRange<float> (0.001f, 8.f, 0.001f, 0.3f), 0.30f));
    };
    addAdsr ("envA", "Env A");
    addAdsr ("envB", "Env B");

    layout.add (std::make_unique<C> ("algorithm",  "Algorithm", algos,   0));
    layout.add (std::make_unique<C> ("mod_target", "Mod Target", targets, 0));
    layout.add (std::make_unique<P> ("mod_depth",  "Mod Depth",
                                     juce::NormalisableRange<float> (0.f, 1.f, 0.001f), 0.5f));
    layout.add (std::make_unique<P> ("out_gain",   "Output",
                                     juce::NormalisableRange<float> (0.f, 1.f, 0.001f), 0.7f));

    return layout;
}

} // namespace bedridden

// JUCE plugin factory entry point.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new bedridden::BedriddenProcessor();
}
