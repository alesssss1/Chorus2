#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Modern Minimal LookAndFeel
class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff00d1ff));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff2d2d2d));
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xff1a1a1a));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = 2.0f;
        auto arcRadius = radius - lineW * 0.5f;

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(fill);
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Minimal Dot Thumb
        auto thumbWidth = 4.0f;
        juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                      bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    }
};

class MokkUnrealChorusAudioEditor : public juce::AudioProcessorEditor
{
public:
    MokkUnrealChorusAudioEditor(MokkUnrealChorusAudioProcessor&);
    ~MokkUnrealChorusAudioEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MokkUnrealChorusAudioProcessor& audioProcessor;
    ModernLookAndFeel modernLookAndFeel;

    juce::Slider delaySlider, rateSlider, depthSlider, mixSlider, analogSlider;
    juce::ComboBox stagesBox;
    juce::ToggleButton spatialButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttach, rateAttach, depthAttach, mixAttach, analogAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stagesAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> spatialAttach;

    juce::Label delayLabel, rateLabel, depthLabel, mixLabel, analogLabel, stagesLabel, spatialLabel, titleLabel;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MokkUnrealChorusAudioEditor)
};
