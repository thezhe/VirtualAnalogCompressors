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
    CompressorTestbenchAudioProcessorEditor (CompressorTestbenchAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~CompressorTestbenchAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
   
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    //dimension specs
    const int marginL = 120, marginT = 20;
    const int sliderDY = 30;
    const int sliderHeight = 20;

    //processor reference
    CompressorTestbenchAudioProcessor& audioProcessor;

    //APVTS reference
    juce::AudioProcessorValueTreeState& valueTreeState;

    //Title
    juce::Label compressorLabel;

    //Sidechain Input
    juce::Label sidechainInputLabel;
    juce::ComboBox sidechainInputComboBox; 
    std::unique_ptr<ComboBoxAttachment> sidechainInputAttachment;

    //Sidechain Input Gain
    juce::Label sidechainInputGainLabel;
    juce::Slider sidechainInputGainSlider;
    std::unique_ptr<SliderAttachment> sidechainInputGainAttachment;

    //Detector
    juce::Label detectorLabel;
    juce::ComboBox detectorComboBox;
    std::unique_ptr<ComboBoxAttachment> detectorAttachment;

    //Stereo Link
    juce::Label stereoLinkLabel;
    juce::ToggleButton stereoLinkToggle;
    std::unique_ptr<ButtonAttachment> stereoLinkAttachment;

    //Threshold
    juce::Label thresholdLabel;
    juce::Slider thresholdSlider;
    std::unique_ptr<SliderAttachment> thresholdAttachment;

    //Ratio
    juce::Label ratioLabel;
    juce::Slider ratioSlider;
    std::unique_ptr<SliderAttachment> ratioAttachment;

    //Attack
    juce::Label attackLabel;
    juce::Slider attackSlider;
    std::unique_ptr<SliderAttachment> attackAttachment;

    //Release
    juce::Label releaseLabel;
    juce::Slider releaseSlider;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    //RMS
    juce::Label RMSLabel;
    juce::ToggleButton RMSToggle;
    std::unique_ptr<ButtonAttachment> RMSAttachment;

    //RL
    juce::Label RLLabel;
    juce::ToggleButton RLToggle;
    std::unique_ptr<ButtonAttachment> RLAttachment;

    //RL Linear Tau
    juce::Label linearTauRLLabel;
    juce::Slider linearTauRLSlider;
    std::unique_ptr<SliderAttachment> linearTauRLAttachment;

    //RL nonlinearity
    juce::Label nonlinearityRLLabel;
    juce::Slider nonlinearityRLSlider;
    std::unique_ptr<SliderAttachment> nonlinearityRLAttachment;

    //Wet
    juce::Label wetLabel;
    juce::Slider wetSlider;
    std::unique_ptr<SliderAttachment> wetAttachment;

    //Dry
    juce::Label dryLabel;
    juce::Slider drySlider;
    std::unique_ptr<SliderAttachment> dryAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorTestbenchAudioProcessorEditor)
};
