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
class SMITH_P03_LFOAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Timer
{
public:
    SMITH_P03_LFOAudioProcessorEditor (SMITH_P03_LFOAudioProcessor&);
    ~SMITH_P03_LFOAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider*) override;
    void timerCallback() override;

private:
    juce::Slider HP;
    juce::Label HPlabel;
    
    juce::Slider LP;
    juce::Label LPlabel;
    
    juce::Slider DryWet;
    juce::Label DryWetlabel;
    
    juce::Slider Speed;
    juce::Label Speedlabel;
    
    juce::Slider Delay;
    juce::Label Delaylabel;
    
    juce::Slider Depth;
    juce::Label Depthlabel;
    
    juce::Slider Feedback;
    juce::Label Feedbacklabel;
    
    juce::Slider Intensity;
    juce::Label Intensitylabel;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SMITH_P03_LFOAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SMITH_P03_LFOAudioProcessorEditor)
};
