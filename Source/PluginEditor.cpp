/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
HarmonicExciterAudioProcessorEditor::HarmonicExciterAudioProcessorEditor (HarmonicExciterAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
  setSize(490, 220);
  
//  Gain slider
  gainSlider.addListener(this);
  gainSlider.setSliderStyle(Slider::Rotary);
  gainSlider.setBounds(360, 60, 120, 120);
  gainSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 80, 32);
  gainSlider.setRange(-48.0, 12.0);
  gainSlider.setValue(0.0);
  gainSlider.setName("Gain");
  addAndMakeVisible(gainSlider);
  
//  Harmonic Slider
  harmonicsSlider.addListener(this);
  harmonicsSlider.setSliderStyle(Slider::LinearBar);
  harmonicsSlider.setBounds(30, 60, 320, 32);
  harmonicsSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 24);
  harmonicsSlider.setRange(1, 7, 1);
  harmonicsSlider.setValue(3);
  harmonicsSlider.setName("Harmonics");
  addAndMakeVisible(harmonicsSlider);
  
//  Randomness Slider
  harmonicVarianceSlider.addListener(this);
  harmonicVarianceSlider.setSliderStyle(Slider::LinearBar);
  harmonicVarianceSlider.setBounds(30, 130, 320, 32);
  harmonicVarianceSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 24);
  harmonicVarianceSlider.setRange(0.0, 1.0);
  harmonicVarianceSlider.setValue(0.1);
  harmonicVarianceSlider.setName("Harmonic Variance");
  addAndMakeVisible(harmonicVarianceSlider);
  
// Use Fundamental for harmonics
  useFundamentalButton.addListener(this);
  useFundamentalButton.setName("Use Fundamental for Harmonics");
  useFundamentalButton.setBounds(30, 180, 80, 24);
  addAndMakeVisible(useFundamentalButton);

  
  startTimer(60);
}

HarmonicExciterAudioProcessorEditor::~HarmonicExciterAudioProcessorEditor()
{
}

//==============================================================================
void HarmonicExciterAudioProcessorEditor::paint (Graphics& g)
{
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

// Title
  g.setColour(Colours::white);
  g.setFont(18.0f);
  g.drawFittedText("Harmonic Exciter", 0,15,500,24, Justification::centred, 1);
  
//  Slider Labels
  g.setColour(Colours::slategrey);
  g.setFont(14.0f);
  g.drawFittedText("Harmonics", 30,40,200,16, Justification::left, 1);
  g.drawFittedText("Harmonic Variance", 30,110,200,16, Justification::left, 1);
  
//  Button Label
  g.drawFittedText("Use Fundamental for Harmonics", 60,185,200,16, Justification::left, 1);
  
}

void HarmonicExciterAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
}

void HarmonicExciterAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
  
  if (slider == &gainSlider) {
    *processor.gain = slider->getValue();
  }
  
  if (slider == &harmonicsSlider) {
    *processor.harmonicsCount = (int) slider->getValue();
  }
  
  if (slider == &harmonicVarianceSlider) {
    *processor.harmonicVariance = slider->getValue();
  }

}

void HarmonicExciterAudioProcessorEditor::buttonClicked(juce::Button *button) {
  
  if (button == &useFundamentalButton) {
    *processor.useOnlyFundamental = button->getToggleStateValue() == juce::Value(true);
  }
  
}

void HarmonicExciterAudioProcessorEditor::timerCallback() {
  
  if (gainSlider.getValue() != processor.gain->get()) {
    gainSlider.setValue(processor.gain->get());
  }
  
  if (harmonicsSlider.getValue() != *processor.harmonicsCount) {
    harmonicsSlider.setValue(*processor.harmonicsCount);
  }
  
  if (harmonicVarianceSlider.getValue() != *processor.harmonicVariance) {
    harmonicVarianceSlider.setValue(*processor.harmonicVariance);
  }

  bool state = useFundamentalButton.getToggleStateValue() == juce::Value(true);
  if (state != *processor.useOnlyFundamental) {
    useFundamentalButton.setToggleState(*processor.useOnlyFundamental, juce::sendNotification);
  }

}













