/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WaveShaperAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor&);
    ~WaveShaperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setRotarySlider(juce::Slider&);
    void updateAttachments();

private:

    WaveShaperAudioProcessor& audioProcessor;

    juce::Slider inGain         { "In Gain" },
                 outGain        { "outGain" },
                 typeSelect     { "Type Select" },
                 distortion     { "Distortion" };

    juce::ToggleButton bypass   { "Bypass" };

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    Attachment inGainAT, outGainAT, typeSelectAT;
    std::unique_ptr<Attachment> distortionAT;

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessorEditor)
};
