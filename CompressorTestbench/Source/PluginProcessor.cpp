/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorTestbenchAudioProcessor::CompressorTestbenchAudioProcessor()
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
}

CompressorTestbenchAudioProcessor::~CompressorTestbenchAudioProcessor()
{
}

//==============================================================================
const juce::String CompressorTestbenchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CompressorTestbenchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CompressorTestbenchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CompressorTestbenchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CompressorTestbenchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CompressorTestbenchAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CompressorTestbenchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorTestbenchAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CompressorTestbenchAudioProcessor::getProgramName (int index)
{
    return {};
}

void CompressorTestbenchAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CompressorTestbenchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    /*zeros.setSize(juce::dsp::SIMDRegister<float>::size(), samplesPerBlock);
    zeros.clear();
    interleaved.setSize(1, samplesPerBlock);*/
    ffvcaTrad.prepare(sampleRate, samplesPerBlock);
    ffvcaTPTz.prepare(sampleRate, samplesPerBlock);
    ffvcaTPT.prepare(sampleRate, samplesPerBlock);
    
    ffvcaTradR.prepare(sampleRate, samplesPerBlock);
    ffvcaTPTzR.prepare(sampleRate, samplesPerBlock);
    ffvcaTPTR.prepare(sampleRate, samplesPerBlock);

    fbvcaTrad.prepare(sampleRate, samplesPerBlock);
    fbvcaTPTz.prepare(sampleRate, samplesPerBlock);

    fbvcaTradR.prepare(sampleRate, samplesPerBlock);
    fbvcaTPTzR.prepare(sampleRate, samplesPerBlock);

}

void CompressorTestbenchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompressorTestbenchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CompressorTestbenchAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    //clear extra buffer channels
    /*
   //get channel pointers
    for (size_t ch = 0; ch < juce::dsp::SIMDRegister<float>::size(); ++ch)
        inout[ch] = ch < totalNumInputChannels ? buffer.getWritePointer(ch) :
        zeros.getWritePointer(ch);
    //interleave channel data
    juce::AudioDataConverters::interleaveSamples(const_cast<float**> (inout),
        reinterpret_cast<float*> (interleaved.getWritePointer(0)),
        buffer.getNumSamples(),
        static_cast<int> (juce::dsp::SIMDRegister<float>::size()));
    //process
    */
    switch (currentCompressor)
    {
        case FF_TRAD:
            ffvcaTrad.process(buffer.getWritePointer(0));
            ffvcaTradR.process(buffer.getWritePointer(1));
            break;
        case FF_TPTZ:
            ffvcaTPTz.process(buffer.getWritePointer(0));
            ffvcaTPTzR.process(buffer.getWritePointer(1));
            break;
        case FF_TPT:
            ffvcaTPT.process(buffer.getWritePointer(0));
            ffvcaTPTR.process(buffer.getWritePointer(1));
            break;
        case FB_TRAD:
            fbvcaTrad.process(buffer.getWritePointer(0));
            fbvcaTradR.process(buffer.getWritePointer(1));
            break;
        default:
            fbvcaTPTz.process(buffer.getWritePointer(0));
            fbvcaTPTzR.process(buffer.getWritePointer(1));
            break;
    }
    /*
    //deinterleave
    juce::AudioDataConverters::deinterleaveSamples(
        reinterpret_cast<float*>(interleaved.getWritePointer(0)),
        inout,
        buffer.getNumSamples(),
        static_cast<int>(juce::dsp::SIMDRegister<float>::size())
        );
        */
}

//==============================================================================
bool CompressorTestbenchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CompressorTestbenchAudioProcessor::createEditor()
{
    return new CompressorTestbenchAudioProcessorEditor (*this);
}

//==============================================================================
void CompressorTestbenchAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CompressorTestbenchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorTestbenchAudioProcessor();
}
