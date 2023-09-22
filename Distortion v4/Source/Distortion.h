/*
  ==============================================================================

    Distortion.h
    Created: 20 Sep 2023 6:00:09pm
    Author:  Senne Hawer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum class DistortionModel
{
    kHard,
    kSoft,
    kSaturation
};

template <typename SampleType>
class Distortion
{
public:
    Distortion();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock      = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples  = outputBlock.getNumSamples();

        jassert (inputBlock.getNumChannels() == numChannels);
        jassert (inputBlock.getNumSamples()  == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom (inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples  = inputBlock .getChannelPointer (channel);
            auto* outputSamples = outputBlock.getChannelPointer (channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample (inputSamples[i]);
        }
    }
    
    SampleType processSample(SampleType inputSample) noexcept
    {
        return processHardClipping(inputSample);
    }
    
    SampleType processHardClipping(SampleType inputSample)
    {
        auto drySignal = inputSample;
        auto wetSignal = inputSample * juce::Decibels::decibelsToGain(_input.getNextValue());
        
        float treshold = _treshold.getNextValue();
         if (std::abs(wetSignal) > treshold)
         {
             wetSignal = treshold;
         }
        
        auto output = (1.0f - _mix.getNextValue()) * drySignal + _mix.getNextValue() * wetSignal;
        
        return output * juce::Decibels::decibelsToGain(_output.getNextValue());
    }
    
    void setDrive(SampleType newDrive);
    void setMix(SampleType newMix);
    void setOutput(SampleType newOutput);
    void setTreshold(SampleType newTreshold);
    
    void setDistortionModel(DistortionModel newModel);
    
private:
    
    juce::SmoothedValue<float> _input;
    juce::SmoothedValue<float> _mix;
    juce::SmoothedValue<float> _output;
    juce::SmoothedValue<float> _treshold;
    
    float _sampleRate = 44100.0f;
    
    DistortionModel _currentDistortionModel = DistortionModel::kHard;
};
