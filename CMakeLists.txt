cmake_minimum_required(VERSION 3.15)

project(MokkUnrealChorus VERSION 1.0.0 LANGUAGES CXX)

# Add JUCE
add_subdirectory(JUCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

juce_add_plugin(MokkUnrealChorus
    VERSION 1.0.0
    COMPANY_NAME "Mokk"
    PLUGIN_NAME "Unreal Chorus"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_STEP TRUE
    FORMATS VST3
    PRODUCT_NAME "Mokk Unreal Chorus"
)

target_sources(MokkUnrealChorus PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(MokkUnrealChorus PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MokkUnrealChorus PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_data_structures
)
