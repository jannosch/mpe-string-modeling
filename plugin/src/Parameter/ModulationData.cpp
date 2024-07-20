#include "ModulationData.h"

void ModulationData::prepareToPlay(Eigen::Index samplesPerBlock) {
    this->clear();

    for (size_t i = 0; i < Sources::ALL.size(); i++) {
        this->push_back(Eigen::ArrayX<Decimal>(samplesPerBlock).setZero());
    }
}

void ModulationData::write(const ModulationData::Source &source, juce::SmoothedValue<Decimal> &smoothedValue,
                           int startSample, int numSamples) {
    jassert(isSourceValid(source)); // ModulationData::Source is not valid

    Eigen::ArrayX<Decimal> data = this->at(source.id);
    Decimal currentValue = smoothedValue.getCurrentValue();
    Decimal targetValue = smoothedValue.getTargetValue();

    if (juce::approximatelyEqual(currentValue, targetValue) && juce::approximatelyEqual(data(0), targetValue)) {
        return;
    }

    for (int i = startSample; i < startSample + numSamples; i++) {
        data(i) = smoothedValue.getNextValue();
    }

    this->at(source.id) = data;
}
