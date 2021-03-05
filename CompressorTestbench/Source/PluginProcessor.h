/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Compressors.h"

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

    //Compressors
    FFVCA_Trad/*<float>*/ ffvcaTrad;
    FFVCA_TPTz/*<float>*/ ffvcaTPTz;
    FFVCA_TPT/*<float>*/ ffvcaTPT;
    FBVCA_Trad fbvcaTrad;
    FBVCA_TPTz fbvcaTPTz;

    FFVCA_Trad/*<float>*/ ffvcaTradR;
    FFVCA_TPTz/*<float>*/ ffvcaTPTzR;
    FFVCA_TPT/*<float>*/ ffvcaTPTR;
    FBVCA_Trad fbvcaTradR;
    FBVCA_TPTz fbvcaTPTzR;
    
private:
    //SIMD optimization
    /*
    float* inout[juce::dsp::SIMDRegister<float>::size()];
    juce::AudioBuffer<juce::dsp::SIMDRegister<float>> interleaved;
    juce::AudioBuffer<float> zeros;
    */

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
        FF_TRAD = 1,
        FF_TPTZ,
        FF_TPT,
        FB_TRAD,
        FB_TPTZ,
        NUM_MODELS
    };
    CompressorModel currentCompressor = FF_TRAD;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorTestbenchAudioProcessor)
};
//TODO double support
//TODO enum classes
//TODO SIMD
//TODO makeupGain slider