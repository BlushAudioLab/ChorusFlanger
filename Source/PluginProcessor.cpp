/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusFlangerAudioProcessor::ChorusFlangerAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)

                       .withOutput ("Output", AudioChannelSet::stereo(), true))
{
    addParameter(mDryWetParameter = new AudioParameterFloat("drywet", "Dry Wet", 0.0, 1.0, 0.5));
    
    addParameter(mDepthParameter = new AudioParameterFloat("depth", "Depth", 0.0, 1.0, 0.5));
    
    addParameter(mRateParameter = new AudioParameterFloat("rate", "Rate", 0.1f, 20.f, 10.f));
    
    addParameter(mPhaseOffsetParameter = new AudioParameterFloat("phaseOffset", "Phase Offset", 0.0, 1.f, 0.f));
    
    addParameter(mFeedbackParameter = new AudioParameterFloat("feedback", "Feedback Amount", 0.0, 0.98, 0.5));
    
    addParameter(mTypeParameter = new AudioParameterInt("type", "Type", 0, 1, 0));
    
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0.;
    mCircularBufferLength = 0.;
    
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    
    mLFOPhase = 0;
    
    mDryWetSmoothed = 0;
    mDepthSmoothed = 0;
    mPhaseOffsetSmoothed = 0;
    mTypeParamterSmoothed = 0;
    
    

}


ChorusFlangerAudioProcessor::~ChorusFlangerAudioProcessor()
{
    if (mCircularBufferLeft != nullptr){
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    
    if (mCircularBufferRight != nullptr){
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
        
    }
    
}

//==============================================================================
const String ChorusFlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusFlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusFlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusFlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusFlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusFlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusFlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusFlangerAudioProcessor::setCurrentProgram (int index)
{
}

const String ChorusFlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusFlangerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ChorusFlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    mLFOPhase = 0;
    
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
    
   
    
    if (mCircularBufferLeft == nullptr){
        mCircularBufferLeft = new float[mCircularBufferLength];
    }
        
    zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float)); //setting all the bytes to zero in left channel
    
    if (mCircularBufferRight == nullptr){
        mCircularBufferRight = new float[mCircularBufferLength];
    }
    
    zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float)); //setting all the bytes to zero in right channel
    
    mCircularBufferWriteHead = 0;
    
    
    mDepthSmoothed = *mDepthParameter;
    mTypeParamterSmoothed = *mTypeParameter;
    
}

void ChorusFlangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusFlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() == AudioChannelSet::stereo() &&
        layouts.getMainOutputChannelSet() == AudioChannelSet::stereo())
{
        return true;
    }
        else {
                  return false;
                   }
}
#endif

void ChorusFlangerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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
    
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            
            
            /* Parameter Smoothing*/
            
            mDryWetSmoothed = mDryWetSmoothed - 0.001 * (mDryWetSmoothed - *mDryWetParameter);
            mDepthSmoothed = mDepthSmoothed - 0.001 * (mDepthSmoothed - *mDepthParameter);
            mPhaseOffsetSmoothed = mPhaseOffsetSmoothed - 0.001 * (mPhaseOffsetSmoothed - *mPhaseOffsetParameter);
            
            
            /* LFO Generation */
            
            //float lfoOutLeft = sin(2*M_PI * mLFOPhase); // calculate sine LFO waveform based on y=sin(2pi * x)
            float lfoOutLeft = (2 * (1/M_PI)) * asin(sin  ( (2*M_PI / 1) * mLFOPhase)) - 1;
            
            float lfoPhaseRight = mLFOPhase + mPhaseOffsetSmoothed;
                       
            if (lfoPhaseRight > 1){
                lfoPhaseRight -= 1;
            }
            
            //float lfoOutRight = sin(2*M_PI * lfoPhaseRight);
            
            float lfoOutRight = (2 * (1/M_PI)) * asin(sin  ( (2*M_PI / 1) * lfoPhaseRight)) - 1;
            
            
            /* Multiplying the LFO by the Depth Parameter*/
            
            lfoOutLeft *= (mDepthSmoothed / 2);
            lfoOutRight *= (mDepthSmoothed / 2);
            
            /* Chorus Effect Selection */
            
            float lfoOutMappedLeft = 0;
            float lfoOutMappedRight = 0;
            
            int mTypeParameterSmoothed = *mTypeParameter;
            
            mTypeParamterSmoothed = mTypeParamterSmoothed - 0.001 * (mTypeParamterSmoothed - *mTypeParameter);
            
            if (mTypeParameterSmoothed == 0){
                lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.005f, 0.03f);
                lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);
            }
            /* Flanger Effect Selection */
            else {
                lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.005f);
                lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
            }
            
            
            float delayTimeSamplesLeft = getSampleRate() * lfoOutMappedLeft;
            float delayTimeSamplesRight = getSampleRate() * lfoOutMappedRight;
            
            
            mLFOPhase += *mRateParameter / getSampleRate(); // moves the LFO phase forwards
                
            if (mLFOPhase > 1) {
                    mLFOPhase -= 1;
                } //this if statement wraps the LFO Phase between 0 and 1
            
            
            mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
            mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
            
            float delayReadHeadLeft = mCircularBufferWriteHead - delayTimeSamplesLeft;
            
            if (delayReadHeadLeft < 0){
                delayReadHeadLeft += mCircularBufferLength;
            }
            
            float delayReadHeadRight = mCircularBufferWriteHead - delayTimeSamplesRight;
            
            if (delayReadHeadRight < 0){
                delayReadHeadRight += mCircularBufferLength;
            }
            
            
            int readHeadLeft_x = (int)delayReadHeadLeft; //casting the read head value as an integer
            int readHeadLeft_x1 = readHeadLeft_x + 1; // getting the next index in the array for the readhead
            float readHeadFloatLeft = delayReadHeadLeft - readHeadLeft_x; // we are now subtracting the integer version of the read head from the original floating point version
            
            if (readHeadLeft_x1 >= mCircularBufferLength){ //making sure the next sample isn't at the end of the circ buffer
                 readHeadLeft_x1 -= mCircularBufferLength;
             }
            
            
            int readHeadRight_x = (int)delayReadHeadRight; //casting the read head value as an integer
            int readHeadRight_x1 = readHeadRight_x + 1; // getting the next index in the array for the readhead
            float readHeadFloatRight = delayReadHeadRight - readHeadRight_x; // we are now subtracting the integer version of the read head from the original floating point version
                       
            if (readHeadRight_x1 >= mCircularBufferLength){ //making sure the next sample isn't at the end of the circ buffer
                            readHeadRight_x1 -= mCircularBufferLength;
                        }
            
            
            float delay_sample_left = lin_interp(mCircularBufferLeft [readHeadLeft_x], mCircularBufferLeft [readHeadLeft_x1], readHeadFloatLeft);

            float delay_sample_right = lin_interp(mCircularBufferRight [readHeadRight_x], mCircularBufferRight [readHeadRight_x1], readHeadFloatRight);

            
            mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
            mFeedbackRight = delay_sample_right * *mFeedbackParameter;
            
            
            mCircularBufferWriteHead++;
            
            buffer.setSample(0, i, buffer.getSample(0, i) * (1 - mDryWetSmoothed) + delay_sample_left * mDryWetSmoothed);
            buffer.setSample(1, i, buffer.getSample(1, i) * (1 - mDryWetSmoothed) + delay_sample_right * mDryWetSmoothed);
            
            if (mCircularBufferWriteHead >= mCircularBufferLength) {
                mCircularBufferWriteHead = 0;
            }
        }
    }
//==============================================================================
bool ChorusFlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ChorusFlangerAudioProcessor::createEditor()
{
    return new ChorusFlangerAudioProcessorEditor (*this);
}

//==============================================================================
void ChorusFlangerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(new XmlElement("ChorusFlanger"));
    
    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("Depth", *mDepthParameter);
    xml->setAttribute("Rate", *mRateParameter);
    xml->setAttribute("PhaseOffset", *mPhaseOffsetParameter);
    xml->setAttribute("Type", *mTypeParameter);
    
    copyXmlToBinary(*xml, destData);
    
}

void ChorusFlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName("ChorusFlanger"))
    {
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");
        *mDepthParameter = xml->getDoubleAttribute("Depth");
        *mRateParameter = xml->getDoubleAttribute("Rate");
        *mPhaseOffsetParameter = xml->getDoubleAttribute("PhaseOffset");
        *mTypeParameter = xml->getIntAttribute("Type");
        
        
    }
    
    
    
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusFlangerAudioProcessor();
}

float ChorusFlangerAudioProcessor::lin_interp(float sample_x, float sample_x1, float inPhase)
{
  return (1 - inPhase) * sample_x + inPhase * sample_x1;
}
