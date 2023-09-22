/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Distortionv4AudioProcessor::Distortionv4AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
, _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    _treeState.addParameterListener(inputID, this);
    _treeState.addParameterListener(mixID, this);
    _treeState.addParameterListener(outputID, this);
    _treeState.addParameterListener(tresholdID, this);
}

Distortionv4AudioProcessor::~Distortionv4AudioProcessor()
{
    _treeState.removeParameterListener(inputID, this);
    _treeState.removeParameterListener(mixID, this);
    _treeState.removeParameterListener(outputID, this);
    _treeState.removeParameterListener(tresholdID, this);
}

//LAYOUT
juce::AudioProcessorValueTreeState::ParameterLayout Distortionv4AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //create parameter pointers
    auto pDrive = std::make_unique<juce::AudioParameterFloat>(inputID, inputName, 0.0f, 24.0f, 0.0f);
    auto pMix = std::make_unique<juce::AudioParameterFloat>(mixID, mixName, 0.0f, 1.0f, 0.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>(outputID, outputName, -24.f, 24.0f, 0.0f);
    auto pTreshold = std::make_unique<juce::AudioParameterFloat>(tresholdID, tresholdName, 0.0f, 1.0f, 1.0f);
    
    //add to vector
    params.push_back(std::move(pDrive));
    params.push_back(std::move(pMix));
    params.push_back(std::move(pOutput));
    params.push_back(std::move(pTreshold));
    
    //create layout
    juce::AudioProcessorValueTreeState::ParameterLayout paramLayout {params.begin(), params.end()};
    
    //return
    return paramLayout;
}

void Distortionv4AudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    updateParameters();
}

void Distortionv4AudioProcessor::updateParameters()
{
    _distortionModule.setDrive(_treeState.getRawParameterValue(inputID)->load());
    _distortionModule.setMix(_treeState.getRawParameterValue(mixID)->load());
    _distortionModule.setOutput(_treeState.getRawParameterValue(outputID)->load());
    _distortionModule.setTreshold(_treeState.getRawParameterValue(tresholdID)->load());
}

//==============================================================================
const juce::String Distortionv4AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Distortionv4AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Distortionv4AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Distortionv4AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Distortionv4AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Distortionv4AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Distortionv4AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Distortionv4AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Distortionv4AudioProcessor::getProgramName (int index)
{
    return {};
}

void Distortionv4AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Distortionv4AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();
    
    _distortionModule.prepare(spec);
    
    updateParameters();
}

void Distortionv4AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Distortionv4AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Distortionv4AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block {buffer};
    _distortionModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool Distortionv4AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Distortionv4AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Distortionv4AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Distortionv4AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Distortionv4AudioProcessor();
}
