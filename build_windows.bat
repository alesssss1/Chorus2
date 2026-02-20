@echo off
setlocal

echo --- Mokk Unreal Chorus - Windows Build Script ---

set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%BuildWin
set JUCE_DIR=%PROJECT_DIR%JUCE

:: 1. Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: CMake non e' installato. Scaricalo da https://cmake.org/download/
    pause
    exit /b 1
)

:: 2. Download JUCE if missing
if not exist "%JUCE_DIR%" (
    echo Download di JUCE in corso...
    git clone --depth 1 https://github.com/juce-framework/JUCE.git "%JUCE_DIR%"
) else (
    echo JUCE gia' presente.
)

:: 3. Create Build Directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

:: 4. Configure and Build
echo Configurazione del progetto (richiede Visual Studio installato)...
cmake .. -D JUCE_BUILD_EXAMPLES=OFF -D JUCE_BUILD_EXTRAS=OFF

echo Compilazione in corso...
cmake --build . --config Release

:: 5. Success
if exist "MokkUnrealChorus_artefacts\Release\VST3\Mokk Unreal Chorus.vst3" (
    echo Successo! Il plugin e' stato creato.
    echo Puoi trovare il file .vst3 in: %BUILD_DIR%\MokkUnrealChorus_artefacts\Release\VST3
    echo Copialo nella tua cartella VST3 (solitamente C:\Program Files\Common Files\VST3).
) else (
    echo Errore durante la compilazione. Assicurati di avere Visual Studio installato.
)

pause
