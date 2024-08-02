#ifndef MPESYNTH_SMOOTHEDMODULATIONSOURCEVALUE_H
#define MPESYNTH_SMOOTHEDMODULATIONSOURCEVALUE_H


#include "../../Types.h"
#include "ModulationSourceValue.h"

class SmoothedModulationSourceValue : public ModulationSourceValue {
public:

    explicit SmoothedModulationSourceValue(Decimal _secondsToReachTarget, Decimal initialValue = 0, Decimal _targetClosenessWithinDuration = 0.05f);

    void prepareToPlay(Decimal newSampleRate, Eigen::Index samplesPerBlock) override;

    void processBlock(const ModulationData* modulationData, int startSample, int numSamples) override;

    void setTarget(Decimal newTargetValue);

    void resetTo(Decimal newTargetAndCurrentValue);

    void setSecondsToReachTarget(Decimal newSecondsToReachTarget);

protected:
    Decimal secondsToReachTarget;
    Decimal targetClosenessWithinDuration;

    Decimal sampleRate = 0;

    Decimal targetFractionPerSample = 0;
    Decimal oneMinusTargetFractionPerSample = 0;

    Decimal targetValue;
    Decimal currentValue;

    void updateTargetFractionPerSample();
};


#endif //MPESYNTH_SMOOTHEDMODULATIONSOURCEVALUE_H
