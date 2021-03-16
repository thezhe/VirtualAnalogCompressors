/*
  ==============================================================================
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Compressors.h"
#include "Filters.h"

//==============================================================================
/**
*/
class CompressorTestbenchAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CompressorTestbenchAudioProcessor();
    ~CompressorTestbenchAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setCompressor(int index)
    {
        if (index < 1 || index > NUM_MODELS) return;
        currentCompressor = (CompressorModel)index;
    }

    //TEST
    Multimode1_TPT<float> mm1_TPT;

    //compressor models
    FFVCA_IIR<float> ffvcaIIR;
    FFVCA_TPTz<float> ffvcaTPTz;
    FFVCA_TPT<float> ffvcaTPT;
    
    FBVCA_IIR<float> fbvcaIIR;
    FBVCA_TPTz<float> fbvcaTPTz;

    using SIMD = xsimd::simd_type<float>;
private:

    //SIMD optimization          
    juce::dsp::AudioBlock<float> interleaved, zero;
    juce::HeapBlock<char> interleavedBlockData, zeroData;              
    juce::HeapBlock<const float*> channelPointers{ SIMD::size };

    //parameters
    /*enum class compressorModel : size_t
    {
        FF_Trad = 1,
        FF_TPTz = 2,
        FF_TPT = 3,
        FB_Trad = 4,
        FB_TPTz = 5,
        FB_TPT = 6
    };*/
    enum CompressorModel
    {
        FF_IIR = 1,
        FF_TPTZ,
        FF_TPT,
        FB_IIR,
        FB_TPTZ,
        NUM_MODELS
    };
    CompressorModel currentCompressor = FF_IIR;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorTestbenchAudioProcessor)
};
//TODO double support
//TODO enum classes
//TODO SIMD