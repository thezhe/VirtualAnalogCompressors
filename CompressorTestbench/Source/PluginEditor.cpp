/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorTestbenchAudioProcessorEditor::CompressorTestbenchAudioProcessorEditor (CompressorTestbenchAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), 
    audioProcessor (p), 
    valueTreeState (vts)
{
    //Saturation Section
    saturationSectionLabel.setText("Saturation", juce::dontSendNotification);
    addAndMakeVisible(saturationSectionLabel);

    //Input Filter
    filterTypeComboBox.addItem("LP1", int(VA::Multimode1FilterType::Lowpass));
    filterTypeComboBox.addItem("HP1", int(VA::Multimode1FilterType::Highpass));
    filterTypeComboBox.onChange = [this]
    {
        audioProcessor.nlMM1.setFilterType
        (
            VA::Multimode1FilterType(filterTypeComboBox.getSelectedId())
        );
    };
    addAndMakeVisible(filterTypeComboBox);

    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.attachToComponent(&filterTypeComboBox, true);
    addAndMakeVisible(filterTypeLabel);

    filterTypeAttachment.reset(new ComboBoxAttachment(valueTreeState, "inputFilterType", filterTypeComboBox));

    //Cutoff
    cutoffSlider.setTextValueSuffix(" Hz");
    cutoffSlider.onValueChange = [this]
    {
        audioProcessor.nlMM1.setLinearCutoff(cutoffSlider.getValue());
    };
    addAndMakeVisible(cutoffSlider);

    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.attachToComponent(&cutoffSlider, true);
    addAndMakeVisible(cutoffLabel);

    cutoffAttachment.reset(new SliderAttachment(valueTreeState, "inputCutoff", cutoffSlider));

    //Feedback Saturation
    feedbackSaturationButton.onStateChange = [this]
    {
        audioProcessor.nlMM1.setFeedbackSaturation(feedbackSaturationButton.getToggleState());
    };
    addAndMakeVisible(feedbackSaturationButton);

    feedbackSaturationLabel.setText("Feedback Saturation", juce::dontSendNotification);
    feedbackSaturationLabel.attachToComponent(&feedbackSaturationButton, true);
    addAndMakeVisible(feedbackSaturationLabel);

    feedbackSaturationAttachment.reset(new ButtonAttachment(valueTreeState, "inputFeedbackSaturation", feedbackSaturationButton));

    //Saturation
    saturationSlider.onValueChange = [this]
    {
        audioProcessor.nlMM1.setNonlinearity(saturationSlider.getValue());
    };
    addAndMakeVisible(saturationSlider);

    saturationLabel.setText("Saturation", juce::dontSendNotification);
    saturationLabel.attachToComponent(&saturationSlider, true);
    addAndMakeVisible(saturationLabel);

    cutoffAttachment.reset(new SliderAttachment(valueTreeState, "inputSaturation", saturationSlider));

    //Dynamics Section
    dynamicsSectionLabel.setText("Dynamics", juce::dontSendNotification);
    addAndMakeVisible(dynamicsSectionLabel);

    //Sidechain Input
    sidechainInputComboBox.addItem("Feedforward", 1);
    sidechainInputComboBox.addItem("Feedback", 2);
    //sidechainInputComboBox.addItem("Sidechain", 3);
    sidechainInputComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setSidechainInputType
        (
            VA::DynamicsProcessorSidechainInputType(sidechainInputComboBox.getSelectedId())
        );
    };
    addAndMakeVisible(sidechainInputComboBox);

    sidechainInputLabel.setText("Sidechain Input", juce::dontSendNotification);
    sidechainInputLabel.attachToComponent(&sidechainInputComboBox, true);
    addAndMakeVisible(sidechainInputLabel);

    sidechainInputAttachment.reset(new ComboBoxAttachment(valueTreeState, "sidechainInput", sidechainInputComboBox));

    //Stereo Link
    stereoLinkButton.onStateChange = [this]
    {
        audioProcessor.dynamicsProcessor.setStereoLink(stereoLinkButton.getToggleState());
    };
    addAndMakeVisible(stereoLinkButton);

    stereoLinkLabel.setText("Stereo Link", juce::dontSendNotification);
    stereoLinkLabel.attachToComponent(&stereoLinkButton, true);
    addAndMakeVisible(stereoLinkLabel);

    stereoLinkAttachment.reset(new ButtonAttachment(valueTreeState, "stereoLink", stereoLinkButton));

    //Pre-filter
    preFilterComboBox.addItem("None", 1);
    preFilterComboBox.addItem("K-Weighting", 2);
    preFilterComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setPreFilterType(VA::DetectorPreFilterType(preFilterComboBox.getSelectedId()));
    };
    addAndMakeVisible(preFilterComboBox);

    preFilterLabel.setText("Pre-filter", juce::dontSendNotification);
    preFilterLabel.attachToComponent(&preFilterComboBox, true);
    addAndMakeVisible(preFilterLabel);

    preFilterAttachment.reset(new ComboBoxAttachment(valueTreeState, "preFilterType", preFilterComboBox));

    //Rectifier
    rectifierComboBox.addItem("Peak", 1);
    rectifierComboBox.addItem("Half Wave", 2);
    rectifierComboBox.addItem("Full Wave", 3);
    rectifierComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setRectifierType(VA::DetectorRectifierType(rectifierComboBox.getSelectedId()));
    };
    addAndMakeVisible(rectifierComboBox);

    rectifierLabel.setText("Rectifier", juce::dontSendNotification);
    rectifierLabel.attachToComponent(&rectifierComboBox, true);
    addAndMakeVisible(rectifierLabel);

    rectifierAttachment.reset(new ComboBoxAttachment(valueTreeState, "rectifierType", rectifierComboBox));

    //Threshold
    thresholdSlider.setTextValueSuffix(" dB");
    thresholdSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setThreshold(thresholdSlider.getValue());
    };
    addAndMakeVisible(thresholdSlider);

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, true);
    addAndMakeVisible(thresholdLabel);

    thresholdAttachment.reset(new SliderAttachment(valueTreeState, "threshold", thresholdSlider));

    //Knee
    kneeSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setKnee(kneeSlider.getValue());
    };
    addAndMakeVisible(kneeSlider);

    kneeLabel.setText("Knee", juce::dontSendNotification);
    kneeLabel.attachToComponent(&kneeSlider, true);
    addAndMakeVisible(kneeLabel);

    kneeAttachment.reset(new SliderAttachment(valueTreeState, "knee", kneeSlider));

    //Ratio
    ratioSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setRatio(ratioSlider.getValue());
    };
    addAndMakeVisible(ratioSlider);

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, true);
    addAndMakeVisible(ratioLabel);

    ratioAttachment.reset(new SliderAttachment(valueTreeState, "ratio", ratioSlider));

    //Attack
    attackSlider.setTextValueSuffix(" ms");
    attackSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setAttack(attackSlider.getValue());
    };
    addAndMakeVisible(attackSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
    addAndMakeVisible(attackLabel);

    attackAttachment.reset(new SliderAttachment(valueTreeState, "attack", attackSlider));

    //Attack Nonlinearity
    attackNonlinearitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setAttackNonlinearity(attackNonlinearitySlider.getValue());
    };
    addAndMakeVisible(attackNonlinearitySlider);

    attackNonlinearityLabel.setText("Attack Nonlinearity", juce::dontSendNotification);
    attackNonlinearityLabel.attachToComponent(&attackNonlinearitySlider, true);
    addAndMakeVisible(attackNonlinearityLabel);

    attackNonlinearityAttachment.reset(new SliderAttachment(valueTreeState, "attackNonlinearity", attackNonlinearitySlider));

    //Release
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setRelease(releaseSlider.getValue());
    };
    addAndMakeVisible(releaseSlider);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    addAndMakeVisible(releaseLabel);

    releaseAttachment.reset(new SliderAttachment(valueTreeState, "release", releaseSlider));

    //Release Nonlinearity
    releaseNonlinearitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setReleaseNonlinearity(releaseNonlinearitySlider.getValue());
    };
    addAndMakeVisible(releaseNonlinearitySlider);

    releaseNonlinearityLabel.setText("Release Nonlinearity", juce::dontSendNotification);
    releaseNonlinearityLabel.attachToComponent(&releaseNonlinearitySlider, true);
    addAndMakeVisible(releaseNonlinearityLabel);

    releaseNonlinearityAttachment.reset(new SliderAttachment(valueTreeState, "releaseNonlinearity", releaseNonlinearitySlider));

    //Sensitivity
    sensitivitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setSensitivity(sensitivitySlider.getValue());
    };
    addAndMakeVisible(sensitivitySlider);

    sensitivityLabel.setText("Sensitivity", juce::dontSendNotification);
    sensitivityLabel.attachToComponent(&sensitivitySlider, true);
    addAndMakeVisible(sensitivityLabel);

    sensitivityAttachment.reset(new SliderAttachment(valueTreeState, "sensitivity", sensitivitySlider));

    //Wet
    wetSlider.setTextValueSuffix(" dB");
    wetSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setWetGain(wetSlider.getValue());
    };
    addAndMakeVisible(wetSlider);
    
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, true);
    addAndMakeVisible(wetLabel);

    wetAttachment.reset(new SliderAttachment(valueTreeState, "wet", wetSlider));

    //Dry
    drySlider.setTextValueSuffix(" dB");
    drySlider.onValueChange = [this] 
    {
        audioProcessor.dynamicsProcessor.setDryGain(drySlider.getValue());
    };
    addAndMakeVisible(drySlider);

    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.attachToComponent(&drySlider, true);
    addAndMakeVisible(dryLabel);

    dryAttachment.reset(new SliderAttachment(valueTreeState, "dry", drySlider));

