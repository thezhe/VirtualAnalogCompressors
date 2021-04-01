/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorTestbenchAudioProcessorEditor::CompressorTestbenchAudioProcessorEditor (CompressorTestbenchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //Compressor label
    addAndMakeVisible(compressorLabel);
    compressorLabel.setText("Compressor Testbench", juce::dontSendNotification);

    //Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setRange(1, 100);
    attackSlider.setSkewFactorFromMidPoint(25);
    attackSlider.setTextValueSuffix(" ms");
    attackSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setAttack(attackSlider.getValue());
    };
    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
   
    //Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setRange(1, 500);
    releaseSlider.setSkewFactorFromMidPoint(100);
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setRelease(releaseSlider.getValue());
    };
    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    
    //Threshold
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setRange(-60, 0);
    thresholdSlider.setTextValueSuffix(" dB");
    thresholdSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setThreshold(thresholdSlider.getValue());
    };
    addAndMakeVisible(thresholdLabel);
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, true);
 
    //Ratio
    addAndMakeVisible(ratioSlider);
    ratioSlider.setRange(1, 100);
    ratioSlider.setSkewFactorFromMidPoint(5);
    ratioSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setRatio(ratioSlider.getValue());
    };
    addAndMakeVisible(ratioLabel);
    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, true);

    //Wet
    addAndMakeVisible(wetSlider);
    wetSlider.setRange(-90, 24);
    wetSlider.setTextValueSuffix(" dB");
    wetSlider.onValueChange = [this]
    {
        audioProcessor.compressor.setWet(wetSlider.getValue());
    };
    addAndMakeVisible(wetLabel);
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, true);

    //Dry
    addAndMakeVisible(drySlider);
    drySlider.setRange(-90, 0);
    drySlider.setTextValueSuffix(" dB");
    drySlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setDry(drySlider.getValue());
    };
    addAndMakeVisible(dryLabel);
    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.attachToComponent(&drySlider, true);

    //Sidechain
    addAndMakeVisible(sidechainComboBox);
    sidechainComboBox.addItem("Feedforward", 1);
    sidechainComboBox.addItem("Feedback", 2);
    sidechainComboBox.addItem("Sidechain", 3);
    sidechainComboBox.onChange = [this] 
    { 
        audioProcessor.compressor.setSidechainType(CompressorSidechainType(sidechainComboBox.getSelectedId())); 
    };
    addAndMakeVisible(sidechainLabel);
    sidechainLabel.setText("Topology", juce::dontSendNotification);
    sidechainLabel.attachToComponent(&sidechainComboBox, true);

    //RL Toggle
    addAndMakeVisible(rlToggle);
    rlToggle.onStateChange = [this]
    {
        audioProcessor.compressor.enableRL(rlToggle.getToggleState());
    };
    addAndMakeVisible(rlToggleLabel);
    rlToggleLabel.setText("RL", juce::dontSendNotification);
    rlToggleLabel.attachToComponent(&rlToggle, true);

    //Linear Tau RL
    addAndMakeVisible(linearTauRLSlider);
    linearTauRLSlider.setRange(1, 100);
    linearTauRLSlider.setSkewFactorFromMidPoint(25);
    linearTauRLSlider.setTextValueSuffix(" ms");
    linearTauRLSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setLinearTauRL(linearTauRLSlider.getValue());
    };
    addAndMakeVisible(linearTauRLLabel);
    linearTauRLLabel.setText("Linear Tau", juce::dontSendNotification);
    linearTauRLLabel.attachToComponent(&linearTauRLSlider, true);

    //Saturation
    addAndMakeVisible(saturationRLSlider);
    saturationRLSlider.setRange(0, 500);
    saturationRLSlider.setSkewFactorFromMidPoint(50);
    saturationRLSlider.onValueChange = [this] 
    {
        audioProcessor.compressor.setSaturationRL(saturationRLSlider.getValue());
    };
    addAndMakeVisible(saturationRLLabel);
    saturationRLLabel.setText("Saturation", juce::dontSendNotification);
    saturationRLLabel.attachToComponent(&saturationRLSlider, true);

    //Use sendNotificationSync or else initial values do not trigger onValueChanged Lambdas properly
    //https://forum.juce.com/t/slider-onvaluechange-callback-behaves-unexpectedly/32677/4
    //initial values
    attackSlider.setValue(75, juce::sendNotificationSync);
    releaseSlider.setValue(150, juce::sendNotificationSync);
    thresholdSlider.setValue(-20, juce::sendNotificationSync);
    ratioSlider.setValue(100, juce::sendNotificationSync);
    wetSlider.setValue(0, juce::sendNotificationSync);
    drySlider.setValue(-90, juce::sendNotificationSync);
    sidechainComboBox.setSelectedId(1, juce::sendNotificationSync);
    
    rlToggle.setToggleState(false, juce::sendNotificationSync);
    linearTauRLSlider.setValue(15, juce::sendNotificationSync);
    saturationRLSlider.setValue(0, juce::sendNotificationSync);

    setSize (400, 2 * marginT + 11 * sliderDY);
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
    attackSlider.setBounds(marginL, marginT + sliderDY, sliderWidth, sliderHeight);
    releaseSlider.setBounds(marginL, marginT + 2 * sliderDY, sliderWidth, sliderHeight);
    thresholdSlider.setBounds(marginL, marginT + 3 * sliderDY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(marginL, marginT + 4 * sliderDY, sliderWidth, sliderHeight);
    wetSlider.setBounds(marginL, marginT + 5 * sliderDY, sliderWidth, sliderHeight);
    drySlider.setBounds(marginL, marginT + 6 * sliderDY, sliderWidth, sliderHeight);
    sidechainComboBox.setBounds(marginL, marginT + 7 * sliderDY, sliderWidth, sliderHeight);

    rlToggle.setBounds(labelL, marginT + 8 * sliderDY, labelWidth, sliderHeight);
    linearTauRLSlider.setBounds(marginL, marginT + 9 * sliderDY, sliderWidth, sliderHeight);
    saturationRLSlider.setBounds(marginL, marginT + 10 * sliderDY, sliderWidth, sliderHeight);
}
