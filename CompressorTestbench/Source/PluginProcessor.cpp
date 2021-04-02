/*
  ==============================================================================
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
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
    //prepare processors
    compressor.prepare(sampleRate, samplesPerBlock);

    //prepare SIMD
    interleaved = juce::dsp::AudioBlock<float>(interleavedBlockData, 1, samplesPerBlock*SIMD::size);
    zero = juce::dsp::AudioBlock<float>(zeroData, SIMD::size, samplesPerBlock);
    zero.clear();
}

void CompressorTestbenchAudioProcessor::releaseResources()
{
    compressor.reset();
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
    //disable denormals via hardware flag
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    Spec::setNumChannels(totalNumInputChannels);
    
    //clear extra buffer channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
   
    //get channel pointers
    auto* inout = channelPointers.getData();
    for (size_t ch = 0; ch < juce::dsp::SIMDRegister<float>::size(); ++ch)
        inout[ch] = ch < totalNumInputChannels ? 
        buffer.getReadPointer(ch) :
        zero.getChannelPointer(ch);
    
    //interleave channel data
    juce::AudioDataConverters::interleaveSamples(
        inout,
        interleaved.getChannelPointer(0),
        buffer.getNumSamples(),
        SIMD::size
    );

    //process
    compressor.process(interleaved.getChannelPointer(0));
    
    //deinterleave
    juce::AudioDataConverters::deinterleaveSamples(
        interleaved.getChannelPointer(0),
        const_cast<float**>(inout),
        buffer.getNumSamples(),
        SIMD::size
        );
        
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
