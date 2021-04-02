/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CompressorTestbenchAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CompressorTestbenchAudioProcessorEditor (CompressorTestbenchAudioProcessor&);
    ~CompressorTestbenchAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    //dimension specs
    const int marginL = 120, marginT = 20;
    const int sliderDY = 30;
    const int sliderHeight = 20;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CompressorTestbenchAudioProcessor& audioProcessor;

    //Processor Parameters
    juce::Label compressorLabel;

    juce::Label attackLabel, releaseLabel, thresholdLabel, ratioLabel, wetLabel, dryLabel, sidechainLabel, stereoLinkLabel;
    juce::Slider attackSlider, releaseSlider, thresholdSlider, ratioSlider, wetSlider, drySlider;
    juce::ComboBox sidechainComboBox;
    juce::ToggleButton stereoLinkToggle;

    juce::Label rlToggleLabel, saturationRLLabel, linearTauRLLabel;
    juce::ToggleButton rlToggle;
    juce::Slider saturationRLSlider, linearTauRLSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorTestbenchAudioProcessorEditor)
};

//TODO stereo link toggle
