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
    compressorLabel.setText("Compressor Testbench", juce::dontSendNotification);
    addAndMakeVisible(compressorLabel);

    //Sidechain Input
    sidechainInputComboBox.addItem("Feedforward", 1);
    sidechainInputComboBox.addItem("Feedback", 2);
    sidechainInputComboBox.addItem("Sidechain", 3);
    sidechainInputComboBox.onChange = [this]
    {
        audioProcessor.compressor.setSidechainType(CompressorSidechainType(sidechainInputComboBox.getSelectedId()));
    };
    addAndMakeVisible(sidechainInputComboBox);

    sidechainInputLabel.setText("Sidechain Input", juce::dontSendNotification);
    sidechainInputLabel.attachToComponent(&sidechainInputComboBox, true);
    addAndMakeVisible(sidechainInputLabel);

    sidechainInputAttachment.reset(new ComboBoxAttachment(valueTreeState, "sidechainInput", sidechainInputComboBox));

    //Input
    sidechainInputGainSlider.setTextValueSuffix(" dB");
    sidechainInputGainSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setInputGain(sidechainInputGainSlider.getValue());
    };
    addAndMakeVisible(sidechainInputGainSlider);

    sidechainInputGainLabel.setText("Sidechain Input Gain", juce::dontSendNotification);
    sidechainInputGainLabel.attachToComponent(&sidechainInputGainSlider, true);
    addAndMakeVisible(sidechainInputGainLabel);

    sidechainInputGainAttachment.reset(new SliderAttachment(valueTreeState, "sidechainInputGain", sidechainInputGainSlider));
    
    //Detector
    detectorComboBox.addItem("Peak", 1);
    detectorComboBox.addItem("Half Wave Rectifier", 2);
    detectorComboBox.addItem("Full Wave Rectifier", 3);
    detectorComboBox.addItem("LUFS", 4);
    detectorComboBox.onChange = [this]
    {
        audioProcessor.compressor.setDetectorMode(DetectorType(detectorComboBox.getSelectedId()));
    };
    addAndMakeVisible(detectorComboBox);

    detectorLabel.setText("Detector", juce::dontSendNotification);
    detectorLabel.attachToComponent(&detectorComboBox, true);
    addAndMakeVisible(detectorLabel);

    detectorAttachment.reset(new ComboBoxAttachment(valueTreeState, "detector", detectorComboBox));

    //Stereo Link
    stereoLinkToggle.onStateChange = [this]
    {
        audioProcessor.compressor.setStereoLink(stereoLinkToggle.getToggleState());
    };
    addAndMakeVisible(stereoLinkToggle);

    stereoLinkLabel.setText("Stereo Link", juce::dontSendNotification);
    stereoLinkLabel.attachToComponent(&stereoLinkToggle, true);
    addAndMakeVisible(stereoLinkLabel);

    stereoLinkAttachment.reset(new ButtonAttachment(valueTreeState, "stereoLink", stereoLinkToggle));

    //Threshold
    thresholdSlider.setTextValueSuffix(" dB");
    thresholdSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setThreshold(thresholdSlider.getValue());
    };
    addAndMakeVisible(thresholdSlider);

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, true);
    addAndMakeVisible(thresholdLabel);

    thresholdAttachment.reset(new SliderAttachment(valueTreeState, "threshold", thresholdSlider));

    //Ratio
    ratioSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setRatio(ratioSlider.getValue());
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
        audioProcessor.compressor.setAttack(attackSlider.getValue());
    };
    addAndMakeVisible(attackSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
    addAndMakeVisible(attackLabel);

    attackAttachment.reset(new SliderAttachment(valueTreeState, "attack", attackSlider));
   
    //Release
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setRelease(releaseSlider.getValue());
    };
    addAndMakeVisible(releaseSlider);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    addAndMakeVisible(releaseLabel);

    releaseAttachment.reset(new SliderAttachment(valueTreeState, "release", releaseSlider));

    //RMS
    RMSToggle.onStateChange = [this]
    {
        audioProcessor.compressor.setRMS(RMSToggle.getToggleState());
    };
    addAndMakeVisible(RMSToggle);

    RMSLabel.setText("Enable RMS", juce::dontSendNotification);
    RMSLabel.attachToComponent(&RMSToggle, true);
    addAndMakeVisible(RMSLabel);

    RMSAttachment.reset(new ButtonAttachment(valueTreeState, "RMS", RMSToggle));

    //RL
    RLToggle.onStateChange = [this]
    {
        audioProcessor.compressor.setRL(RLToggle.getToggleState());
    };
    addAndMakeVisible(RLToggle);

    RLLabel.setText("RL", juce::dontSendNotification);
    RLLabel.attachToComponent(&RLToggle, true);
    addAndMakeVisible(RLLabel);

    RLAttachment.reset(new ButtonAttachment(valueTreeState, "RL", RLToggle));

    //RL Linear Tau
    linearTauRLSlider.setTextValueSuffix(" ms");
    linearTauRLSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setLinearTauRL(linearTauRLSlider.getValue());
    };
    addAndMakeVisible(linearTauRLSlider);

    linearTauRLLabel.setText("RL Linear Tau", juce::dontSendNotification);
    linearTauRLLabel.attachToComponent(&linearTauRLSlider, true);
    addAndMakeVisible(linearTauRLLabel);

    linearTauRLAttachment.reset(new SliderAttachment(valueTreeState, "linearTauRL", linearTauRLSlider));

    //RL
    nonlinearityRLSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setSaturationRL(nonlinearityRLSlider.getValue());
    };
    addAndMakeVisible(nonlinearityRLSlider);
  
    nonlinearityRLLabel.setText("RL Nonlinearity", juce::dontSendNotification);
    nonlinearityRLLabel.attachToComponent(&nonlinearityRLSlider, true);
    addAndMakeVisible(nonlinearityRLLabel);

    nonlinearityRLAttachment.reset(new SliderAttachment(valueTreeState, "nonlinearityRL", nonlinearityRLSlider));

    //Wet
    wetSlider.setTextValueSuffix(" dB");
    wetSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setWetGain(wetSlider.getValue());
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
        audioProcessor.compressor.setDryGain(drySlider.getValue());
    };
    addAndMakeVisible(drySlider);

    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.attachToComponent(&drySlider, true);
    addAndMakeVisible(dryLabel);

    dryAttachment.reset(new SliderAttachment(valueTreeState, "dry", drySlider));

    //Window Size
    setSize (400, 2 * marginT + 15 * sliderDY);
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

    compressorLabel.setBounds(labelL, marginT, labelWidth, sliderHeight);

    sidechainInputComboBox.setBounds(marginL, marginT + sliderDY, sliderWidth, sliderHeight);
    sidechainInputGainSlider.setBounds(marginL, marginT + 2 * sliderDY, sliderWidth, sliderHeight);
    detectorComboBox.setBounds(marginL, marginT + 3 * sliderDY, sliderWidth, sliderHeight);
    stereoLinkToggle.setBounds(marginL, marginT + 4 * sliderDY, sliderWidth, sliderHeight);
    thresholdSlider.setBounds(marginL, marginT + 5 * sliderDY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(marginL, marginT + 6 * sliderDY, sliderWidth, sliderHeight);
    attackSlider.setBounds(marginL, marginT + 7 * sliderDY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(marginL, marginT + 8 * sliderDY, sliderWidth, sliderHeight);
    RMSToggle.setBounds(marginL, marginT + 9 * sliderDY, sliderWidth, sliderHeight);
    RLToggle.setBounds(marginL, marginT + 10 * sliderDY, labelWidth, sliderHeight);
    linearTauRLSlider.setBounds(marginL, marginT + 11 * sliderDY, sliderWidth, sliderHeight);
    nonlinearityRLSlider.setBounds(marginL, marginT + 12 * sliderDY, sliderWidth, sliderHeight);
    wetSlider.setBounds(marginL, marginT + 13 * sliderDY, sliderWidth, sliderHeight);
    drySlider.setBounds(marginL, marginT + 14 * sliderDY, sliderWidth, sliderHeight);
}
