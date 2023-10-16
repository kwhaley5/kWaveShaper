/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KiTiKLNF.h"

//==============================================================================
/**
*/
class WaveShaperAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor&);
    ~WaveShaperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setRotarySlider(juce::Slider&);
    void updateAttachments();
    void timerCallback() override;

private:

    Laf Lnf;

    WaveShaperAudioProcessor& audioProcessor;

    std::array<Laf::LevelMeter, 2> meter;
    std::array<Laf::LevelMeter, 2> outMeter;

    juce::Slider inGain         { "In Gain" },
                 outGain        { "Out Gain" },
                 typeSelect     { "Type Select" },
                 distortion     { "Shape" };

    juce::Slider bypass         { "Bypass" };

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    Attachment inGainAT, outGainAT, typeSelectAT, bypassAT;
    std::unique_ptr<Attachment> distortionAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessorEditor)
};
