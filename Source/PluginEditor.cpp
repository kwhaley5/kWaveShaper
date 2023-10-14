/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveShaperAudioProcessorEditor::WaveShaperAudioProcessorEditor (WaveShaperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    inGainAT(audioProcessor.apvts, "inGainValue", inGain), outGainAT(audioProcessor.apvts, "outGainValue", outGain),
    typeSelectAT(audioProcessor.apvts, "typeSelect", typeSelect), bypassAT(audioProcessor.apvts, "bypass", bypass),
    distortionAT(nullptr)
{

    setRotarySlider(inGain);
    setRotarySlider(outGain);
    setRotarySlider(typeSelect);
    setRotarySlider(distortion);

    typeSelect.onValueChange = [this]
        {
            updateAttachments();
        };

    addAndMakeVisible(bypass);
    
    updateAttachments();
    setSize (500, 400);
}

WaveShaperAudioProcessorEditor::~WaveShaperAudioProcessorEditor()
{
}

//==============================================================================
void WaveShaperAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);

    auto center = bounds.reduced(bounds.getWidth() * .2, bounds.getHeight() * .2);

    auto topRow = center.removeFromTop(center.getHeight() * .3);
    auto bottomRow = center.removeFromBottom(center.getHeight() * .43);

    auto leftTop = topRow.removeFromLeft(topRow.getWidth() * .5);
    auto leftBottom = bottomRow.removeFromLeft(bottomRow.getWidth() * .5);
    
}

void WaveShaperAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto center = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .15);

    auto topRow = center.removeFromTop(center.getHeight() * .3);
    auto bottomRow = center.removeFromBottom(center.getHeight() * .43);

    auto leftTop = topRow.removeFromLeft(topRow.getWidth() * .5);
    auto leftBottom = bottomRow.removeFromLeft(bottomRow.getWidth() * .5);

    typeSelect.setBounds(leftTop);
    bypass.setBounds(topRow);
    distortion.setBounds(center);
    inGain.setBounds(leftBottom);
    outGain.setBounds(bottomRow);
    
}

void WaveShaperAudioProcessorEditor::setRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 50);
    slider.setComponentID("Filter");
    addAndMakeVisible(slider);
}

void WaveShaperAudioProcessorEditor::updateAttachments()
{
    distortionAT.reset();

    juce::String newID;
    auto param = typeSelect.getValue();

    if (param == 1)
    {
        newID = audioProcessor.apvts.getParameter("sinDistort")->getParameterID();
    }
    else if (param == 2)
    {
        newID = audioProcessor.apvts.getParameter("quadraticDistort")->getParameterID();
    }
    else if (param == 3)
    {
        newID = audioProcessor.apvts.getParameter("factorDistort")->getParameterID();
    }
    else
    {
        newID = audioProcessor.apvts.getParameter("gbDistort")->getParameterID();
    }

    distortionAT = std::make_unique<Attachment>(audioProcessor.apvts, newID, distortion);
}
