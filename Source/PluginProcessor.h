/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class WaveShaperAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    WaveShaperAudioProcessor();
    ~WaveShaperAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processSinusoidal(int channel, juce::AudioBuffer<float>& buffer);
    void processQuadratic(int channel, juce::AudioBuffer<float>& buffer);
    void processFactor(int channel, juce::AudioBuffer<float>& buffer);
    void processGB(int channel, juce::AudioBuffer<float>& buffer);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "parameters", createParameterLayout() };

private:
    
    enum WaveShaper {
        none,
        sinusoidal,
        quadratic,
        factor,
        GloubiBoulga
    };

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    juce::AudioParameterBool* bypass{ nullptr };
    juce::AudioParameterInt* typeSelect{ nullptr };
    juce::AudioParameterFloat* sinDistort{ nullptr };
    juce::AudioParameterFloat* quadraticDistort{ nullptr };
    juce::AudioParameterFloat* factorDistort{ nullptr };
    juce::AudioParameterFloat* gbDistort{ nullptr };
    juce::AudioParameterFloat* inGainValue{ nullptr };
    juce::AudioParameterFloat* outGainValue{ nullptr };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveShaperAudioProcessor)
};
