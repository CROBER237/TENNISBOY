/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TableTennisAudioProcessorEditor::TableTennisAudioProcessorEditor(TableTennisAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), treeState(vts)
{
    // Define plugin Window Size
    setSize(400, 400);


    // Create Delay time control

    delayTimeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "delayTime", delayTimeSlider);           //Create a slider Attachment
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);                                                                                 //Define Slider as Vertical Drag Rotary Knob
    delayTimeSlider.setRange(1.0f, 3000.0f, 1.0f);                                                                     //Set parameter value range and interval
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 25);       //Text Box placement - Set to be able to type value in, i.e. "Read Only = False"
    addAndMakeVisible(&delayTimeSlider);                                                                                                            //Add to display

    addAndMakeVisible(delayTimeLabel);                                                                                                          //Add Parameter label to display
    delayTimeLabel.setText("Delay time (mS)", juce::dontSendNotification);                                                                 //Set Text
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);                                                                              //Attach to slider, centre above
    delayTimeLabel.setJustificationType(juce::Justification::centred);                                                                          //Text Justification centred




    // Create Feedback Control
    feedbackValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "feedback", feedbackSlider);
    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setRange(0.0f, 0.99f, 0.01f);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 25);
    addAndMakeVisible(&feedbackSlider);

    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    feedbackLabel.setJustificationType(juce::Justification::centred);

    // Create Wet/Dry Control
    wetDryValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "wetDry", wetDrySlider);
    wetDrySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    wetDrySlider.setRange(0.0f, 1.0f, 0.01f);
    wetDrySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 25);
    addAndMakeVisible(&wetDrySlider);

    addAndMakeVisible(wetDryLabel);
    wetDryLabel.setText("Mix", juce::dontSendNotification);
    wetDryLabel.attachToComponent(&wetDrySlider, false);
    wetDryLabel.setJustificationType(juce::Justification::centred);


    //Create LPF Control
    lpfValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "lpf", lpfSlider);
    lpfSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lpfSlider.setRange(1.0f, 20000.0f, 1.0f);
    lpfSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 25);
    addAndMakeVisible(&lpfSlider);

    addAndMakeVisible(lpfLabel);
    lpfLabel.setText("Cutoff (Hz)", juce::dontSendNotification);
    lpfLabel.attachToComponent(&lpfSlider, false);
    lpfLabel.setJustificationType(juce::Justification::centred);

    //Create Resonance Control
    qValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "Q", qSlider);
    qSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    qSlider.setRange(0.1f, 15.0f, 0.1f);
    qSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 75, 25);
    addAndMakeVisible(&qSlider);

    addAndMakeVisible(qLabel);
    qLabel.setText("Q", juce::dontSendNotification);
    qLabel.attachToComponent(&qSlider, false);
    qLabel.setJustificationType(juce::Justification::centred);



   
}

TableTennisAudioProcessorEditor::~TableTennisAudioProcessorEditor()
{

}

//==============================================================================
void TableTennisAudioProcessorEditor::paint(juce::Graphics& g)
{
    {
        //Set up Graphics characteristics


        g.fillAll(juce::Colours::grey); // Fill colour - Grey
        g.setColour(juce::Colours::white); // Text colour - White
        g.setFont(20); // Font Size
        g.drawFittedText("TENNISBOY jr", 80, 30, 250, 15, juce::Justification::centred, 1, 1.0f); //Draw title Text
        g.setFont(18); // Font Size
    	g.drawFittedText("Effect LPF", 120, 230, 150, 10, juce::Justification::centred, 1, 1.0f); // Draw LPF title text
        g.setFont(15); // Font Size
    	g.drawFittedText("loudtoys", 5, 10, 80, 5, juce::Justification::centred, 1, 1.0f);
        g.setFont(10); // Font Size
        g.drawFittedText("Scamalogue Echo Processor", 250, 10, 150, 5, juce::Justification::centred, 1, 1.0f);
    }
}

void TableTennisAudioProcessorEditor::resized()
{
    //Positioning of Sub-components, (x, y, width, height)

    delayTimeSlider.setBounds(50, 85, 120, 120);
    feedbackSlider.setBounds(150, 85, 120, 120);
    wetDrySlider.setBounds(250, 85, 120, 120);
    lpfSlider.setBounds(100, 280, 80, 80);
    qSlider.setBounds(200, 280, 80, 80);
}
