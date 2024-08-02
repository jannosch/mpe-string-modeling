#pragma once

#include "Modulation.h"
#include "ModulationData.h"

class ModulatedParameterFloat : public juce::AudioParameterFloat {
public:

    List<std::shared_ptr<Modulation>> modulations;



    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, Decimal sliderSmoothingSeconds = 0.100f);


    void processBlock();

    Eigen::ArrayX<Decimal> convertFrom0to1(const Eigen::ArrayX<Decimal> &values0to1);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData* modulationData, int startSample, int numSamples);

    Eigen::ArrayX<Decimal> getModulated(const ModulationData* modulationData, int startSample, int numSamples);

    Eigen::ArrayX<Decimal> getModulated(const List<ModulationData*>& modulationDataList, int startSample, int numSamples);

    void prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock);

    ModulatedParameterFloat* withModulation(const std::shared_ptr<Modulation>& modulation);

protected:

    Eigen::ArrayX<Decimal> bufferNormalizedSliderValue;

    Decimal sliderSmoothingSeconds{0.100f};
    juce::SmoothedValue<Decimal> smoothedNormalizedSliderValue;


    // Listener
    void valueChanged(float newValue) override;

};