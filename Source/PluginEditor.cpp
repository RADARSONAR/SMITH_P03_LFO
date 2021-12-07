/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SMITH_P03_LFOAudioProcessorEditor::SMITH_P03_LFOAudioProcessorEditor (SMITH_P03_LFOAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    HP.setRange(20.0, 7000.0, 1.0);
    HP.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    HP.setTextValueSuffix("Hz");
    HP.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    HP.addListener(this);
    addAndMakeVisible(HP);
    
    HPlabel.setText("High Pass", juce::dontSendNotification);
    HPlabel.attachToComponent(&HP, false);
    HPlabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(HPlabel);
    
    LP.setRange(250.0, 15000.0, 1.0);
    LP.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    LP.setTextValueSuffix("Hz");
    LP.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    LP.addListener(this);
    addAndMakeVisible(LP);
    
    LPlabel.setText("Low Pass", juce::dontSendNotification);
    LPlabel.attachToComponent(&LP, false);
    LPlabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(LPlabel);
    
    Delay.setRange(0.1, 10.0, 1.0);
    Delay.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    Delay.setTextValueSuffix("ms");
    Delay.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    Delay.addListener(this);
    addAndMakeVisible(Delay);
    
    Delaylabel.setText("Delay Time", juce::dontSendNotification);
    Delaylabel.attachToComponent(&Delay, false);
    Delaylabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(Delaylabel);
    
    Speed.setRange(0.1, 10.0, 0.01);
    Speed.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    Speed.setTextValueSuffix("Hz");
    Speed.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    Speed.addListener(this);
    addAndMakeVisible(Speed);
    
    Speedlabel.setText("LFO Speed", juce::dontSendNotification);
    Speedlabel.attachToComponent(&Speed, false);
    Speedlabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(Speedlabel);
    
    Depth.setRange(0.0, 100.0, 1.0);
    Depth.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    Depth.setTextValueSuffix("%");
    Depth.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    Depth.addListener(this);
    addAndMakeVisible(Depth);
    
    Depthlabel.setText("Depth", juce::dontSendNotification);
    Depthlabel.attachToComponent(&Depth, false);
    Depthlabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(Depthlabel);
    
    Feedback.setRange(0.0, 100.0, 1.0);
    Feedback.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    Feedback.setTextValueSuffix(" %");
    Feedback.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    Feedback.addListener(this);
    addAndMakeVisible(Feedback);
    
    Feedbacklabel.setText("Feedback", juce::dontSendNotification);
    Feedbacklabel.attachToComponent(&Feedback, false);
    Feedbacklabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(Feedbacklabel);
    
    //Intensity.setRange(0.0, 10.0, 1.0);
    
    DryWet.setRange(0.0, 100.0, 1.0);
    DryWet.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    DryWet.setTextValueSuffix("%");
    DryWet.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    DryWet.addListener(this);
    addAndMakeVisible(DryWet);
    
    DryWetlabel.setText("Dry to Wet", juce::dontSendNotification);
    DryWetlabel.attachToComponent(&DryWet, false);
    DryWetlabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(DryWetlabel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);
    startTimer (100);
}

SMITH_P03_LFOAudioProcessorEditor::~SMITH_P03_LFOAudioProcessorEditor()
{
}

//==============================================================================
void SMITH_P03_LFOAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Flanger and Tremolo share an LFO", getLocalBounds(), juce::Justification::centredTop, 1);
}

void SMITH_P03_LFOAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float y = 150;
    float xDel = 50;
    float xDep = 100;
    float xFeed = 150;
    float xSpeed = 200;
    float xHP = 250;
    float xLP = 300;
    float xDW = 350;
    
    Delay.setBounds(xDel, y, 50, 50);
    Depth.setBounds(xDep, y, 50, 50);
    Feedback.setBounds(xFeed, y, 50, 50);
    Speed.setBounds(xSpeed, y, 50, 50);
    HP.setBounds(xHP, y, 50, 50);
    LP.setBounds(xLP, y, 50, 50);
    DryWet.setBounds(xDW, y, 50, 50);
}

void SMITH_P03_LFOAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &Feedback) {
        audioProcessor.FeedbackUParam->beginChangeGesture();
        
        *audioProcessor.FeedbackUParam = (float) Feedback.getValue();
        audioProcessor.FeedbackUParam->endChangeGesture();
    }
    
    else if (slider == &DryWet) {
        audioProcessor.DryWetUParam->beginChangeGesture();
        
        *audioProcessor.DryWetUParam = (float) DryWet.getValue();
        audioProcessor.DryWetUParam->endChangeGesture();
    }
    
    else if (slider == &HP) {
        audioProcessor.HPUParam->beginChangeGesture();
        
        *audioProcessor.HPUParam = (float) HP.getValue();
        audioProcessor.HPUParam->endChangeGesture();
    }
    
    else if (slider == &LP) {
        audioProcessor.LPUParam->beginChangeGesture();
        
        *audioProcessor.LPUParam = (float) LP.getValue();
        audioProcessor.LPUParam->endChangeGesture();
    }
    
    else if (slider == &Depth) {
        audioProcessor.DepthUParam->beginChangeGesture();
        
        *audioProcessor.DepthUParam = (float) Depth.getValue();
        audioProcessor.DepthUParam->endChangeGesture();
    }
    
    else if (slider == &Speed) {
        audioProcessor.SpeedUParam->beginChangeGesture();
        
        *audioProcessor.SpeedUParam = (float) Speed.getValue();
        audioProcessor.SpeedUParam->endChangeGesture();
    }
    
    else if (slider == &Delay) {
        audioProcessor.DelayTimeUParam->beginChangeGesture();
        
        *audioProcessor.DelayTimeUParam = (float) Delay.getValue();
        audioProcessor.DelayTimeUParam->endChangeGesture();
    }
}

void SMITH_P03_LFOAudioProcessorEditor::timerCallback()
{
    Feedback.setValue(*audioProcessor.FeedbackUParam, juce::dontSendNotification);
    Delay.setValue(*audioProcessor.DelayTimeUParam, juce::dontSendNotification);
    DryWet.setValue(*audioProcessor.DryWetUParam, juce::dontSendNotification);
    HP.setValue(*audioProcessor.HPUParam, juce::dontSendNotification);
    LP.setValue(*audioProcessor.LPUParam, juce::dontSendNotification);
    Depth.setValue(*audioProcessor.DepthUParam, juce::dontSendNotification);
    Speed.setValue(*audioProcessor.SpeedUParam, juce::dontSendNotification);
}
