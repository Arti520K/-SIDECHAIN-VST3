#pragma once

#include "PluginProcessor.h"

class SidechainProAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit SidechainProAudioProcessorEditor (SidechainProAudioProcessor&);
    ~SidechainProAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void setupSlider (juce::Slider&, juce::Label&, const juce::String&);
    SidechainProAudioProcessor& audioProcessor;

    juce::Slider threshold, ratio, attack, release, mix, output;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, mixLabel, outputLabel;
    juce::Label meterLabel;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> thresholdAttachment, ratioAttachment, attackAttachment;
    std::unique_ptr<Attachment> releaseAttachment, mixAttachment, outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SidechainProAudioProcessorEditor)
};
