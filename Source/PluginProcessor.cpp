/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include <complex.h>
#include <map>


#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
HarmonicExciterAudioProcessor::HarmonicExciterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
  
  addParameter(gain = new AudioParameterFloat("gainMultiplier", "Gain", -48.0, 12.0, 0.0));
  addParameter(harmonicsCount = new AudioParameterInt("harmonicsCount", "Harmonics", 1, 7, 3));
  addParameter(harmonicVariance = new AudioParameterFloat("harmonicVariance", "Harmonic Variance", 0.0, 1.0, 0.1));
  addParameter(useOnlyFundamental = new AudioParameterBool("useOnlyFundamental", "Only Use Fundamental", false));

}

HarmonicExciterAudioProcessor::~HarmonicExciterAudioProcessor()
{
  
}

//==============================================================================
const String HarmonicExciterAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool HarmonicExciterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool HarmonicExciterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool HarmonicExciterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double HarmonicExciterAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int HarmonicExciterAudioProcessor::getNumPrograms()
{
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
  // so this should be at least 1, even if you're not really implementing programs.
}

int HarmonicExciterAudioProcessor::getCurrentProgram()
{
  return 0;
}

void HarmonicExciterAudioProcessor::setCurrentProgram (int index)
{
}

const String HarmonicExciterAudioProcessor::getProgramName (int index)
{
  return {};
}

void HarmonicExciterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void HarmonicExciterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  
  order = log2(samplesPerBlock);
  
  fft = new dsp::FFT(order);
  
  currentGain = *gain;
}

void HarmonicExciterAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  //  std::free(&fft);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HarmonicExciterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  ignoreUnused (layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
      && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void HarmonicExciterAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  ScopedNoDenormals noDenormals;
  const int totalNumInputChannels  = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();
  
  const double sampFreq = HarmonicExciterAudioProcessor::getSampleRate();
  const int bufferSize = buffer.getNumSamples();
  
  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear (i, 0, buffer.getNumSamples());
  
  
  juce::AudioBuffer<float>* bufCopy = new juce::AudioBuffer<float>();
  bufCopy->makeCopyOf(buffer);
  
  
  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  for (int channel = 0; channel < totalNumInputChannels; ++channel)
  {
    
    // FFT operations needs 2 * numSamples, so change that here
    bufCopy->setSize(totalNumInputChannels, bufferSize * 2, true, true);
    float* freqData = bufCopy->getWritePointer(channel);
    

    // Get frequencies from buffer copy
    fft->performRealOnlyForwardTransform(freqData, true);

    // Find fundamental Freq
//    int freqCap = 1000;
    float freqBlockSize = (sampFreq / 2) / bufferSize;
//    int cutoffIndex = freqCap / freqBlockSize;

    int loudestSamp = 0;
    float loudestMag = 0.0;

    for (int s = 0; s < bufferSize; s++) {
      std::complex<float>* c = reinterpret_cast<std::complex<float>*>(freqData) + s;
      
      float amp = sqrt(square(c->real()) + square(c->imag()));
     
      if (loudestMag < amp) {
        loudestMag = amp;
        loudestSamp = s;
      }
    }
    

    int fundamentalFreq = loudestSamp * freqBlockSize * 2;
    
    // shift frequency elements over to fit new frequency
    for (int i = sizeof(frequencyHistory) - 2; i > 0; i--)
      frequencyHistory[i] = frequencyHistory[i-1];
    
    frequencyHistory[0] = fundamentalFreq;
    

    int carryingFreq = HarmonicExciterAudioProcessor::getHighestOccuringFrequnecy();
    
    if (carryingFreq != 0) {
      currentFundamental = carryingFreq;
    }
    
    for (int s = 0; s < bufferSize; s++) {
      int f = s * freqBlockSize;
      if (f == currentFundamental) {
        currentFundamentalIndex = s;
        break;
      }
    }
    

    // If only using fundamental, level all other frequencies
    if (useOnlyFundamental) {
      for (int s = 0; s < currentFundamentalIndex; s++) {
        std::complex<float>* c = reinterpret_cast<std::complex<float>*>(freqData) + s;
        c->real(0);
        c->imag(0);
      }
    }
    
    // level frequencies above fundamental
    for (int s = currentFundamentalIndex + 1; s < bufferSize; s++) {
      std::complex<float>* c = reinterpret_cast<std::complex<float>*>(freqData) + s;
      c->real(0);
      c->imag(0);
    }
    
    // back to time domain
    fft->performRealOnlyInverseTransform(freqData);
    bufCopy->setSize(totalNumInputChannels, bufferSize, true);


    // Perform distortion
    for (int samp = 0; samp < bufferSize; samp++) {
      float currentSamp = bufCopy->getSample(channel, samp);

      // number of samples to smooth by on each end
      int smoothRange = 32;
      float smoothOffset = 1.0f;
     
      int outCount = samp;
      if (samp > (bufferSize - smoothRange)) {
        outCount = bufferSize - samp;
      }
     
      if (outCount < smoothRange) {
        float d = (float)outCount / ((float)smoothRange / 5.0f);
        smoothOffset = atanf(d);
      }

      float distort = 0;
      for (int h = 1; h <= *harmonicsCount; h += 2) {
//        float drive = std::log((float) h);
        distort += (currentSamp - pow(currentSamp, h) / h);
      }
      
      // rand number between -0.5 and 0.5
      float r = ((rand() % 1024) / 1024.0f) - 0.5;
      float v = *harmonicVariance / 100.0f;

      int polarity = currentSamp >= 0 ? 1 : -1;

      float distorted = polarity * smoothOffset * distort * (1 + (r * v));

      bufCopy->setSample(channel, samp, distorted);
    }

  // Add distorted source
  const float* distortedSource = bufCopy->getReadPointer(channel);
  buffer.addFrom(channel, 0, distortedSource, bufferSize);
      

  }
  
  // Add gain gradually
  double gainAdjusted = pow(10, *gain / (double) 20);
  buffer.applyGainRamp(0, bufferSize, currentGain, gainAdjusted);
  currentGain = gainAdjusted;
  
}


