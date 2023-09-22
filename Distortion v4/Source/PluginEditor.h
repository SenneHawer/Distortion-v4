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
class Distortionv4AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Distortionv4AudioProcessorEditor (Distortionv4AudioProcessor&);
    ~Distortionv4AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Distortionv4AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distortionv4AudioProcessorEditor)
};
