/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"



//==============================================================================
/**
 */
class HarmonicExciterAudioProcessor  : public AudioProcessor
{
public:
  //==============================================================================
  HarmonicExciterAudioProcessor();
  ~HarmonicExciterAudioProcessor();
  
  //==============================================================================
  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  
#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
  
  void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
  
  //==============================================================================
  AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;
  
  //==============================================================================
  const String getName() const override;
  
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect () const override;
  double getTailLengthSeconds() const override;
  
  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const String getProgramName (int index) override;
  void changeProgramName (int index, const String& newName) override;
  
  //==============================================================================
  void getStateInformation (MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;
  
  //  Editor Variables
  AudioParameterFloat* gain;
  AudioParameterInt* harmonicsCount;
  AudioParameterFloat* harmonicVariance;
  AudioParameterBool* useOnlyFundamental;

  
private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicExciterAudioProcessor)
  
  int nyquist;
  int order;
  
  const dsp::FFT *fft;
  const float gainStep = 0.001;
  
  double currentGain = 0;
//  carry over fundamental
  int currentFundamental = 0;
  int currentFundamentalIndex = 0;

};








