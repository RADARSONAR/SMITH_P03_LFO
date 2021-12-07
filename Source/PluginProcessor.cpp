/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SMITH_P03_LFOAudioProcessor::SMITH_P03_LFOAudioProcessor()
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
    addParameter(FeedbackUParam = new juce::AudioParameterFloat (
                                "Feedback",
                                "Feedback",
                                juce::NormalisableRange<float> (0.0f, 100.0f),
                                (50.0f)));
    
    addParameter(DelayTimeUParam = new juce::AudioParameterFloat (
                                "Time",
                                "Time",
                                juce::NormalisableRange<float> (0.0f, 10.0f),
                                (5.0f)));
    
    addParameter(DryWetUParam = new juce::AudioParameterFloat (
                                "DryWet",
                                "DryWet",
                                juce::NormalisableRange<float> (0.0f, 100.0f),
                                (50.0f)));
    
    addParameter(HPUParam = new juce::AudioParameterFloat (
            "HP",
            "HP",
            juce::NormalisableRange<float> (20.0f, 7000.0f),
            (20.0f)));
    
    addParameter(LPUParam = new juce::AudioParameterFloat (
            "LP",
            "LP",
            juce::NormalisableRange<float> (250.0f, 15000.0f),
            (15000.0f)));
    
    addParameter (SpeedUParam = new juce::AudioParameterFloat (
                    "Spped", // parameterID
                    "Speed", // parameter name
                    juce::NormalisableRange<float> (0.1f, 20.0f), //parameter range
                    1.0f)); // default value
        
    addParameter (DepthUParam = new juce::AudioParameterFloat (
                            "Depth", // parameterID
                            "Depth", // parameter name
                            juce::NormalisableRange<float> (0.0f, 100.0f), //parameter range
                            50.0f)); // default value
    
    addParameter (IntensityUParam = new juce::AudioParameterFloat (
                            "Intensity", // parameterID
                            "Intensity", // parameter name
                            juce::NormalisableRange<float> (0.0f, 100.0f), //parameter range
                            10.0f)); // default value
    
}

SMITH_P03_LFOAudioProcessor::~SMITH_P03_LFOAudioProcessor()
{
}

//==============================================================================
const juce::String SMITH_P03_LFOAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SMITH_P03_LFOAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SMITH_P03_LFOAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SMITH_P03_LFOAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SMITH_P03_LFOAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SMITH_P03_LFOAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SMITH_P03_LFOAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SMITH_P03_LFOAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SMITH_P03_LFOAudioProcessor::getProgramName (int index)
{
    return {};
}

void SMITH_P03_LFOAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SMITH_P03_LFOAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    fs = sampleRate;
    
    LFO.setFreq(*SpeedUParam, fs);
    LFO.resetPhase();
    DepthAParam = *DepthUParam * 0.01; //Converting from 0-100%
    
    HPAParam = *HPUParam;
    LPAParam = *LPUParam;
    
    DelayAParam = *DelayTimeUParam;
    DelaySamps = calcDelaySamps(DelayTimeUParam->get());
    DelayL.setDelay(DelaySamps);
    DelayR.setDelay(DelaySamps);
    
    
    SpeedAParam = *SpeedUParam;
    FeedbackAParam = *FeedbackUParam;
    DryWetAParam = *DryWetUParam;
    IntensityAParam = *IntensityUParam;
}

void SMITH_P03_LFOAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SMITH_P03_LFOAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//============================

float SMITH_P03_LFOAudioProcessor::linInterpolation(float start, float end, float fract)
{
    return start + (fract * (end - start));
}

float SMITH_P03_LFOAudioProcessor::calcDelaySamps(float msec)
{
    return fs * (msec/1000);
}

void SMITH_P03_LFOAudioProcessor::calcAlgorithimParams()

{
    SpeedAParam = *SpeedUParam;
    
    
    dryGain = 1 - (*DryWetUParam * 0.01);
    wetGain = *DryWetUParam * 0.01;
    fbGain = *FeedbackUParam * 0.01;
    
    tremoloGain = *IntensityUParam * 0.01;
    
    
    HPAParam = *HPUParam;
    LPAParam = *LPUParam;
    float HPcoeffs[5];          // an array of 5 floats for filter coeffs
    float LPcoeffs[5];
    float HPfc = HPAParam;      // cutoff freq. based upon user
    float LPfc = LPAParam;
    float Q = 10;
    
    FilterCalc::calcCoeffsHPF(HPcoeffs, HPfc, Q, fs);
    FilterCalc::calcCoeffsLPF(LPcoeffs, LPfc, Q, fs);
    
    HPL.setCoefficients(HPcoeffs[0], HPcoeffs[1], HPcoeffs[2], HPcoeffs[3], HPcoeffs[4]);
    HPR.setCoefficients(HPcoeffs[0], HPcoeffs[1], HPcoeffs[2], HPcoeffs[3], HPcoeffs[4]);
    LPL.setCoefficients(LPcoeffs[0], LPcoeffs[1], LPcoeffs[2], LPcoeffs[3], LPcoeffs[4]);
    LPR.setCoefficients(LPcoeffs[0], LPcoeffs[1], LPcoeffs[2], LPcoeffs[3], LPcoeffs[4]);
    
    DelayAParam = *DelayTimeUParam;
    DelaySamps = calcDelaySamps(DelayTimeUParam->get()); //wait to set delays
    
    LFO.setFreq(SpeedUParam->get(), fs);
    float N = 1024; //N is the amount of swing delay
    lfoDepth = (*DepthUParam * 0.01) * N;
    
    DelayL.setDelay(DelaySamps);
    DelayR.setDelay(DelaySamps);
    
    
}

