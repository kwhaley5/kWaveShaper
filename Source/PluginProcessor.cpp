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
    typeSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("typeSelect"));
    amount = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("amount"));
    clipSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("clipSelect"));
    threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("threshold"));
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        if (typeSelect->get() == 1) //Wavefolds quickly, probably limit to 2
        {
            
            auto z = juce::MathConstants<float>::pi * amount->get();
            auto a = 1 / sin(z);
            auto b = 1 / amount->get();

            for (int s = 0; s < buffer.getNumSamples(); ++s)
            {
                if (channelData[s] > b)
                {
                    channelData[s] = 1;
                }
                else
                {
                    channelData[s] = sin(z * channelData[s]) * a;
                }
                
            }
        }

        else if (typeSelect->get() == 2)
        {
            for (int s = 0; s < buffer.getNumSamples(); ++s)
            {
                channelData[s] = channelData[s] * (abs(channelData[s]) + amount->get()) / (pow(channelData[s], 2) + (amount->get() - 1) * abs(channelData[s]) + 1); //Simple, crunchy wavefolding
                test = juce::Decibels::gainToDecibels(channelData[s]);
                
            }
        }

        else if (typeSelect->get() == 3) //has to be less then 1
        {
            auto factor = 2 * amount->get() / (1 - amount->get());

            for (int s = 0; s < buffer.getNumSamples(); ++s)
            {
               channelData[s] =  ((1 + factor) * channelData[s]) / (1 + factor * abs(channelData[s]));
            }

        }

        else if (typeSelect->get() == 4) //this would be more useful in an on off scenario, like synth
        {
            for (int s = 0; s < buffer.getNumSamples(); ++s) 
            {
                channelData[s] = 1.5 * channelData[s] - .5 * pow(channelData[s], 3);
            }
        }

        else if (typeSelect->get() == 5)
        {
            for (int s = 0; s < buffer.getNumSamples(); ++s) //Another good distrotion, maybe go up to 10?
            {
                auto distort = channelData[s] * amount->get();
                auto constant = 1 + exp(sqrt(fabs(distort)) * -0.75);

                channelData[s] = (exp(distort) - exp(-distort * constant)) / (exp(distort) + exp(-distort));
            }
        }

        else
        {

            //for (int s = 0; s < buffer.getNumSamples(); ++s) //This breaks, will keep for reference
            //{
            //    auto t = juce::Decibels::decibelsToGain(threshold->get());
            //    channelData[s] = (channelData[s] - t + pow(t, 2)) / channelData[s];

            //}

        }

        if (clipSelect->get() == 1)
        {
            for (int s = 0; s < buffer.getNumSamples(); ++s) //This seems to work, but it would tech be hard clipping
            {
                auto gain = juce::Decibels::decibelsToGain(threshold->get());
                channelData[s] > gain ? channelData[s] = gain : channelData[s] = channelData[s];
            }
        }

        else if (clipSelect->get() == 2) //this distorts the crap out of it, not sure if I did it right
        {
            for (int s = 0; s < buffer.getNumSamples(); ++s)
            {
                auto gain = juce::Decibels::decibelsToGain(threshold->get());

                if (channelData[s] > gain)
                {
                    channelData[s] = gain * (2 / 3);
                }
                else if (channelData[s] < -gain)
                {
                    channelData[s] = -gain * (2 / 3);
                }
                else
                {
                    channelData[s] = channelData[s] - (pow(channelData[s], 3) / 3);
                }
            }
        }
    }

    //Now lets try our hands at a clipper
    


}

//==============================================================================
bool WaveShaperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaveShaperAudioProcessor::createEditor()
{
    //return new WaveShaperAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
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

    auto amountRange = NormalisableRange<float>(.1, 5, .1, 1);
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);

    layout.add(std::make_unique<AudioParameterInt>("typeSelect", "Disrotion Type", 1, 6, 1));
    layout.add(std::make_unique<AudioParameterFloat>("amount", "Distortion Factor", amountRange, 1));
    layout.add(std::make_unique<AudioParameterInt>("clipSelect", "Clipper Type", 1, 5, 1));
    layout.add(std::make_unique<AudioParameterFloat>("threshold", "Clipper Treshold", threshRange, 0));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveShaperAudioProcessor();
}
