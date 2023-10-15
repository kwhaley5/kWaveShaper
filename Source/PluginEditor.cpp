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
    distortionAT(nullptr), bypassTestAT(audioProcessor.apvts, "bypass", bypassTest)
{
    setLookAndFeel(&Lnf);

    addAndMakeVisible(meter[0]);
    addAndMakeVisible(meter[1]);
    addAndMakeVisible(outMeter[0]);
    addAndMakeVisible(outMeter[1]);

    setRotarySlider(inGain);
    setRotarySlider(outGain);
    setRotarySlider(typeSelect);
    setRotarySlider(distortion);
    setRotarySlider(bypassTest);

    typeSelect.onValueChange = [this]
        {
            updateAttachments();
        };

    //addAndMakeVisible(bypass);
    
    updateAttachments();
    setSize (500, 300);
}

WaveShaperAudioProcessorEditor::~WaveShaperAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void WaveShaperAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .05);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .053);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);

    auto center = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .05);
    auto centerHold = center;

    center.removeFromTop(center.getHeight() * .25);
    center.removeFromBottom(center.getHeight() * .33);

    center = centerHold;
    auto topRow = center.removeFromTop(center.getHeight() * .4);
    auto leftTop = topRow.removeFromLeft(topRow.getWidth() * .5);
    //g.drawRect(topRow);

    center = centerHold;
    auto bottomRow = center.removeFromBottom(center.getHeight() * .4);
    auto leftBottom = bottomRow.removeFromLeft(bottomRow.getWidth() * .5);

    g.setFont(15);
    g.drawFittedText("Out Gain", bottomRow.getX() + bottomRow.getWidth(), bottomRow.getCentreY(), 30, 30, juce::Justification::centred, 2);

    juce::Rectangle<float> r;
    r.setX(bounds.getX());
    r.setY(bounds.getY());
    r.setHeight((topRow.getY() - bounds.getY()) * 1.5);
    r.setWidth(bounds.getWidth());
    g.drawRect(r);

    r.setX(bounds.getBottomLeft().getX());
    r.setY(leftBottom.getBottom() - 10);
    r.setHeight(topRow.getY() - bounds.getY() + 10);
    r.setWidth(bounds.getWidth());
    g.drawRect(r);
    
}

void WaveShaperAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .05);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    meter[0].setBounds(meterLSide);
    meter[1].setBounds(inputMeter);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .053);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);
    outMeter[0].setBounds(outMeterLSide);
    outMeter[1].setBounds(outputMeter);

    bounds = getLocalBounds();

    auto center = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .05);
    auto centerHold = center;

    center.removeFromTop(center.getHeight() * .25);
    center.removeFromBottom(center.getHeight() * .33);

    distortion.setBounds(center);

    center = centerHold;
    auto topRow = center.removeFromTop(center.getHeight() * .4);
    auto leftTop = topRow.removeFromLeft(topRow.getWidth() * .5);

    center = centerHold;
    auto bottomRow = center.removeFromBottom(center.getHeight() * .4);
    auto leftBottom = bottomRow.removeFromLeft(bottomRow.getWidth() * .5);

    typeSelect.setBounds(leftTop);
    bypassTest.setBounds(topRow);
    inGain.setBounds(leftBottom);
    outGain.setBounds(bottomRow);
    
}

void WaveShaperAudioProcessorEditor::setRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 50);
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

void WaveShaperAudioProcessorEditor::timerCallback()
{
    for (auto channel = 0; channel < audioProcessor.getTotalNumInputChannels(); channel++) {
        meter[channel].setLevel(audioProcessor.getRMS(channel));
        meter[channel].repaint();

        outMeter[channel].setLevel(audioProcessor.getOutRMS(channel));
        outMeter[channel].repaint();
    }
}
