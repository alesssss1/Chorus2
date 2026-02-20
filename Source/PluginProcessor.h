#pragma once

#include <JuceHeader.h>

class MokkUnrealChorusAudioProcessor : public juce::AudioProcessor
{
public:
    MokkUnrealChorusAudioProcessor();
    ~MokkUnrealChorusAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Mokk Unreal Chorus"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP Components
    static constexpr int maxStages = 3;
    static constexpr int maxVoicesPerStage = 2; // Stereo
    
    struct ChorusVoice {
        juce::LinearSmoothedValue<float> delayBuffer[2]; // Stereo L/R delay lines
        float phase = 0.0f;
    };

    std::vector<juce::AudioBuffer<float>> delayBuffers;
    std::vector<int> writeIndices;
    
    float lastSampleRate = 44100.0f;
    
    // Parameters cached for performance
    std::atomic<float>* rateParam = nullptr;
    std::atomic<float>* depthParam = nullptr;
    std::atomic<float>* delayParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* stagesParam = nullptr;
    std::atomic<float>* spatialParam = nullptr;
    std::atomic<float>* analogParam = nullptr;

    float lfoPhase = 0.0f;

    // Tape Saturation / Analog Distortion
    float applyAnalogDistortion(float input, float drive);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MokkUnrealChorusAudioProcessor)
};
