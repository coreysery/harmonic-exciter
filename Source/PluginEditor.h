/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
 */
class HarmonicExciterAudioProcessorEditor  : public AudioProcessorEditor,
public Slider::Listener,
public ToggleButton::Listener,
public Timer
{
public:
  HarmonicExciterAudioProcessorEditor (HarmonicExciterAudioProcessor&);
  ~HarmonicExciterAudioProcessorEditor();
  
  //==============================================================================
  void paint (Graphics&) override;
  void resized() override;
  

  void sliderValueChanged(Slider* slider) override;
  void buttonClicked(Button* button) override;
  
  void timerCallback() override;

  
private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  HarmonicExciterAudioProcessor& processor;
  
  Slider gainSlider;
  Slider harmonicsSlider;
  Slider harmonicVarianceSlider;
  
  ToggleButton useFundamentalButton;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicExciterAudioProcessorEditor)
};
