#pragma once


#include "Parameters.h"
#include "ExponentialRange.h"

class ParameterCollection : public Parameters {
public:

    ModulatedParameterFloat* timbre = add<ModulatedParameterFloat>(
            "Timbre",
            juce::NormalisableRange<float>(0, 1, 0, 1.f, false),
            0);

    ModulatedParameterFloat* volume = add<ModulatedParameterFloat>(
            "Volume",
            juce::NormalisableRange<float>(0, 1, 0, 1.f, false),
            0.75f);


    ModulatedParameterFloat* envelope1Attack = add<ModulatedParameterFloat>(
            "Envelope 1 attack seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.008f);
    ModulatedParameterFloat* envelope1Decay = add<ModulatedParameterFloat>(
            "Envelope 1 decay seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.100f);
    ModulatedParameterFloat* envelope1Sustain = add<ModulatedParameterFloat>(
            "Envelope 1 sustain level",
            juce::NormalisableRange<float>(0, 1, 0, 1, false),
            1);
    ModulatedParameterFloat* envelope1Release = add<ModulatedParameterFloat>(
            "Envelope 1 release seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.100f);


    ModulatedParameterFloat* envelope2Attack = add<ModulatedParameterFloat>(
            "Envelope 2 attack seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.008f);
    ModulatedParameterFloat* envelope2Decay = add<ModulatedParameterFloat>(
            "Envelope 2 decay seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.100f);
    ModulatedParameterFloat* envelope2Sustain = add<ModulatedParameterFloat>(
            "Envelope 2 sustain level",
            juce::NormalisableRange<float>(0, 1, 0, 1, false),
            1);
    ModulatedParameterFloat* envelope2Release = add<ModulatedParameterFloat>(
            "Envelope 2 release seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .35f, false),
            0.100f);


    ModulatedParameterFloat* frequency = add<ModulatedParameterFloat>(
            "Frequency",
            ExponentialRange(2.5, 2560),
            110);

    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(12, frequency);
    }
};