/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveShaperAudioProcessor::WaveShaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bypass"));
    typeSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("typeSelect"));
    sinDistort = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("sinDistort"));
    quadraticDistort = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("quadraticDistort"));
    factorDistort = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("factorDistort"));
    gbDistort = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("gbDistort"));
    inGainValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("inGainValue"));
    outGainValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("outGainValue"));

}

WaveShaperAudioProcessor::~WaveShaperAudioProcessor()
{
}

//==============================================================================
const juce::String WaveShaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveShaperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaveShaperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaveShaperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaveShaperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveShaperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaveShaperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveShaperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WaveShaperAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaveShaperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WaveShaperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    spec.sampleRate = sampleRate;

    inGain.reset();
    inGain.prepare(spec);

    outGain.reset();
    outGain.prepare(spec);
}

void WaveShaperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveShaperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WaveShaperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //TODO:
        //figure out how to change knobs instead of just having multiple

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (bypass->get())
        return;

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    inGain.setGainDecibels(inGainValue->get());
    inGain.process(context);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        switch (typeSelect->get())
        {
            case WaveShaper::sinusoidal:
                processSinusoidal(channel, buffer);
                break;
            
            case WaveShaper::quadratic:
                processQuadratic(channel, buffer);
                break;
            
            case WaveShaper::factor:
                processFactor(channel, buffer);
                break;

            case WaveShaper::GloubiBoulga:
                processGB(channel, buffer);
                break;
        }

        //else if (typeSelect->get() == 4) //this would be more useful in an on off scenario, like synth
        //{
        //    for (int s = 0; s < buffer.getNumSamples(); ++s) 
        //    {
        //        channelData[s] = 1.5 * channelData[s] - .5 * pow(channelData[s], 3);
        //    }
        //}
    }

    outGain.setGainDecibels(outGainValue->get());
    outGain.process(context);

    /******************************************************************************************************
    *
    *                         PLACEHOLDER FOR CLIPPING TO BE MOVED
    *
    *******************************************************************************************************/


    //if (clipSelect->get() == 1) //Hard Clipper
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s) 
    //    {
    //        auto gain = juce::Decibels::decibelsToGain(threshold->get());
    //        channelData[s] > gain ? channelData[s] = gain : channelData[s] = channelData[s];
    //        channelData[s] < -gain ? channelData[s] = -gain : channelData[s] = channelData[s];
    //    }
    //}

    //else if (clipSelect->get() == 2) //Soft Clipper (cubixc)
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s)
    //    {
    //        auto newLimit = juce::Decibels::decibelsToGain(threshold->get());
    //        auto inverse = 1 / newLimit;
    //        auto resizeSamples = channelData[s]  * inverse;
    //        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
    //        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
    //        auto cubic = (resizeSamples - pow(resizeSamples, 3) / 3);

    //        channelData[s] = cubic * newLimit;
    //    }
    //}

    //else if (clipSelect->get() == 3) //sin clipping, 
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s)
    //    {
    //        auto newLimit = juce::Decibels::decibelsToGain(threshold->get());
    //        auto inverse = 1 / newLimit;
    //        auto resizeSamples = channelData[s] * inverse;
    //        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
    //        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;

    //        auto sinosidal = sin(3 * juce::MathConstants<float>::pi * resizeSamples / 4);

    //        channelData[s] = sinosidal * newLimit;
    //    }

    //}

    //else if (clipSelect->get() == 4) //hyperbolic tangent
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s)
    //    {
    //        auto newLimit = juce::Decibels::decibelsToGain(threshold->get());
    //        auto inverse = 1 / newLimit;
    //        auto resizeSamples = channelData[s] * inverse;
    //        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
    //        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;

    //        auto hyperTan = tanh(5*resizeSamples)*(3/juce::MathConstants<float>::pi);

    //        channelData[s] = hyperTan * newLimit;
    //    }
    //}

    //else if (clipSelect->get() == 5) //arctangent tangent
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s)
    //    {
    //        auto newLimit = juce::Decibels::decibelsToGain(threshold->get());
    //        auto inverse = 1 / newLimit;
    //        auto resizeSamples = channelData[s] * inverse;
    //        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
    //        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;

    //        auto hyperTan = atan(5 * resizeSamples) * (2 / juce::MathConstants<float>::pi);

    //        channelData[s] = hyperTan * newLimit;
    //    }
    //}

    //else if (clipSelect->get() == 2) //Soft Clipper (quntic)
    //{
    //    for (int s = 0; s < buffer.getNumSamples(); ++s)
    //    {
    //        auto newLimit = juce::Decibels::decibelsToGain(threshold->get());
    //        auto inverse = 1 / newLimit;
    //        auto resizeSamples = channelData[s] * inverse;
    //        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
    //        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
    //        auto quintic = resizeSamples - pow(resizeSamples, 5) / 5;

    //        channelData[s] = quintic * newLimit;
    //    }
    //}

    //need to figure out how to keep the volumes the same

}

