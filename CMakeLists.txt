cmake_minimum_required(VERSION 3.22)

project(SIDECHAIN_PRO VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(SIDECHAIN_PRO
    COMPANY_NAME "Arti520K"
    BUNDLE_ID "com.arti520k.sidechainpro"
    PLUGIN_MANUFACTURER_CODE Arti
    PLUGIN_CODE ScP1
    FORMATS VST3
    PRODUCT_NAME "SIDECHAIN PRO"
    DESCRIPTION "External sidechain compressor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(SIDECHAIN_PRO PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp
)

target_compile_definitions(SIDECHAIN_PRO PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SIDECHAIN_PRO
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
