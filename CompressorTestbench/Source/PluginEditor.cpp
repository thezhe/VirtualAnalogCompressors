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
    //Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setRange(0.1, 250);
    attackSlider.setSkewFactorFromMidPoint(50);
    attackSlider.setTextValueSuffix(" ms");
    attackSlider.onValueChange = [this] {
        audioProcessor.ffvcaIIR.setAttack(attackSlider.getValue());
        audioProcessor.ffvcaTPTz.setAttack(attackSlider.getValue());
        audioProcessor.ffvcaTPT.setAttack(attackSlider.getValue());
        audioProcessor.fbvcaIIR.setAttack(attackSlider.getValue());
        audioProcessor.fbvcaTPTz.setAttack(attackSlider.getValue());
    };
    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
   
    //Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setRange(0.1, 500);
    releaseSlider.setSkewFactorFromMidPoint(100);
    releaseSlider.setTextValueSuffix(" ms");
    releaseSlider.onValueChange = [this] {
        audioProcessor.ffvcaIIR.setRelease(releaseSlider.getValue());
        audioProcessor.ffvcaTPTz.setRelease(releaseSlider.getValue());
        audioProcessor.ffvcaTPT.setRelease(releaseSlider.getValue());
        audioProcessor.fbvcaIIR.setRelease(releaseSlider.getValue());
        audioProcessor.fbvcaTPTz.setRelease(releaseSlider.getValue());
    };
    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);
    
    //Threshold
    addAndMakeVisible(thresholdSlider);
    thresholdSlider.setRange(-60, 0);
    thresholdSlider.setTextValueSuffix(" dB");
    thresholdSlider.onValueChange = [this] {
        audioProcessor.ffvcaIIR.setThreshold(thresholdSlider.getValue());
        audioProcessor.ffvcaTPTz.setThreshold(thresholdSlider.getValue());
        audioProcessor.ffvcaTPT.setThreshold(thresholdSlider.getValue());
        audioProcessor.fbvcaIIR.setThreshold(thresholdSlider.getValue());
        audioProcessor.fbvcaTPTz.setThreshold(thresholdSlider.getValue());
    };
    addAndMakeVisible(thresholdLabel);
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, true);
 
    //Ratio
    addAndMakeVisible(ratioSlider);
    ratioSlider.setRange(1, 100);
    ratioSlider.onValueChange = [this] {
        audioProcessor.ffvcaIIR.setRatio(ratioSlider.getValue());
        audioProcessor.ffvcaTPTz.setRatio(ratioSlider.getValue());
        audioProcessor.ffvcaTPT.setRatio(ratioSlider.getValue());
        audioProcessor.fbvcaIIR.setRatio(ratioSlider.getValue());
        audioProcessor.fbvcaTPTz.setRatio(ratioSlider.getValue());
    };
    addAndMakeVisible(ratioLabel);
    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, true);

    //Makeup
    addAndMakeVisible(wetSlider);
    wetSlider.setRange(-90, 24);
    wetSlider.setTextValueSuffix(" dB");
    wetSlider.onValueChange = [this]{
        audioProcessor.ffvcaIIR.setWet(wetSlider.getValue());
        audioProcessor.ffvcaTPTz.setWet(wetSlider.getValue());
        audioProcessor.ffvcaTPT.setWet(wetSlider.getValue());
        audioProcessor.fbvcaIIR.setWet(wetSlider.getValue());
        audioProcessor.fbvcaTPTz.setWet(wetSlider.getValue());
    };
    addAndMakeVisible(wetLabel);
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.attachToComponent(&wetSlider, true);

    //Dry
    addAndMakeVisible(drySlider);
    drySlider.setRange(-90, 0);
    drySlider.setTextValueSuffix(" dB");
    drySlider.onValueChange = [this] {
        audioProcessor.ffvcaIIR.setDry(drySlider.getValue());
        audioProcessor.ffvcaTPTz.setDry(drySlider.getValue());
        audioProcessor.ffvcaTPT.setDry(drySlider.getValue());
        audioProcessor.fbvcaIIR.setDry(drySlider.getValue());
        audioProcessor.fbvcaTPTz.setDry(drySlider.getValue());
    };
    addAndMakeVisible(dryLabel);
    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.attachToComponent(&drySlider, true);

    //Topology
    addAndMakeVisible(topologyComboBox);
    topologyComboBox.addItem("FF (IIR)", 1);
    topologyComboBox.addItem("FF (TPTz)", 2);
    topologyComboBox.addItem("FF (TPT)", 3);
    topologyComboBox.addItem("FB (IIR)", 4);
    topologyComboBox.addItem("FB (TPTz)", 5);
    topologyComboBox.onChange = [this] { audioProcessor.setCompressor(topologyComboBox.getSelectedId()); };
    addAndMakeVisible(topologyLabel);
    topologyLabel.setText("Topology", juce::dontSendNotification);
    topologyLabel.attachToComponent(&topologyComboBox, true);

    //Use sendNotificationSync or else initial values do not trigger onValueChanged Lambdas properly
    //https://forum.juce.com/t/slider-onvaluechange-callback-behaves-unexpectedly/32677/4
    //initial values
    attackSlider.setValue(75, juce::sendNotificationSync);
    releaseSlider.setValue(150, juce::sendNotificationSync);
    thresholdSlider.setValue(-20, juce::sendNotificationSync);
    ratioSlider.setValue(100, juce::sendNotificationSync);
    wetSlider.setValue(0, juce::sendNotificationSync);
    drySlider.setValue(-90, juce::sendNotificationSync);
    topologyComboBox.setSelectedId(3, juce::sendNotificationSync);

    setSize (400, 2 * marginT + 7 * sliderDY);
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

    attackSlider.setBounds(marginL, marginT, sliderWidth, sliderHeight);
    releaseSlider.setBounds(marginL, marginT + sliderDY, sliderWidth, sliderHeight);
    thresholdSlider.setBounds(marginL, marginT + 2 * sliderDY, sliderWidth, sliderHeight);
    ratioSlider.setBounds(marginL, marginT + 3 * sliderDY, sliderWidth, sliderHeight);
    wetSlider.setBounds(marginL, marginT + 4 * sliderDY, sliderWidth, sliderHeight);
    drySlider.setBounds(marginL, marginT + 5 * sliderDY, sliderWidth, sliderHeight);
    topologyComboBox.setBounds(marginL, marginT + 6 * sliderDY, sliderWidth, sliderHeight);
}
