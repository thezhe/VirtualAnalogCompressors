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
    //Title
    titleLabel.setText("Dynamics Processor Testbench", juce::dontSendNotification);
    addAndMakeVisible(titleLabel);

    //Input Section
    inputSectionLabel.setText("Input", juce::dontSendNotification);
    addAndMakeVisible(inputSectionLabel);

    //Input Filter
    inputFilterComboBox.addItem("LP1", 1);
    inputFilterComboBox.addItem("HP1", 2);
    inputFilterComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setInputFilterType
        (
            Multimode1FilterType(inputFilterComboBox.getSelectedId())
        );
    };
    addAndMakeVisible(inputFilterComboBox);

    inputFilterLabel.setText("Input Filter", juce::dontSendNotification);
    inputFilterLabel.attachToComponent(&inputFilterComboBox, true);
    addAndMakeVisible(inputFilterLabel);

    inputFilterAttachment.reset(new ComboBoxAttachment(valueTreeState, "inputFilterType", inputFilterComboBox));

    //Cutoff
    cutoffSlider.setTextValueSuffix(" Hz");
    cutoffSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setInputFilterCutoff(cutoffSlider.getValue());
    };
    addAndMakeVisible(cutoffSlider);

    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.attachToComponent(&cutoffSlider, true);
    addAndMakeVisible(cutoffLabel);

    cutoffAttachment.reset(new SliderAttachment(valueTreeState, "inputFilterCutoff", cutoffSlider));

    //Feedback Saturation
    feedbackSaturationButton.onStateChange = [this]
    {
        audioProcessor.dynamicsProcessor.setInputFilterFeedbackSaturation(feedbackSaturationButton.getToggleState());
    };
    addAndMakeVisible(feedbackSaturationButton);

    feedbackSaturationLabel.setText("Feedback Saturation", juce::dontSendNotification);
    feedbackSaturationLabel.attachToComponent(&feedbackSaturationButton, true);
    addAndMakeVisible(feedbackSaturationLabel);

    feedbackSaturationAttachment.reset(new ButtonAttachment(valueTreeState, "inputFilterFeedbackSaturation", feedbackSaturationButton));

    //Saturation
    saturationSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setInputFilterSaturation(saturationSlider.getValue());
    };
    addAndMakeVisible(saturationSlider);

    saturationLabel.setText("Saturation", juce::dontSendNotification);
    saturationLabel.attachToComponent(&saturationSlider, true);
    addAndMakeVisible(saturationLabel);

    cutoffAttachment.reset(new SliderAttachment(valueTreeState, "inputFilterSaturation", saturationSlider));

    //Sidechain Section
    sidechainSectionLabel.setText("Sidechain", juce::dontSendNotification);
    addAndMakeVisible(sidechainSectionLabel);

    //Sidechain Input
    sidechainInputComboBox.addItem("Feedforward", 1);
    sidechainInputComboBox.addItem("Feedback", 2);
    //sidechainInputComboBox.addItem("Sidechain", 3);
    sidechainInputComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setSidechainInputType
        (
            DynamicsProcessorSidechainInputType(sidechainInputComboBox.getSelectedId())
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
        audioProcessor.dynamicsProcessor.setPreFilterType(DetectorPreFilterType(preFilterComboBox.getSelectedId()));
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
        audioProcessor.dynamicsProcessor.setRectifierType(DetectorRectifierType(rectifierComboBox.getSelectedId()));
    };
    addAndMakeVisible(rectifierComboBox);

    rectifierLabel.setText("Rectifier", juce::dontSendNotification);
    rectifierLabel.attachToComponent(&rectifierComboBox, true);
    addAndMakeVisible(rectifierLabel);

    rectifierAttachment.reset(new ComboBoxAttachment(valueTreeState, "rectifierType", rectifierComboBox));

    //Detector Gain
    detectorGainSlider.setTextValueSuffix(" dB");
    detectorGainSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setDetectorGain(detectorGainSlider.getValue());
    };
    addAndMakeVisible(detectorGainSlider);

    detectorGainLabel.setText("Detector Gain", juce::dontSendNotification);
    detectorGainLabel.attachToComponent(&detectorGainSlider, true);
    addAndMakeVisible(detectorGainLabel);

    detectorGainAttachment.reset(new SliderAttachment(valueTreeState, "detectorGain", detectorGainSlider));

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

    //Compressor
    compressorButton.onStateChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressor(compressorButton.getToggleState());
    };
    addAndMakeVisible(compressorButton);

    compressorLabel.setText("Compressor", juce::dontSendNotification);
    compressorLabel.attachToComponent(&compressorButton, true);
    addAndMakeVisible(compressorLabel);

    compressorAttachment.reset(new ButtonAttachment(valueTreeState, "compressor", compressorButton));

    //Compressor Section
    compressorSectionLabel.setText("Compressor Only", juce::dontSendNotification);
    addAndMakeVisible(compressorSectionLabel);

    //Attack
    attackSlider.setTextValueSuffix(" ms");
    attackSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressorAttack(attackSlider.getValue());
    };
    addAndMakeVisible(attackSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
    addAndMakeVisible(attackLabel);

    attackAttachment.reset(new SliderAttachment(valueTreeState, "compressorAttack", attackSlider));

    //Attack Nonlinearity
    attackNonlinearitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressorAttackNonlinearity(attackNonlinearitySlider.getValue());
    };
    addAndMakeVisible(attackNonlinearitySlider);

    attackNonlinearityLabel.setText("Attack Nonlinearity", juce::dontSendNotification);
    attackNonlinearityLabel.attachToComponent(&attackNonlinearitySlider, true);
    addAndMakeVisible(attackNonlinearityLabel);

    attackNonlinearityAttachment.reset(new SliderAttachment(valueTreeState, "compressorAttackNonlinearity", attackNonlinearitySlider));

    //Release
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressorRelease(releaseSlider.getValue());
    };
    addAndMakeVisible(releaseSlider);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    addAndMakeVisible(releaseLabel);

    releaseAttachment.reset(new SliderAttachment(valueTreeState, "compressorRelease", releaseSlider));

    //Release Nonlinearity
    releaseNonlinearitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressorReleaseNonlinearity(releaseNonlinearitySlider.getValue());
    };
    addAndMakeVisible(releaseNonlinearitySlider);

    releaseNonlinearityLabel.setText("Release Nonlinearity", juce::dontSendNotification);
    releaseNonlinearityLabel.attachToComponent(&releaseNonlinearitySlider, true);
    addAndMakeVisible(releaseNonlinearityLabel);

    releaseNonlinearityAttachment.reset(new SliderAttachment(valueTreeState, "compressorReleaseNonlinearity", releaseNonlinearitySlider));

    //Ratio
    ratioSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setCompressorRatio(ratioSlider.getValue());
    };
    addAndMakeVisible(ratioSlider);

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, true);
    addAndMakeVisible(ratioLabel);

    ratioAttachment.reset(new SliderAttachment(valueTreeState, "compressorRatio", ratioSlider));

    //Transient Designer Section
    transientDesignerSectionLabel.setText("Transient Designer Only", juce::dontSendNotification);

    //Tau
    tauSlider.setTextValueSuffix(" ms");
    tauSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setTransientDesignerTau(tauSlider.getValue());
    };
    addAndMakeVisible(tauSlider);

    tauLabel.setText("Tau", juce::dontSendNotification);
    tauLabel.attachToComponent(&tauSlider, true);
    addAndMakeVisible(tauLabel);

    tauAttachment.reset(new SliderAttachment(valueTreeState, "transientDesignerTau", tauSlider));

    //Sensitivity
    sensitivitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setTransientDesignerSensitivity(sensitivitySlider.getValue());
    };
    addAndMakeVisible(sensitivitySlider);

    sensitivityLabel.setText("Sensitivity", juce::dontSendNotification);
    sensitivityLabel.attachToComponent(&sensitivitySlider, true);
    addAndMakeVisible(sensitivityLabel);

    sensitivityAttachment.reset(new SliderAttachment(valueTreeState, "transientDesignerSensitivity", sensitivitySlider));
  
    //Nonlinearity
    nonlinearitySlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setTransientDesignerNonlinearity(nonlinearitySlider.getValue());
    };
    addAndMakeVisible(nonlinearitySlider);

    nonlinearityLabel.setText("Nonlinearity", juce::dontSendNotification);
    nonlinearityLabel.attachToComponent(&nonlinearitySlider, true);
    addAndMakeVisible(nonlinearityLabel);

    nonlinearityAttachment.reset(new SliderAttachment(valueTreeState, "transientDesignerNonlinearity", nonlinearitySlider));

    //Attack Ratio
    attackRatioSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setTransientDesignerAttackRatio(attackRatioSlider.getValue());
    };
    addAndMakeVisible(attackRatioSlider);

    attackRatioLabel.setText("Attack Ratio", juce::dontSendNotification);
    attackRatioLabel.attachToComponent(&attackRatioSlider, true);
    addAndMakeVisible(attackRatioLabel);

    attackRatioAttachment.reset(new SliderAttachment(valueTreeState, "transientDesignerAttackRatio", attackRatioSlider));

    //Release Ratio
    releaseRatioSlider.onValueChange = [this]
    {
        audioProcessor.dynamicsProcessor.setTransientDesignerReleaseRatio(releaseRatioSlider.getValue());
    };
    addAndMakeVisible(releaseRatioSlider);

    releaseRatioLabel.setText("Release Ratio", juce::dontSendNotification);
    releaseRatioLabel.attachToComponent(&releaseRatioSlider, true);
    addAndMakeVisible(releaseRatioLabel);

    releaseRatioAttachment.reset(new SliderAttachment(valueTreeState, "transientDesignerReleaseRatio", releaseRatioSlider));

    //Output 
    outputSectionLabel.setText("Output", juce::dontSendNotification);
    addAndMakeVisible(outputSectionLabel);