void WaveShaperAudioProcessor::processSinusoidal(int channel, juce::AudioBuffer<float>& buffer)
{
    float* data = buffer.getWritePointer(channel);

    auto z = juce::MathConstants<float>::pi * sinDistort->get();
    auto a = (1 / sin(z));
    auto b = 1 / sinDistort->get();

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        if (data[s] > b)
        {
            data[s] = 1;
        }
        else
        {
            data[s] = sin(z * data[s]) * a;
        }

    }
}

void WaveShaperAudioProcessor::processQuadratic(int channel, juce::AudioBuffer<float>& buffer)
{
    float* channelData = buffer.getWritePointer(channel);

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        channelData[s] = channelData[s] * (abs(channelData[s]) + quadraticDistort->get()) / (pow(channelData[s], 2) + (quadraticDistort->get() - 1) * abs(channelData[s]) + 1);

    }
}

void WaveShaperAudioProcessor::processFactor(int channel, juce::AudioBuffer<float>& buffer)
{
    float* channelData = buffer.getWritePointer(channel);

    auto factor = 2 * factorDistort->get() / (1 - factorDistort->get());

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        channelData[s] = ((1 + factor) * channelData[s]) / (1 + factor * abs(channelData[s]));
    }
}

void WaveShaperAudioProcessor::processGB(int channel, juce::AudioBuffer<float>& buffer)
{
    float* channelData = buffer.getWritePointer(channel);

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        auto distort = channelData[s] * gbDistort->get();
        auto constant = 1 + exp(sqrt(fabs(distort)) * -0.75);

        channelData[s] = (exp(distort) - exp(-distort * constant)) / (exp(distort) + exp(-distort));
    }
}

//==============================================================================
bool WaveShaperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaveShaperAudioProcessor::createEditor()
{
    return new WaveShaperAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void WaveShaperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WaveShaperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout WaveShaperAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto amountRange = NormalisableRange<float>(.01, .99, .01, 1);
    auto amountGreaterRange = NormalisableRange<float>(.01, 10, .01, 1);
    auto gainRange = NormalisableRange<float>(-20, 20, .1, 1);

    layout.add(std::make_unique<AudioParameterFloat>("inGainValue", "Gain In", gainRange, 0));
    layout.add(std::make_unique<AudioParameterInt>("typeSelect", "Disrotion Type", 1, 4, 1));
    layout.add(std::make_unique<AudioParameterFloat>("sinDistort", "Sine Distortion Factor", amountRange, .5));
    layout.add(std::make_unique<AudioParameterFloat>("quadraticDistort", "Quadratic Distortion Factor", amountGreaterRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>("factorDistort", "Factor Distortion Factor", amountRange, .5));
    layout.add(std::make_unique<AudioParameterFloat>("gbDistort", "Gloubi Boulga Distortion Factor", amountGreaterRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>("outGainValue", "Gain Out", gainRange, 0));
    layout.add(std::make_unique<AudioParameterBool>("bypass", "Bypassed", false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveShaperAudioProcessor();
}
