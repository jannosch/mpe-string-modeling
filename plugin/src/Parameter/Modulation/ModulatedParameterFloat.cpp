#include "ModulatedParameterFloat.h"
#include "../Parameters.h"
#include "ModulationData.h"


ModulatedParameterFloat::ModulatedParameterFloat() :
        juce::AudioParameterFloat(juce::ParameterID("", Parameters::VERSION), "", juce::NormalisableRange<float>(), 0) {

}

ModulatedParameterFloat::ModulatedParameterFloat(const juce::String &newName, const juce::NormalisableRange<float> &newRange,
                                                 float defaultValue, Decimal sliderSmoothingSeconds) :
        juce::AudioParameterFloat(juce::ParameterID(newName, Parameters::VERSION), newName, newRange, defaultValue),
        sliderSmoothingSeconds(sliderSmoothingSeconds),
        smoothedNormalizedSliderValue(newRange.convertTo0to1(defaultValue)) {

}



void ModulatedParameterFloat::processBlock() {
    Decimal currentValue = smoothedNormalizedSliderValue.getCurrentValue();
    Decimal targetValue = smoothedNormalizedSliderValue.getTargetValue();
    if (juce::approximatelyEqual(currentValue, targetValue) && juce::approximatelyEqual(bufferNormalizedSliderValue[0], targetValue)) {
        // Buffer would not really change
        return;
    }

    // Writes the raw value in the Buffer
    for (Decimal & i : bufferNormalizedSliderValue) {
        i = smoothedNormalizedSliderValue.getNextValue();
    }
}


Eigen::ArrayX<Decimal> ModulatedParameterFloat::convertFrom0to1(const Eigen::ArrayX<Decimal> &values0to1) {
    return values0to1.unaryExpr([this](Decimal decimal) {
        return static_cast<Decimal>(juce::AudioParameterFloat::convertFrom0to1(static_cast<float>(decimal)));
    });
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulatedNormalized(const ModulationData* modulationData, int startSample, int numSamples) {
    Eigen::ArrayX<Decimal> buffer = bufferNormalizedSliderValue(Eigen::seqN(startSample, numSamples));

    for (const auto& modulation : modulations) {
        Eigen::ArrayX<Decimal> result = modulation->getModulatedNormalized(modulationData, startSample, numSamples);
        buffer += result;
    }

    return buffer;
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const ModulationData* modulationData, int startSample, int numSamples) {
    return convertFrom0to1(getModulatedNormalized(modulationData, startSample, numSamples));
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const List<ModulationData*>& modulationDataList, int startSample, int numSamples) {
    auto buffer = bufferNormalizedSliderValue;

    for (const auto& modulationData : modulationDataList) {
        for (const auto& modulation : modulations) {
            auto modulationValues = modulation->getModulatedNormalized(modulationData, startSample, numSamples);

            jassert(buffer.size() == modulationValues.size());
            buffer += modulationValues *
                    modulationData->getBySource(ModulationData::Sources::ENVELOPE1, startSample, numSamples);
        }
    }

    return convertFrom0to1(buffer);
}



void ModulatedParameterFloat::valueChanged(float newValue) {
    smoothedNormalizedSliderValue.setTargetValue(static_cast<Decimal>(convertTo0to1(newValue)));
}


void ModulatedParameterFloat::prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock) {
    smoothedNormalizedSliderValue.reset(sampleRate, sliderSmoothingSeconds);
    bufferNormalizedSliderValue = Eigen::ArrayX<Decimal>(samplesPerBlock);
}

ModulatedParameterFloat *ModulatedParameterFloat::withModulation(const std::shared_ptr<Modulation> &modulation) {
    modulations.append(modulation);
    return this;
}
