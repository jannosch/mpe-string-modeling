#include "SmoothedModulationSourceValue.h"

SmoothedModulationSourceValue::SmoothedModulationSourceValue(Decimal _secondsToReachTarget, Decimal initialValue,
                                                             Decimal _targetClosenessWithinDuration) :
        secondsToReachTarget(_secondsToReachTarget),
        targetClosenessWithinDuration(_targetClosenessWithinDuration),
        targetValue(initialValue),
        currentValue(initialValue)
{

}

void SmoothedModulationSourceValue::prepareToPlay(Decimal newSampleRate, Eigen::Index samplesPerBlock) {
    ModulationSourceValue::prepareToPlay(newSampleRate, samplesPerBlock);

    sampleRate = newSampleRate;
    updateTargetFractionPerSample();
}

void SmoothedModulationSourceValue::processBlock(const ModulationData* modulationData, int startSample, int numSamples) {
    jassert(sampleRate > 0);
    jassert(startSample >= 0);
    jassert(numSamples > 0 && startSample + numSamples <= values.size());
    juce::ignoreUnused(modulationData);

    if (juce::approximatelyEqual(targetValue, values(startSample), values(startSample + numSamples - 1))) {
        changedLastBlock = false;
        return;
    }
    changedLastBlock = true;

    for (Decimal& value : values) {
        currentValue = targetFractionPerSample * targetValue + oneMinusTargetFractionPerSample * currentValue;
        value = currentValue;
    }

}


void SmoothedModulationSourceValue::updateTargetFractionPerSample() {
    jassert(sampleRate > 0);
    jassert(targetClosenessWithinDuration >= 0 && targetClosenessWithinDuration < 1);

    targetFractionPerSample = pow(targetClosenessWithinDuration, 1.0f / sampleRate);
    oneMinusTargetFractionPerSample = 1 - targetFractionPerSample;
}

void SmoothedModulationSourceValue::setTarget(Decimal newTargetValue) {
    targetValue = newTargetValue;
}

void SmoothedModulationSourceValue::resetTo(Decimal newTargetAndCurrentValue) {
    targetValue = newTargetAndCurrentValue;
    currentValue = newTargetAndCurrentValue;
}

void SmoothedModulationSourceValue::setSecondsToReachTarget(Decimal newSecondsToReachTarget) {
    secondsToReachTarget = newSecondsToReachTarget;
    updateTargetFractionPerSample();
}
