/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TableTennisAudioProcessor::TableTennisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), treeState(*this, nullptr, juce::Identifier("PARAMETERS"),
        { std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay (samples)", 0.f, 3000.f, 1000.f),
          std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback 0-1", 0.f, 0.99f, 0.3f),
         std::make_unique<juce::AudioParameterFloat>("wetDry", "Mix 0-1", 0.f, 1.0f, 0.3f),
        	std::make_unique<juce::AudioParameterFloat>("lpf", "frequency",  juce::NormalisableRange<float>(20.0f,20000.0f,1.0f, 0.35f), 600.f),
            std::make_unique<juce::AudioParameterFloat>("Q", "resonance", 0.1f, 15.f, 1.0f)
        }), lowPassFilter(juce::dsp::IIR::Coefficients  <float>::makeLowPass(44100, 600.f, 1.f)) //LPF
#endif

    //Value Tree instantiated. 5 Parameters created - delayTime, feedback, wetDry, lpf and Q. Each with a value range set and initial values specified.
    //lpf frequency has a normalized range, skewed towards emphasis of lower frequencies, this mimics the logarithmic nature of human frequency perception.
	//LPF is instantiated here also with initial values.

{
    const juce::StringArray params = { "delayTime", "feedback", "wetDry"}; // Array is created with first three parameter tags. LPF are read as raw data straight from the tree
    for (int i = 0; i <= 2; ++i) // increments through array
    {
        treeState.addParameterListener(params[i], this);  // Assigns a listener to the first 3 params 
    }
}
TableTennisAudioProcessor::~TableTennisAudioProcessor()
{
}

//==============================================================================
const juce::String TableTennisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TableTennisAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TableTennisAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TableTennisAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TableTennisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TableTennisAudioProcessor::getNumPrograms()
{
    return 1;  
}

int TableTennisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TableTennisAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String TableTennisAudioProcessor::getProgramName(int index)
{
    return {};
}

void TableTennisAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void TableTennisAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    

    juce::dsp::ProcessSpec spec; //set up structure which contains info about context in which func is likely to be called
    spec.sampleRate = sampleRate; //Sample Rate for session specified
    spec.maximumBlockSize = samplesPerBlock; // Maximum no. samples which will be in a block sent to process
    spec.numChannels = getTotalNumOutputChannels(); //no. output channels

    mDelayLineL.reset(); //resets the mDelayLineL Variable
    mDelayLineL.prepare(spec);

    mDelayLineR.reset(); //resets the mDelayLineR Variable
    mDelayLineR.prepare(spec);

    lowPassFilter.reset(); // Resets LPF Variable
    lowPassFilter.prepare(spec);
}

void TableTennisAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TableTennisAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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
//LPF
void TableTennisAudioProcessor::updateFilter() // This function is called by the process block to continually check for changed in the LPF parameter
{
    float freq = *treeState.getRawParameterValue("lpf"); // create float variable - get lpf frequency as a Raw value from the Value Tree
	float res = *treeState.getRawParameterValue("Q");// create float variable - get lpf resonance as a Raw value from the Value Tree

  

    *lowPassFilter.state = *juce::dsp::IIR::Coefficients <float> ::makeLowPass(44100, freq, res);

	//Update the LPF state with the values in the freq and res variables^^
}

void TableTennisAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    dryBuffer.makeCopyOf(buffer); // Create a copy of the audio buffer and store in the dryBuffer. This will be summed at the end of the process block to create a wet/dry mix

    //Low Pass Filter
    //This acts on the audio buffer, hence creating the copy of the dry signal to sum at the end of the process block.
    
		juce::dsp::AudioBlock <float> block(buffer);                                 //Create an AudioBlock and fill with the contents of the Audio buffer
		updateFilter();                                                                //call the updateFilter function to update the parameter values in the LPF
		lowPassFilter.process(juce::dsp::ProcessContextReplacing <float>(block));   //Capture context data for the LPF as to where in the block is currently being processed



    //Delay Processing
    for (int channel = 0; channel < 2; ++channel) //Cycle through the 2 channels
    
    {

        //L Channel
        if (channel == 0)
        {
            auto* channelData = buffer.getWritePointer(channel); // store where buffer write pointer is

            


            for (int i = 0; i < buffer.getNumSamples(); i++) //increment through all samples in buffer
            {


            	float in = channelData[i];
            	// assign unaffected current buffer sample to local variable: in

            	float temp = mDelayLineL.popSample(channel, mDelayTime, false);
            	// take one sample from the left delay line, delayed by DT1 and stored in local variable: temp

            	float temp2 = mDelayLineL.popSample(channel, mDelayTime2, true);
                // take another sample from the left delay line, delayed by DT2 and store in local variable: temp2

            	mDelayLineL.pushSample(channel, in + (temp * mFeedback));
                //Push a sample into the LEFT Delay Line. The sample is the sum of the unaffected sample (in) with the first delayed sample (temp), multiplied by the Feedback modifier (mFeedback)

            	mDelayLineR.pushSample(channel, (temp2 * mFeedback));
                //Push a sample into the RIGHT Delay Line. The sample is the second delayed sample (temp2), multiplied by the Feedback modifier (mFeedback). Note: no unaffected signal.
                //This creates the "cross pollinating" Ping-Pong Effect



                channelData[i] = (temp * sin(mWetDry * 1.5708));
                //The Current sample in the channel Data is replaced with the contents of the temp variable, multiplied by the wet/Dry value.
                // The sin function is one half of an equal power crossfade calculation. The other cos portion of the calculation takes place
                // at the end of the process block, as the dry signal is mixed in again from the dryBuffer. this allows for the LPF to only affect the Wet signal.
            }
            
        }




        
        //R Channel
        //the reciprocal mirror image process to the L channel has been repeated for the R channel. The cross pollination on the Delay Lines is reversed,
        //with DT1 and DT2 swapped, creating a "trippy"space echo -esque ping pong effect. DT1 and DT2 are related to each other by a factor of 0.79

        else if (channel == 1)
        {
            auto* channelData = buffer.getWritePointer(channel);




            for (int i = 0; i < buffer.getNumSamples(); i++) 
            {


                float in = channelData[i] ; 
                float temp = mDelayLineR.popSample(channel, mDelayTime2, false); 
                float temp2 = mDelayLineR.popSample(channel, mDelayTime, true);
                mDelayLineR.pushSample(channel, in + (temp * mFeedback));
                mDelayLineL.pushSample(channel, (temp2 * mFeedback));
            	channelData[i] = (temp * sin(mWetDry * 1.5708)); 
            }
        }

        
   }

	//Sum DRY buffer with WET buffer

	float DryGain = (1 * cos(mWetDry * 1.5708)); // The reciprocal part of the Wet/Dry mix equal power crossfade calculation

    for (int channel = 0; channel < totalNumInputChannels; ++channel) // increment through channels
    {
        buffer.addFromWithRamp(channel, 0, dryBuffer.getWritePointer(channel), dryBuffer.getNumSamples(), DryGain , DryGain);
        //Sum a sample from dryBuffer (copied at beginning of process block) with buffer (wet)
        //before a dry sample is summed it is multiplied by a flat gain "ramp"
        //i.e. theDry Gain Value from the Wet/Dry crossfade calculation
    }
}

//==============================================================================
bool TableTennisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}



juce::AudioProcessorEditor* TableTennisAudioProcessor::createEditor()
{
    return new TableTennisAudioProcessorEditor(*this, treeState);
}

//==============================================================================
void TableTennisAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    //Saving State information FUNCTIONALITY INCOMPLETE
    //Plugin will save and recall state information but unable to display to dials. Avenue for future improvement.

    juce::MemoryOutputStream stream(destData, true);

    //Saves each of the four Delay Params to output stream

    stream.writeFloat(mDelayTime);
    stream.writeFloat(mDelayTime2);
    stream.writeFloat(mFeedback);
    stream.writeFloat(mWetDry);

    

    //stream.writeFloat(*treeState.getRawParameterValue("lpf"));

    //^This was attempted for the Raw Parameter Values, unsuccessfully
}

void TableTennisAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);

    // The Values are recalled, but since they are just the float variables created to hold the values,
    // the dials return to their set initial values on startup
    //Was unable to use pointers to value tree params.


	mDelayTime = stream.readFloat();
    mDelayTime2 = stream.readFloat();
    mFeedback = stream.readFloat();
    mWetDry = stream.readFloat();


    //*treeState.getRawParameterValue("lpf") = stream.readFloat();

   //^This was attempted for the Raw Parameter Values, unsuccessfully


}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TableTennisAudioProcessor();
}


void TableTennisAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)

//Function detects when parameters are changed and which ones are changed
//sends the new values to the variables set up in the Processor.h
{

    if (parameterID == "delayTime") // If a change in delayTime param is detected

    {
        mDelayTime = (newValue * 44.1);
        //Assigns newValue to mDelayTime variable, *44.1 converts mS into Samples
        mDelayLineL.setDelay(newValue * 44.1);
        //newValue sets the DT1

        mDelayTime2 = (newValue * (44.1 * 0.79));
        mDelayLineR.setDelay(newValue * (44.1 * 0.79));
        //DT2 is related to DT by a factor of 0.79. This leads to a "trippy" feel from the delay, but somewhat musical,
        //due to the two taps' mathematical relationship
        //To create different effects this factor could be changed with a selection box, adding additional functionality
        //this could be explored in future iterations of the plugin
        
    }

    else if (parameterID == "feedback") // If a change to feedback param is detected
    {
        mFeedback = newValue;
        //mFeedbackvalue is changed to newValue
    }

    else if (parameterID == "wetDry") //If a change to wetDry param is detected
    {
        mWetDry = newValue;
        //m_wet_dry is changed to newValue
    }
  
}


