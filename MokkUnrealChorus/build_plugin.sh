#!/bin/bash

# Mokk Unreal Chorus - Build Script

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOURCE_DIR="$PROJECT_DIR/Source"
BUILD_DIR="$PROJECT_DIR/Build"
JUCE_DIR="$PROJECT_DIR/JUCE"

echo "--- Mokk Unreal Chorus Build Script ---"

# 1. Check for Dependencies
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake non è installato. Per favore installalo con 'brew install cmake' o dal sito ufficiale."
    exit 1
fi

if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode non è installato. Per favore installalo dall'App Store."
    exit 1
fi

# 2. Download JUCE if missing
if [ ! -d "$JUCE_DIR" ]; then
    echo "Download di JUCE in corso... (potrebbe volerci un minuto)"
    git clone --depth 1 https://github.com/juce-framework/JUCE.git "$JUCE_DIR"
else
    echo "JUCE già presente."
fi

# 3. Create CMake Project
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Configurazione del progetto..."
cmake .. -D JUCE_BUILD_EXAMPLES=OFF -D JUCE_BUILD_EXTRAS=OFF

# 4. Build Plugin
echo "Compilazione in corso..."
cmake --build . --config Release

# 5. Final Output
if [ -d "$BUILD_DIR/MokkUnrealChorus_artefacts/Release/VST3/MokkUnrealChorus.vst3" ]; then
    echo "Successo! Il plugin è stato creato."
    echo "Il file si trova in: $BUILD_DIR/MokkUnrealChorus_artefacts/Release/VST3/MokkUnrealChorus.vst3"
    echo "Copia questa cartella in /Library/Audio/Plug-Ins/VST3 per usarlo in Cubase."
else
    # Check deeper if path varies
    VST_PATH=$(find . -name "*.vst3" -type d | grep "MokkUnrealChorus" | head -n 1)
    if [ -n "$VST_PATH" ]; then
        echo "Successo! Il plugin è stato creato in: $(pwd)/$VST_PATH"
    else
        echo "Errore durante la compilazione. Controlla i log sopra."
    fi
fi
