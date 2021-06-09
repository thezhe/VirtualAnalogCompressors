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
    const int sliderDY = 20;
    const int sliderHeight = 20;

    //references
    CompressorTestbenchAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    //Saturation Section
    juce::Label saturationSectionLabel;

    //Filter Type 
    juce::Label filterTypeLabel;
    juce::ComboBox filterTypeComboBox;
    std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;

    //Cutoff
    juce::Label cutoffLabel;
    juce::Slider cutoffSlider;
    std::unique_ptr<SliderAttachment> cutoffAttachment;

    //Feedback Saturation
    juce::Label feedbackSaturationLabel;
    juce::ToggleButton feedbackSaturationButton;
    std::unique_ptr<ButtonAttachment> feedbackSaturationAttachment;

    //Saturation
    juce::Label saturationLabel;
    juce::Slider saturationSlider;
    std::unique_ptr<SliderAttachment> saturationAttachment;

    //Dynamics Section
    juce::Label dynamicsSectionLabel;

    //Sidechain Input
    juce::Label sidechainInputLabel;
    juce::ComboBox sidechainInputComboBox; 
    std::unique_ptr<ComboBoxAttachment> sidechainInputAttachment;

    //Stereo Link
    juce::Label stereoLinkLabel;
    juce::ToggleButton stereoLinkButton;
    std::unique_ptr<ButtonAttachment> stereoLinkAttachment;

    //Pre-filter
    juce::Label preFilterLabel;
    juce::ComboBox preFilterComboBox;
    std::unique_ptr<ComboBoxAttachment> preFilterAttachment;

    //Rectifier
    juce::Label rectifierLabel;
    juce::ComboBox rectifierComboBox;
    std::unique_ptr<ComboBoxAttachment> rectifierAttachment;

    //Threshold
    juce::Label thresholdLabel;
    juce::Slider thresholdSlider;
    std::unique_ptr<SliderAttachment> thresholdAttachment;

    //Knee
    juce::Label kneeLabel;
    juce::Slider kneeSlider;
    std::unique_ptr<SliderAttachment> kneeAttachment;

    //Ratio
    juce::Label ratioLabel;
    juce::Slider ratioSlider;
    std::unique_ptr<SliderAttachment> ratioAttachment;

    //Attack
    juce::Label attackLabel;
    juce::Slider attackSlider;
    std::unique_ptr<SliderAttachment> attackAttachment;

    //Attack Nonlinearity
    juce::Label attackNonlinearityLabel;
    juce::Slider attackNonlinearitySlider;
    std::unique_ptr<SliderAttachment> attackNonlinearityAttachment;

    //Release
    juce::Label releaseLabel;
    juce::Slider releaseSlider;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    //Release Nonlinearity
    juce::Label releaseNonlinearityLabel;
    juce::Slider releaseNonlinearitySlider;
    std::unique_ptr<SliderAttachment> releaseNonlinearityAttachment;

    //Sensitivity
    juce::Label sensitivityLabel;
    juce::Slider sensitivitySlider;
    std::unique_ptr<SliderAttachment> sensitivityAttachment;

    //Wet
    juce::Label wetLabel;
    juce::Slider wetSlider;
    std::unique_ptr<SliderAttachment> wetAttachment;

    //Dry
    juce::Label dryLabel;
    juce::Slider drySlider;
    std::unique_ptr<SliderAttachment> dryAttachment;


#ifdef DEBUG
    
    //Debug Only Section
    juce::Label debugOnlySectionLabel;

    //Output
    juce::Label outputLabel;
    juce::ComboBox outputComboBox;

#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorTestbenchAudioProcessorEditor)
};

//TODO make -60dB dry -infdB internally
//TODO CTF animation: https://docs.juce.com/master/tutorial_animation.html