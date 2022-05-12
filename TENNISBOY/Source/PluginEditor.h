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
class TableTennisAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TableTennisAudioProcessorEditor(TableTennisAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~TableTennisAudioProcessorEditor() override;

	//==============================================================================

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    //Declare Audio Processor and Value Tree
    TableTennisAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& treeState; 

    //Declare Slider objects for each parameter
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider wetDrySlider;
	juce::Slider lpfSlider;
	juce::Slider qSlider;

    //Declare Labels for each parameter
    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label wetDryLabel;
    juce::Label lpfLabel;
    juce::Label qLabel;

    //Declare Unique Pointers for each parameter
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> feedbackValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> wetDryValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> lpfValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> qValue;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableTennisAudioProcessorEditor)
};
