#pragma once


#include "Parameters.h"
#include "ModulatedParameterFloat.h"
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
            0.8f);

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


    ModulatedParameterFloat* baseFrequency = add<ModulatedParameterFloat>(
            "Frequency",
            ExponentialRange(2.5, 2560),
            110);

    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(12, baseFrequency->getParameterID());
    }
};