#include "PluginEditor.h"

SidechainProAudioProcessorEditor::SidechainProAudioProcessorEditor (SidechainProAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (620, 330);
    setupSlider (threshold, thresholdLabel, "THRESHOLD");
    setupSlider (ratio, ratioLabel, "RATIO");
    setupSlider (attack, attackLabel, "ATTACK");
    setupSlider (release, releaseLabel, "RELEASE");
    setupSlider (mix, mixLabel, "MIX");
    setupSlider (output, outputLabel, "OUTPUT");

    thresholdAttachment = std::make_unique<Attachment> (p.parameters, "threshold", threshold);
    ratioAttachment = std::make_unique<Attachment> (p.parameters, "ratio", ratio);
    attackAttachment = std::make_unique<Attachment> (p.parameters, "attack", attack);
    releaseAttachment = std::make_unique<Attachment> (p.parameters, "release", release);
    mixAttachment = std::make_unique<Attachment> (p.parameters, "mix", mix);
    outputAttachment = std::make_unique<Attachment> (p.parameters, "output", output);

    meterLabel.setJustificationType (juce::Justification::centred);
    meterLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    meterLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible (meterLabel);
    startTimerHz (20);
}

void SidechainProAudioProcessorEditor::setupSlider (juce::Slider& slider,
                                                      juce::Label& label,
                                                      const juce::String& name)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 74, 22);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff20c7b5));
    slider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff202936));
    addAndMakeVisible (slider);

    label.setText (name, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colours::white);
    label.setFont (juce::Font (12.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void SidechainProAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101722));
    g.setColour (juce::Colour (0xff20c7b5));
    g.setFont (juce::Font (25.0f, juce::Font::bold));
    g.drawFittedText ("SIDECHAIN PRO", 16, 12, getWidth() - 32, 34,
                      juce::Justification::centred, 1);
    g.setColour (juce::Colour (0xff344354));
    g.drawRoundedRectangle (12.0f, 55.0f, (float) getWidth() - 24.0f, 252.0f, 12.0f, 1.5f);
}

void SidechainProAudioProcessorEditor::resized()
{
    const int margin = 24;
    const int top = 75;
    const int colWidth = (getWidth() - margin * 2) / 6;
    juce::Component* sliders[] = { &threshold, &ratio, &attack, &release, &mix, &output };
    juce::Component* labels[] = { &thresholdLabel, &ratioLabel, &attackLabel,
                                  &releaseLabel, &mixLabel, &outputLabel };

    for (int i = 0; i < 6; ++i)
    {
        const int x = margin + i * colWidth;
        labels[i]->setBounds (x, top, colWidth, 24);
        sliders[i]->setBounds (x + 2, top + 26, colWidth - 4, 145);
    }
    meterLabel.setBounds (margin, 258, getWidth() - margin * 2, 30);
}

void SidechainProAudioProcessorEditor::timerCallback()
{
    meterLabel.setText ("GAIN REDUCTION: "
        + juce::String (audioProcessor.gainReductionDb.load(), 1) + " dB",
        juce::dontSendNotification);
}

