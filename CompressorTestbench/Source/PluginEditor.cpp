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

    /*
    //add and make visible
    addAndMakeVisible(sliderAttack);
    addAndMakeVisible(sliderRelease);
    addAndMakeVisible(sliderThreshold);
    addAndMakeVisible(sliderRatio);
    addAndMakeVisible(boxTopology);
    boxTopology.addItem("Trad", 1);
    boxTopology.addItem("TPTz", 1);
    boxTopology.addItem("TPT", 1);

    //Lambdas
    sliderAttack.onValueChange = [this] {
        audioProcessor.ffvcaTrad.setAttack(sliderAttack.getValue()); 
        audioProcessor.ffvcaTPTz.setAttack(sliderAttack.getValue());
        audioProcessor.ffvcaTPT.setAttack(sliderAttack.getValue());
    };
    sliderRelease.onValueChange = [this] {
        audioProcessor.ffvcaTrad.setRelease(sliderRelease.getValue());
        audioProcessor.ffvcaTPTz.setRelease(sliderRelease.getValue());
        audioProcessor.ffvcaTPT.setRelease(sliderRelease.getValue());
    };
    sliderThreshold.onValueChange = [this] {
        audioProcessor.ffvcaTrad.setThreshold(sliderThreshold.getValue());
        audioProcessor.ffvcaTPTz.setThreshold(sliderThreshold.getValue());
        audioProcessor.ffvcaTPT.setThreshold(sliderThreshold.getValue());
    };
    sliderRatio.onValueChange = [this] {
        audioProcessor.ffvcaTrad.setRatio(sliderRatio.getValue());
        audioProcessor.ffvcaTPTz.setRatio(sliderRatio.getValue());
        audioProcessor.ffvcaTPT.setRatio(sliderRatio.getValue());
    };
    boxTopology.onChange = [this] { audioProcessor.setCompressor(boxTopology.getSelectedId()); };
    
    //Initial values
    sliderAttack.setValue(75);
    sliderRelease.setValue(150);
    sliderThreshold.setValue(-20);
    sliderRelease.setValue(100);
    boxTopology.setSelectedId(3);
    */
    setSize (400, 300);
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
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CompressorTestbenchAudioProcessorEditor::resized()
{
    //sliderAttack.setBounds()
}
