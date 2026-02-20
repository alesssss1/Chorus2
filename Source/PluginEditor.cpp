#include "PluginProcessor.h"
#include "PluginEditor.h"

MokkUnrealChorusAudioEditor::MokkUnrealChorusAudioEditor(MokkUnrealChorusAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&modernLookAndFeel);

    setupSlider(delaySlider, delayLabel, "DELAY");
    delayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "delay", delaySlider);

    setupSlider(rateSlider, rateLabel, "RATE");
    rateAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "rate", rateSlider);

    setupSlider(depthSlider, depthLabel, "DEPTH");
    depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "depth", depthSlider);

    setupSlider(mixSlider, mixLabel, "MIX");
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "mix", mixSlider);

    setupSlider(analogSlider, analogLabel, "ANALOG");
    analogAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "analog", analogSlider);

    stagesBox.addItem("1 Stage", 1);
    stagesBox.addItem("2 Stages", 2);
    stagesBox.addItem("3 Stages", 3);
    addAndMakeVisible(stagesBox);
    stagesAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getAPVTS(), "stages", stagesBox);

    stagesLabel.setText("STAGES", juce::dontSendNotification);
    stagesLabel.setJustificationType(juce::Justification::centred);
    stagesLabel.setFont(juce::Font("Roboto", 12.0f, juce::Font::Plain));
    addAndMakeVisible(stagesLabel);

    addAndMakeVisible(spatialButton);
    spatialAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.getAPVTS(), "spatial", spatialButton);

    spatialLabel.setText("SPATIAL", juce::dontSendNotification);
    spatialLabel.setJustificationType(juce::Justification::centred);
    spatialLabel.setFont(juce::Font("Roboto", 12.0f, juce::Font::Plain));
    addAndMakeVisible(spatialLabel);

    titleLabel.setText("MOKK UNREAL CHORUS", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font("Project", 24.0f, juce::Font::Bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d1ff));
    addAndMakeVisible(titleLabel);

    setSize(600, 300);
}

MokkUnrealChorusAudioEditor::~MokkUnrealChorusAudioEditor()
{
    setLookAndFeel(nullptr);
}

void MokkUnrealChorusAudioEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font("Roboto", 12.0f, juce::Font::Plain));
    addAndMakeVisible(label);
}

void MokkUnrealChorusAudioEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0a0a0a)); // Dark background

    // Minimal grid or subtle decorations could go here
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawRect(getLocalBounds().reduced(10), 1.0f);
}

void MokkUnrealChorusAudioEditor::resized()
{
    auto area = getLocalBounds();
    
    titleLabel.setBounds(area.removeFromTop(60));
    
    auto mainArea = area.reduced(20);
    auto sliderWidth = mainArea.getWidth() / 5;

    delaySlider.setBounds(mainArea.removeFromLeft(sliderWidth).reduced(10));
    delayLabel.setBounds(delaySlider.getBounds().translated(0, 70).withHeight(20));

    rateSlider.setBounds(mainArea.removeFromLeft(sliderWidth).reduced(10));
    rateLabel.setBounds(rateSlider.getBounds().translated(0, 70).withHeight(20));

    depthSlider.setBounds(mainArea.removeFromLeft(sliderWidth).reduced(10));
    depthLabel.setBounds(depthSlider.getBounds().translated(0, 70).withHeight(20));

    mixSlider.setBounds(mainArea.removeFromLeft(sliderWidth).reduced(10));
    mixLabel.setBounds(mixSlider.getBounds().translated(0, 70).withHeight(20));

    analogSlider.setBounds(mainArea.removeFromLeft(sliderWidth).reduced(10));
    analogLabel.setBounds(analogSlider.getBounds().translated(0, 70).withHeight(20));

    // Bottom section for stages and spatial
    auto bottomArea = area.removeFromBottom(60).reduced(20, 0);
    stagesBox.setBounds(bottomArea.removeFromLeft(100).withHeight(20));
    stagesLabel.setBounds(stagesBox.getBounds().translated(0, -20));

    spatialButton.setBounds(bottomArea.removeFromRight(40).withHeight(20));
    spatialLabel.setBounds(spatialButton.getBounds().translated(0, -20));
}