#ifdef DEBUG
    //Output
    outputComboBox.addItem("Detector", 1);
    outputComboBox.addItem("Filter", 2);
    outputComboBox.addItem("Transfer Function", 3);
    outputComboBox.addItem("Normal", 4);
    outputComboBox.onChange = [this]
    {
        audioProcessor.dynamicsProcessor.setOutputType
        (
            DynamicsProcessorOutputType(outputComboBox.getSelectedId())
        );
    };
    addAndMakeVisible(outputComboBox);

    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.attachToComponent(&outputComboBox, true);
    addAndMakeVisible(outputLabel);
#endif

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

    //Window Size
    setSize (400, 2 * marginT + 30 * sliderDY);
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

    titleLabel.setBounds(labelL, marginT, labelWidth, sliderHeight);

    inputSectionLabel.setBounds(marginL, marginT + sliderDY, labelWidth, sliderHeight);
    inputFilterComboBox.setBounds(marginL, marginT + 2 * sliderDY, sliderWidth, sliderHeight);
    cutoffSlider.setBounds(marginL, marginT + 3 * sliderDY, sliderWidth, sliderHeight);
    feedbackSaturationButton.setBounds(marginL, marginT + 4 * sliderDY, sliderWidth, sliderHeight);
    saturationSlider.setBounds(marginL, marginT + 5 * sliderDY, sliderWidth, sliderHeight);

    sidechainSectionLabel.setBounds(marginL, marginT + 6 * sliderDY, labelWidth, sliderHeight);
    sidechainInputComboBox.setBounds(marginL, marginT + 7 * sliderDY, sliderWidth, sliderHeight);
    stereoLinkButton.setBounds(marginL, marginT + 8 * sliderDY, sliderWidth, sliderHeight);
    preFilterComboBox.setBounds(marginL, marginT + 9 * sliderDY, sliderWidth, sliderHeight);
    rectifierComboBox.setBounds(marginL, marginT + 10 * sliderDY, sliderWidth, sliderHeight);
    detectorGainSlider.setBounds(marginL, marginT + 11 * sliderDY, sliderWidth, sliderHeight);
    thresholdSlider.setBounds(marginL, marginT + 12 * sliderDY, sliderWidth, sliderHeight);
    compressorButton.setBounds(marginL, marginT + 13 * sliderDY, sliderWidth, sliderHeight);

    compressorSectionLabel.setBounds(marginL, marginT + 14* sliderDY, labelWidth, sliderHeight);
    attackSlider.setBounds(marginL, marginT + 15 * sliderDY, sliderWidth, sliderHeight);
    attackNonlinearitySlider.setBounds(marginL, marginT + 16 * sliderDY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(marginL, marginT + 17 * sliderDY, sliderWidth, sliderHeight);
    releaseNonlinearitySlider.setBounds(marginL, marginT + 18 * sliderDY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(marginL, marginT + 19 * sliderDY, sliderWidth, sliderHeight);

    transientDesignerSectionLabel.setBounds(marginL, marginT + 20 * sliderDY, labelWidth, sliderHeight);
    tauSlider.setBounds(marginL, marginT + 21 * sliderDY, sliderWidth, sliderHeight);
    sensitivitySlider.setBounds(marginL, marginT + 22 * sliderDY, sliderWidth, sliderHeight);
    nonlinearitySlider.setBounds(marginL, marginT + 23 * sliderDY, sliderWidth, sliderHeight);
    attackRatioSlider.setBounds(marginL, marginT + 24 * sliderDY, sliderWidth, sliderHeight);
    releaseRatioSlider.setBounds(marginL, marginT + 25 * sliderDY, sliderWidth, sliderHeight);

    outputSectionLabel.setBounds(marginL, marginT + 26 * sliderDY, labelWidth, sliderHeight);

#ifdef DEBUG
    outputComboBox.setBounds(marginL, marginT + 27 * sliderDY, sliderWidth, sliderHeight);
#endif

    wetSlider.setBounds(marginL, marginT + 28 * sliderDY, sliderWidth, sliderHeight);
    drySlider.setBounds(marginL, marginT + 29 * sliderDY, sliderWidth, sliderHeight);
}
