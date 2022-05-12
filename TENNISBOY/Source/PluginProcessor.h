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
class TableTennisAudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    TableTennisAudioProcessor();
    ~TableTennisAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    
    void updateFilter(); //LPF update function declaration
    

    void parameterChanged(const juce::String& parameterID, float newValue) override; // ParameterChange function declaration
private:
    //==============================================================================

    juce::AudioProcessorValueTreeState treeState; // ValueTreeState created
    juce::dsp::DelayLine<float> mDelayLineL{ 132300 }; //Delay line buffer created for L channel. Init with 1323000 sample buffer (3000 mSeconds at 44.1Khz). Changing this would change the maximum possible delay
    juce::dsp::DelayLine<float> mDelayLineR{ 132300 }; //The same thing but for R channel

    juce::AudioBuffer<float> dryBuffer; // Create an additional buffer for the dry signal

    //Variables used to hold a copy of the slider values. Initialising values are specified too. LPF values are read as raw values from ValueTree.
    float mDelayTime = 1000.f;
    float mDelayTime2 = 2000.0f;
    float mFeedback = 0.3f;
    float mWetDry = 0.5f;

    //IIR filter function declared
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableTennisAudioProcessor)
};
