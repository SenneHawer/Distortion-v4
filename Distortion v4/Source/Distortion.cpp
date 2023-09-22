/*
  ==============================================================================

    Distortion.cpp
    Created: 20 Sep 2023 6:00:09pm
    Author:  Senne Hawer

  ==============================================================================
*/

#include "Distortion.h"

template <typename SampleType>
Distortion<SampleType>::Distortion()
{
    
}

template <typename SampleType>
void Distortion<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    _sampleRate = spec.sampleRate;
    reset();
}

template <typename SampleType>
void Distortion<SampleType>::reset()
{
    if (_sampleRate <= 0) return;
    
    _input.reset(_sampleRate, 0.02f);
    _input.setTargetValue(0.0f);
    
    _mix.reset(_sampleRate, 0.02f);
    _mix.setTargetValue(100.0f);
    
    _output.reset(_sampleRate, 0.02f);
    _output.setTargetValue(0.0f);
    
    _treshold.reset(_sampleRate, 0.02f);
    _treshold.setTargetValue(0.0f);
}

template <typename SampleType>
void Distortion<SampleType>::setDrive(SampleType newDrive)
{
    _input.setTargetValue(newDrive);
}
template <typename SampleType>
void Distortion<SampleType>::setMix(SampleType newMix)
{
    _mix.setTargetValue(newMix);
}
template <typename SampleType>
void Distortion<SampleType>::setOutput(SampleType newOutput)
{
    _output.setTargetValue(newOutput);
}
template <typename SampleType>
void Distortion<SampleType>::setTreshold(SampleType newTreshold)
{
    _treshold.setTargetValue(newTreshold);
}


template <typename SampleType>
void Distortion<SampleType>::setDistortionModel(DistortionModel newModel)
{
    _currentDistortionModel = newModel;
        
    //call Update
}

template class Distortion<float>;
template class Distortion<double>;
