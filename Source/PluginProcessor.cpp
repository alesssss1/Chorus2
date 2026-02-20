#include "PluginProcessor.h"
#include "PluginEditor.h"

MokkUnrealChorusAudioProcessor::MokkUnrealChorusAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    rateParam = apvts.getRawParameterValue("rate");
    depthParam = apvts.getRawParameterValue("depth");
    delayParam = apvts.getRawParameterValue("delay");
    mixParam = apvts.getRawParameterValue("mix");
    stagesParam = apvts.getRawParameterValue("stages");
    spatialParam = apvts.getRawParameterValue("spatial");
    analogParam = apvts.getRawParameterValue("analog");
}

MokkUnrealChorusAudioProcessor::~MokkUnrealChorusAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MokkUnrealChorusAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("delay", "Delay", 0.0f, 50.0f, 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rate", "Rate", 0.1f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 100.0f, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("stages", "Stages", 1, 3, 1));
    params.push_back(std::make_unique<juce::AudioParameterBool>("spatial", "Spatial", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("analog", "Analog", 0.0f, 100.0f, 0.0f));

    return { params.begin(), params.end() };
}

void MokkUnrealChorusAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lastSampleRate = (float)sampleRate;
    
    // Allocate delay buffers: 3 stages * 2 channels (stereo)
    // Max delay of 0.1s is enough (100ms)
    int delayLength = (int)(sampleRate * 0.1) + 2;
    
    delayBuffers.clear();
    writeIndices.clear();
    
    for (int i = 0; i < maxStages; ++i)
    {
        juce::AudioBuffer<float> buf(2, delayLength);
        buf.clear();
        delayBuffers.push_back(std::move(buf));
        writeIndices.push_back(0);
    }
}

void MokkUnrealChorusAudioProcessor::releaseResources() {}

bool MokkUnrealChorusAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void MokkUnrealChorusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float rate = rateParam->load();
    float depth = depthParam->load() / 100.0f;
    float baseDelayMs = delayParam->load();
    int stages = (int)stagesParam->load();
    bool spatial = spatialParam->load() > 0.5f;
    float mix = mixParam->load() / 100.0f;
    float analog = analogParam->load() / 100.0f;

    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float dryL = channelDataL[sample];
        float dryR = channelDataR[sample];
        float wetL = 0.0f;
        float wetR = 0.0f;

        // LFO Update
        float lfo = std::sin(lfoPhase);
        lfoPhase += (2.0f * juce::MathConstants<float>::pi * rate) / lastSampleRate;
        if (lfoPhase >= 2.0f * juce::MathConstants<float>::pi) lfoPhase -= 2.0f * juce::MathConstants<float>::pi;

        for (int s = 0; s < stages; ++s)
        {
            auto& buf = delayBuffers[s];
            int& writeIdx = writeIndices[s];
            
            // Write input to delay line
            buf.setSample(0, writeIdx, dryL);
            buf.setSample(1, writeIdx, dryR);

            // Calculate modulated delay time
            float modL = lfo;
            float modR = spatial ? -lfo : lfo; // Inverse phase for spatial width

            float currentDelayL = baseDelayMs + (modL * depth * 5.0f); // 5ms deviation
            float currentDelayR = baseDelayMs + (modR * depth * 5.0f);

            auto readFromBuf = [&](int channel, float delayMs) {
                float delaySamples = delayMs * (lastSampleRate / 1000.0f);
                float readPos = (float)writeIdx - delaySamples;
                if (readPos < 0) readPos += buf.getNumSamples();

                int idx1 = (int)readPos;
                int idx2 = (idx1 + 1) % buf.getNumSamples();
                float frac = readPos - (float)idx1;

                return buf.getSample(channel, idx1) * (1.0f - frac) + buf.getSample(channel, idx2) * frac;
            };

            wetL += readFromBuf(0, currentDelayL);
            wetR += readFromBuf(1, currentDelayR);

            writeIdx = (writeIdx + 1) % buf.getNumSamples();
        }

        // Apply stages normalization
        wetL /= (float)stages;
        wetR /= (float)stages;

        // Apply Analog Saturation to wet signal only
        if (analog > 0.0f)
        {
            wetL = applyAnalogDistortion(wetL, analog * 2.0f);
            wetR = applyAnalogDistortion(wetR, analog * 2.0f);
        }

        // Mix
        channelDataL[sample] = dryL * (1.0f - mix) + wetL * mix;
        channelDataR[sample] = dryR * (1.0f - mix) + wetR * mix;
    }
}

float MokkUnrealChorusAudioProcessor::applyAnalogDistortion(float input, float drive)
{
    // Simple soft clipping with slight asymmetry for "tape" feel
    float x = input * (1.0f + drive);
    // x = x - (x * x * x / 3.0f); // Standard soft clipper
    
    // Better tape approximation (tanh is common)
    return std::tanh(x);
}

void MokkUnrealChorusAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MokkUnrealChorusAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* MokkUnrealChorusAudioProcessor::createEditor()
{
    return new MokkUnrealChorusAudioEditor(*this);
}

// Factory function for JUCE
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MokkUnrealChorusAudioProcessor();
}
