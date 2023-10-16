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
    setLookAndFeel(&Lnf);

    addAndMakeVisible(meter[0]);
    addAndMakeVisible(meter[1]);
    addAndMakeVisible(outMeter[0]);
    addAndMakeVisible(outMeter[1]);

    setRotarySlider(inGain);
    setRotarySlider(outGain);
    setRotarySlider(typeSelect);
    setRotarySlider(distortion);
    setRotarySlider(bypass);

    typeSelect.onValueChange = [this]
        {
            updateAttachments();
        };
    
    updateAttachments();
    setSize (500, 300);
    startTimerHz(24);
}

WaveShaperAudioProcessorEditor::~WaveShaperAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void WaveShaperAudioProcessorEditor::paint(juce::Graphics& g)
{

    g.fillAll(juce::Colours::black);
    auto bounds = getLocalBounds();
    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);

    auto fontSize = 15;
    g.setFont(fontSize);
    g.setColour(juce::Colours::white);
    
    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .05);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .053);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);
    g.drawFittedText("kwaudioproduction.com", bounds.getX(), bounds.getBottom() - 22, bounds.getWidth(), 25, juce::Justification::left, 1);
    g.drawFittedText("kwhaley5.gumroad.com", bounds.getX(), bounds.getBottom() - 22, bounds.getWidth(), 25, juce::Justification::right, 1);

    auto center = bounds.reduced(bounds.getWidth() * .15, bounds.getHeight() * .05);
    auto centerHold = center;

    center.removeFromTop(center.getHeight() * .25);
    center.removeFromBottom(center.getHeight() * .33);

    center = centerHold;
    auto topRow = center.removeFromTop(center.getHeight() * .4);
    auto leftTop = topRow.removeFromLeft(topRow.getWidth() * .5);

    juce::Rectangle<int> name;
    name.setX(topRow.getX() - topRow.getWidth() / 4);
    name.setY(leftTop.getY() - 10);
    name.setWidth(leftTop.getWidth() / 2);
    name.setHeight(leftTop.getHeight() / 1.3);
    auto newFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::OFFSHORE_TTF, BinaryData::OFFSHORE_TTFSize));
    g.setFont(newFont);
    g.setFont(50);
    g.drawFittedText("KiTiK Wave Shapper", name, juce::Justification::centred, 3);

    center = centerHold;
    auto bottomRow = center.removeFromBottom(center.getHeight() * .4);
    auto leftBottom = bottomRow.removeFromLeft(bottomRow.getWidth() * .5);

    name.setX(bottomRow.getX() - bottomRow.getWidth() / 4);
    name.setY(leftBottom.getY() + 35);
    name.setWidth(leftBottom.getWidth() / 2);
    name.setHeight(leftBottom.getHeight() / 1.75);
    g.drawImage(logo, name.toFloat(), juce::RectanglePlacement::fillDestination);

    juce::Rectangle<float> r;
    r.setX(bounds.getBottomLeft().getX());
    r.setY(leftBottom.getBottom() - 10);
    r.setHeight(topRow.getY() - bounds.getY() + 10);
    r.setWidth(bounds.getWidth());
    g.setColour(juce::Colour(186u, 34u, 34u));
    g.drawLine(0, r.getY() + 7, 500, r.getY() + 7, 1);

    g.setFont(juce::Font::Font());
    g.setFont(fontSize);
    g.setColour(juce::Colours::white);
    g.drawFittedText("Out Gain", bottomRow.getX() + bottomRow.getWidth() - fontSize, bottomRow.getCentreY() - fontSize, 30, 30, juce::Justification::centred, 2);
    g.drawFittedText("Bypass", topRow.getX() + topRow.getWidth() - 20, topRow.getCentreY() - fontSize, 60, 30, juce::Justification::centred, 2);
    
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
    bypass.setBounds(topRow);
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

void WaveShaperAudioProcessorEditor::updateAttachments() //Will have to go back and try out a map next time.
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