//==============================================================================
bool HarmonicExciterAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HarmonicExciterAudioProcessor::createEditor()
{
  return new HarmonicExciterAudioProcessorEditor (*this);
}

//==============================================================================
void HarmonicExciterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  ScopedPointer<XmlElement> xml (new XmlElement ("com.coreysery.HarmonicExiciter.params"));
  
  xml->setAttribute("gain", (float) *gain);
  xml->setAttribute("harmonicsCount", (int) *harmonicsCount);
  xml->setAttribute("harmonicVariance", (float) *harmonicVariance);
  xml->setAttribute("useOnlyFundamental", (bool) *useOnlyFundamental);

  copyXmlToBinary(*xml, destData);
}

void HarmonicExciterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
  ScopedPointer<XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
  
  if (xmlState != nullptr && xmlState->hasTagName("com.coreysery.HarmonicExiciter.params")) {
    *gain = xmlState->getDoubleAttribute("gain");
    *harmonicsCount = xmlState->getIntAttribute("harmonicsCount");
    *harmonicVariance = xmlState->getDoubleAttribute("harmonicVariance");
    *useOnlyFundamental = xmlState->getBoolAttribute("useOnlyFundamental");
  }
}

int HarmonicExciterAudioProcessor::getHighestOccuringFrequnecy()
{
  std::map<int, int> mydict = {};
  int cnt = 0;
  int freq = 0;
  
  for (auto&& item : frequencyHistory) {
    
    if (item == 0) continue;
    
    mydict[item] = mydict.emplace(item, 0).first->second + 1;
    if (mydict[item] >= cnt) {
      std::tie(cnt, freq) = std::tie(mydict[item], item);
    }
  }
  
  return freq;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new HarmonicExciterAudioProcessor();
}













