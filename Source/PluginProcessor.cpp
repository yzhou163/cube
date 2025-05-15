/*
  ==============================================================================

 //PluginProcessor.c

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
         #if ! JucePlugin_IsSynth
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
         #endif
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
        #endif
      ),
#else
    : AudioProcessor (BusesProperties()),
#endif
      apvts (*this, nullptr, "Parameters", createParameters())
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}


void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void NewProjectAudioProcessor::releaseResources()
{
}




#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
   
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

float NewProjectAudioProcessor::getCurrentSampleRate()
{
    return getSampleRate();
}
///SetValuesSafe

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gain",
                                                                 "Gain",
                                                                 0.0f,
                                                                 0.7f,
                                                                 0.5f));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("overdrive",
                                                                 "Overdrive",
                                                                 0.1f,
                                                                 10.0f,
                                                                 0.1f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("targetSR",
                                                                 "TargetSR",
                                                                 0.0f,
                                                                 0.6f,
                                                                 0.6f));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("carrier",
                                                                 "Carrier",
                                                                 20.0f,
                                                                 2000.0f,
                                                                 1000.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ringWave",
                                                                 "RingWave",
                                                                 0.0,
                                                                 1.0f,
                                                                 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ringModAmount",
                                                                 "ringModAmount",
                                                                 0.0,
                                                                 1.0f,
                                                                 0.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverb",
                                                                 "Reverb",
                                                                 0.0,
                                                                 1.0f,
                                                                 0.0f));
    
    
    

    return { params.begin(), params.end() };
}


void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    // Clean unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear (i, 0, buffer.getNumSamples());
 
    
    
    
    //==========================================Reverb
                                  
                   juce::Reverb::Parameters params;
                       params.roomSize = 0.6f;
                       params.damping = 0.5f;
                   params.wetLevel = reverbwet;
    params.dryLevel = 1.0f - reverbwet;
                       params.width = 1.0f;
                       params.freezeMode = 0.0f;
                       reverb.setParameters(params);

    
    
    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    
    
    
    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
       
        float* channelData = buffer.getWritePointer(channel);
    }
        
        
    //Ring Mod Frequency Slider
    float sampleRate = getSampleRate();
        float carrierIncrement = (carrier * 2.0f * juce::MathConstants<float>::pi) / sampleRate;


    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
       {
           auto* channelData = buffer.getWritePointer(channel);

           for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
           {
               float inputSample = channelData[sample];

               
           
//==========================================  Overdrive
            
               
               float distortedSample = std::tanh(inputSample * overdrive);
               
               distortedSample = distortedSample/(overdrive/2.f);

//==========================================Bitcrusher
               counter++;
               if (counter % countTo == 0)
               {
                   counter = 0;
                   storedBitCrushVal = distortedSample;
                   countTo = static_cast<int>(sampleRate / targetSR);
                   if (countTo<1){countTo=1;}
               }

               float crushedSample = storedBitCrushVal * 0.7;

//==========================================Ring Modulation
               
//SinCarrier

               carrierPhaseSine += carrierIncrement;
               if (carrierPhaseSine >= juce::MathConstants<float>::twoPi)
                   carrierPhaseSine -= juce::MathConstants<float>::twoPi;
               float carrierSignalSine = std::sin(carrierPhaseSine);
               
//SawToothCarrier
               carrierPhaseSawtooth += carrierIncrement;
                          if (carrierPhaseSawtooth >= 1.0f)
                              carrierPhaseSawtooth -= 1.0f;
                              
             float sawtoothWave = 2.0f * carrierPhaseSawtooth - 1.0f;
               
//modulated sample  for sine & sawtooth
               float modulatedSineSample = crushedSample * carrierSignalSine;
               float modulatedSawtoothSample = crushedSample * sawtoothWave;
               
//  combine Modulated Sample: (Slider choose the ratio of Sine and Sawtooth wave combied)
            float modulatedSample = ((1- ringWave) * modulatedSineSample + (ringWave * modulatedSawtoothSample)) ;
               
               
//Output Ring Mod Mix dry/wet slider
               float outputSample = (1.0f - ringModAmount) * crushedSample + ringModAmount * modulatedSample;
               
               


               
//==========================================Gain output
                       channelData[sample] = outputSample * rawVolume;

              
           }
       }
   }


   
                
        

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

