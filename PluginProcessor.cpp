#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr float minGainDb = -60.0f;
    constexpr float maxGainDb = 0.0f;
}

SidechainProAudioProcessor::SidechainProAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",    juce::AudioChannelSet::stereo(), true)
        .withInput  ("Sidechain",juce::AudioChannelSet::stereo(), false)
        .withOutput ("Output",   juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
SidechainProAudioProcessor::createParameterLayout()
{
    using Float = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back (std::make_unique<Float> ("threshold", "Threshold",
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -18.0f));
    p.push_back (std::make_unique<Float> ("ratio", "Ratio",
        juce::NormalisableRange<float> (1.0f, 20.0f, 0.1f), 4.0f));
    p.push_back (std::make_unique<Float> ("attack", "Attack",
        juce::NormalisableRange<float> (0.1f, 200.0f, 0.1f, 0.35f), 10.0f));
    p.push_back (std::make_unique<Float> ("release", "Release",
        juce::NormalisableRange<float> (5.0f, 1000.0f, 1.0f, 0.35f), 120.0f));
    p.push_back (std::make_unique<Float> ("mix", "Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f));
    p.push_back (std::make_unique<Float> ("output", "Output",
        juce::NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f));

    return { p.begin(), p.end() };
}

void SidechainProAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    envelope = 0.0f;
    gainReductionDb.store (0.0f);
}

void SidechainProAudioProcessor::releaseResources()
{
}

bool SidechainProAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    const auto sidechain = layouts.getChannelSet (true, 1);
    return sidechain.isDisabled()
        || sidechain == juce::AudioChannelSet::mono()
        || sidechain == juce::AudioChannelSet::stereo();
}

void SidechainProAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int samples = buffer.getNumSamples();
    const int channels = juce::jmin (2, buffer.getNumChannels());

    auto* thresholdParam = parameters.getRawParameterValue ("threshold");
    auto* ratioParam = parameters.getRawParameterValue ("ratio");
    auto* attackParam = parameters.getRawParameterValue ("attack");
    auto* releaseParam = parameters.getRawParameterValue ("release");
    auto* mixParam = parameters.getRawParameterValue ("mix");
    auto* outputParam = parameters.getRawParameterValue ("output");

    const float threshold = thresholdParam->load();
    const float ratio = juce::jmax (1.0f, ratioParam->load());
    const float attackMs = juce::jmax (0.1f, attackParam->load());
    const float releaseMs = juce::jmax (1.0f, releaseParam->load());
    const float wet = juce::jlimit (0.0f, 1.0f, mixParam->load() / 100.0f);
    const float outputGain = juce::Decibels::decibelsToGain (outputParam->load());

    const float attackCoeff = std::exp (-1.0f / (0.001f * attackMs * (float) currentSampleRate));
    const float releaseCoeff = std::exp (-1.0f / (0.001f * releaseMs * (float) currentSampleRate));

    // Bus 1 is the optional external sidechain input. If it is not connected,
    // the plugin uses its main input as the detector source.
    auto sidechain = getBusCount (true) > 1 ? getBusBuffer (buffer, true, 1)
                                           : juce::AudioBuffer<float>();
    const bool hasSidechain = sidechain.getNumChannels() > 0;
    const int detectorChannels = hasSidechain ? sidechain.getNumChannels() : channels;

    float peakReduction = 0.0f;

    for (int i = 0; i < samples; ++i)
    {
        float detector = 0.0f;
        for (int ch = 0; ch < detectorChannels; ++ch)
        {
            const float x = hasSidechain
                ? sidechain.getReadPointer (ch)[i]
                : buffer.getReadPointer (juce::jmin (ch, channels - 1))[i];
            detector = juce::jmax (detector, std::abs (x));
        }

        const float coeff = detector > envelope ? attackCoeff : releaseCoeff;
        envelope = coeff * envelope + (1.0f - coeff) * detector;

        const float levelDb = juce::Decibels::gainToDecibels (envelope, minGainDb);
        float reductionDb = 0.0f;

        if (levelDb > threshold)
        {
            const float compressedDb = threshold + (levelDb - threshold) / ratio;
            reductionDb = juce::jmax (0.0f, levelDb - compressedDb);
        }

        peakReduction = juce::jmax (peakReduction, reductionDb);
        const float compressedGain = juce::Decibels::decibelsToGain (-reductionDb);
        const float gain = outputGain * ((1.0f - wet) + wet * compressedGain);

        for (int ch = 0; ch < channels; ++ch)
            buffer.getWritePointer (ch)[i] *= gain;
    }

    gainReductionDb.store (peakReduction);
}

juce::AudioProcessorEditor* SidechainProAudioProcessor::createEditor()
{
    return new SidechainProAudioProcessorEditor (*this);
}

void SidechainProAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SidechainProAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SidechainProAudioProcessor();
}