#ifdef DEBUG

    //Debug Only Section
    debugOnlySectionLabel.setText("Debug Only", juce::dontSendNotification);
    addAndMakeVisible(debugOnlySectionLabel);

    //Output
    outputComboBox.addItem("Detector", 1);
    outputComboBox.addItem("Envelope Filter", 2);
    outputComboBox.addItem("Transfer Function", 3);
    outputComboBox.addItem("Normal", 4);
    outputComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setOutputType
        (
            VA::DynamicsProcessorOutputType(outputComboBox.getSelectedId())
        );
    };
    addAndMakeVisible(outputComboBox);

    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.attachToComponent(&outputComboBox, true);
    addAndMakeVisible(outputLabel);

#endif

    //Window Size
    setSize (400, 2 * marginT + 22 * sliderDY);
}

CompressorTestbenchAudioProcessorEditor::~CompressorTestbenchAudioProcessorEditor()
{
}

//==============================================================================
void CompressorTestbenchAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void CompressorTestbenchAudioProcessorEditor::resized()
{
    auto sliderWidth = getWidth() - marginL - 10;
    auto labelWidth = getWidth() / 2;
    auto labelL = (getWidth() / 2) - (labelWidth / 2);

    saturationSectionLabel.setBounds(labelL, marginT, labelWidth, sliderHeight);
    filterTypeComboBox.setBounds(marginL, marginT + sliderDY, sliderWidth, sliderHeight);
    cutoffSlider.setBounds(marginL, marginT + 2* sliderDY, sliderWidth, sliderHeight);
    feedbackSaturationButton.setBounds(marginL, marginT + 3 * sliderDY, sliderWidth, sliderHeight);
    saturationSlider.setBounds(marginL, marginT + 4 * sliderDY, sliderWidth, sliderHeight);

    dynamicsSectionLabel.setBounds(labelL, marginT + 5 * sliderDY, labelWidth, sliderHeight);
    sidechainInputComboBox.setBounds(marginL, marginT + 6 * sliderDY, sliderWidth, sliderHeight);
    stereoLinkButton.setBounds(marginL, marginT + 7 * sliderDY, sliderWidth, sliderHeight);
    preFilterComboBox.setBounds(marginL, marginT + 8 * sliderDY, sliderWidth, sliderHeight);
    rectifierComboBox.setBounds(marginL, marginT + 9 * sliderDY, sliderWidth, sliderHeight);
    thresholdSlider.setBounds(marginL, marginT + 10 * sliderDY, sliderWidth, sliderHeight);
    kneeSlider.setBounds(marginL, marginT + 11 * sliderDY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(marginL, marginT + 12 * sliderDY, sliderWidth, sliderHeight);
    attackSlider.setBounds(marginL, marginT + 13 * sliderDY, sliderWidth, sliderHeight);
    attackNonlinearitySlider.setBounds(marginL, marginT + 14 * sliderDY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(marginL, marginT + 15 * sliderDY, sliderWidth, sliderHeight);
    releaseNonlinearitySlider.setBounds(marginL, marginT + 16 * sliderDY, sliderWidth, sliderHeight);    
    sensitivitySlider.setBounds(marginL, marginT + 17 * sliderDY, sliderWidth, sliderHeight);

    wetSlider.setBounds(marginL, marginT + 18 * sliderDY, sliderWidth, sliderHeight);
    drySlider.setBounds(marginL, marginT + 19 * sliderDY, sliderWidth, sliderHeight);

#ifdef DEBUG

    debugOnlySectionLabel.setBounds(labelL, marginT + 20 * sliderDY, labelWidth, sliderHeight);
    outputComboBox.setBounds(marginL, marginT + 21 * sliderDY, sliderWidth, sliderHeight);

#endif

}
