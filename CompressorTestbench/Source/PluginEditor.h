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

    //processor reference
    CompressorTestbenchAudioProcessor& audioProcessor;

    //APVTS reference
    juce::AudioProcessorValueTreeState& valueTreeState;

    //Title
    juce::Label titleLabel;

    //Input Section
    juce::Label inputSectionLabel;

    //Input Filter 
    juce::Label inputFilterLabel;
    juce::ComboBox inputFilterComboBox;
    std::unique_ptr<ComboBoxAttachment> inputFilterAttachment;

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

    //Sidechain Section
    juce::Label sidechainSectionLabel;

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

    //Detector Gain
    juce::Label detectorGainLabel;
    juce::Slider detectorGainSlider;
    std::unique_ptr<SliderAttachment> detectorGainAttachment;

    //Threshold
    juce::Label thresholdLabel;
    juce::Slider thresholdSlider;
    std::unique_ptr<SliderAttachment> thresholdAttachment;

    //Compressor
    juce::Label compressorLabel;
    juce::ToggleButton compressorButton;
    std::unique_ptr<ButtonAttachment> compressorAttachment;

    //Compressor Section
    juce::Label compressorSectionLabel;

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

    //Ratio
    juce::Label ratioLabel;
    juce::Slider ratioSlider;
    std::unique_ptr<SliderAttachment> ratioAttachment;

    //Transient Designer Section
    juce::Label transientDesignerSectionLabel;

    //Tau
    juce::Label tauLabel;
    juce::Slider tauSlider;
    std::unique_ptr<SliderAttachment> tauAttachment;

    //Sensitivity
    juce::Label sensitivityLabel;
    juce::Slider sensitivitySlider;
    std::unique_ptr<SliderAttachment> sensitivityAttachment;

    //Nonlinearity
    juce::Label nonlinearityLabel;
    juce::Slider nonlinearitySlider;
    std::unique_ptr<SliderAttachment> nonlinearityAttachment;

    //Attack Ratio
    juce::Label attackRatioLabel;
    juce::Slider attackRatioSlider;
    std::unique_ptr<SliderAttachment> attackRatioAttachment;

    //Release Ratio
    juce::Label releaseRatioLabel;
    juce::Slider releaseRatioSlider;
    std::unique_ptr<SliderAttachment> releaseRatioAttachment;

    //Output Section
    juce::Label outputSectionLabel;

#ifdef DEBUG
    //Output
    juce::Label outputLabel;
    juce::ComboBox outputComboBox;
    std::unique_ptr<ComboBoxAttachment> outputAttachment;
#endif

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
