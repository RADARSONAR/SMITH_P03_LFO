/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "filterCalc/FilterCalc.h"
#include "STK/Delay.h"
#include "STK/BiQuad.h"
#include "STK/basicLFO.h"

//==============================================================================
/**
*/
class SMITH_P03_LFOAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SMITH_P03_LFOAudioProcessor();
    ~SMITH_P03_LFOAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    
    juce::AudioParameterFloat* DryWetUParam;
    juce::AudioParameterFloat* FeedbackUParam;
    juce::AudioParameterFloat* DelayTimeUParam;
    juce::AudioParameterFloat* SpeedUParam;  //same as LFORate in Tremolo Excersize?
    juce::AudioParameterFloat* IntensityUParam;  //Currently Not Taken into Account. I think this is just an additional gain control. Based my Tremolo off of my Fender Twin Reverb Settings
    juce::AudioParameterFloat* HPUParam;
    juce::AudioParameterFloat* LPUParam;
    juce::AudioParameterFloat* DepthUParam;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SMITH_P03_LFOAudioProcessor)
    //Ginas
    float fs; //sampling rate
    float fbGain;
    float dryGain;
    float wetGain;
    float finalGain;
    
    // AParams
    float DepthAParam;
    float HPAParam;
    float LPAParam;
    float DelayAParam;
    float SpeedAParam;
    float FeedbackAParam;
    float DryWetAParam;
    
    
    float prevDepthValue;
    float prevDelayTimeValue;
    float prevSpeedValue;
    float prevHPValue;
    float prevLPValue;
    float prevDryWetValue;
    float prevFeedbackValue;
    
    //Delay Things
    float DelayInput;
    float DelayOutput;
    float DelaySamps;
    
    // LFO things
    float LFODepthAParam;
    float lfoDepth;
    float lfoAmp;
    float DelayTime;
    
    //function and imports
    basicLFO LFO;
    stk::Delay DelayL, DelayR;
    stk::BiQuad HPL, HPR, LPL, LPR;
    float calcDelaySamps(float msec);
    void calcAlgorithimParams();
    
    
    // Interpolation stuff
    float linInterpolation(float start, float end, float fract);
    
};