//==========================

void SMITH_P03_LFOAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    calcAlgorithimParams();
    const int numSamps = buffer.getNumSamples();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        for (int i = 0; i < numSamps; ++i)
        {
            //LFO first
            lfoAmp = LFO.tick(); //value between -1 and 1. BIPOLAR LFO
            lfoAmp = lfoAmp * DepthAParam;
            //interpolation of one control
            float fract = ((float) i / (float) numSamps); //creates 0-1 ramp across the loop (linear interpolation)
            if (DepthAParam != prevDepthValue) {
                DepthAParam = linInterpolation(prevDepthValue, DepthAParam, fract);
            }
            if (DelayAParam != prevDelayTimeValue) {
                DelayAParam = linInterpolation(prevDelayTimeValue, DelayAParam, fract);
            }
            if (SpeedAParam != prevSpeedValue) {
                SpeedAParam = linInterpolation(prevSpeedValue, SpeedAParam, fract);
            }
            if (HPAParam != prevHPValue) {
                HPAParam = linInterpolation(prevHPValue, HPAParam, fract);
            }
            if (LPAParam != prevLPValue) {
                LPAParam = linInterpolation(prevLPValue, LPAParam, fract);
            }
            if (DryWetAParam != prevDryWetValue) {
                DryWetAParam = linInterpolation(prevDryWetValue, DryWetAParam, fract);
            }
            if (FeedbackAParam != prevFeedbackValue) {
                FeedbackAParam = linInterpolation(prevFeedbackValue, FeedbackAParam, fract);
            }
            //Interpolation over
            
            /*
             from Delay Project
            if (channel == 0) { //left channel 
                float delayInput = (ProjDelayL.nextOut() * fbGain) + channelData[i];
                float delayOutput = ProjDelayL.tick(delayInput);
                channelData[i] =  delayOutput * wetGain + channelData[i] * dryGain;
            }
            else if (channel == 1) { //right channel 
                float delayInput = (ProjDelayR.nextOut() * fbGain) + channelData[i];
                float delayOutput = ProjDelayR.tick(delayInput);
                channelData[i] =  delayOutput * wetGain + channelData[i] * dryGain;
            */
            
            /*
             from HP Filter Project
             if (channel == 0) { //left channel 
             channelData[i] = filterL.tick(channelData[i]);
             }
             else if (channel == 1) { //right channel 
             channelData[i] = filterR.tick(channelData[i]);
             }
             */
            //Tremolo
            
             tremLFO = lfoAmp * tremoloGain;
             
             //Need to use lfo to affect wet gain
             
            
            
            
            
            // Flanger. No Tremolo
            DelayTime = DelaySamps + (lfoDepth * lfoAmp);
            DelayL.setDelay(DelayTime);
            DelayR.setDelay(DelayTime);
            
            if (channel == 0) //Left
            {
                DelayOutput = wetGain * DelayL.tick(DelayL.nextOut() * fbGain + channelData[i]);
                channelData[i] = LPL.tick(HPL.tick(DelayOutput)) * wetGain + channelData[i] * dryGain;
            }
            if (channel == 1)
            {
                DelayOutput = wetGain * DelayR.tick(DelayR.nextOut() * fbGain + channelData[i]);
                channelData[i] = LPL.tick(HPR.tick(DelayOutput)) * wetGain + channelData[i] * dryGain;
            }
            
            
            
            
            /*
            fctmp = fc + ((fc * 0.5) * lfoAmp); //portion of fc * -1to1 added to fc
            mfL.setCutoff(fctmp); //left side
            mfL.setRes(res);
            mfL.calc();
                
            mfR.setCutoff(fctmp); //right side
            mfR.setRes(res);
            mfR.calc();
            //process samples. no need to check channel data. just specify which array you are using.
            leftChannel[i] = mfL.process(leftChannel[i]);   // filter it
            rightChannel[i] = mfR.process(rightChannel[i]);    // filter it
             */
                
        }
        prevDepthValue = DepthAParam; //after loop, set prev to current so we only interpolate once. otherwise clicks
        prevDelayTimeValue = DelayAParam;
        prevSpeedValue = SpeedAParam;
        prevHPValue = HPAParam;
        prevLPValue = LPAParam;
        prevDryWetValue = *DryWetUParam;
        prevFeedbackValue = *FeedbackUParam;
    }
}

//==============================================================================
bool SMITH_P03_LFOAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SMITH_P03_LFOAudioProcessor::createEditor()
{
    return new SMITH_P03_LFOAudioProcessorEditor (*this);
}

//==============================================================================
void SMITH_P03_LFOAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SMITH_P03_LFOAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SMITH_P03_LFOAudioProcessor();
}
