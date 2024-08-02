#ifndef MPESYNTH_MODULATIONSOURCE_H
#define MPESYNTH_MODULATIONSOURCE_H

#include "../../Types.h"

class ModulationData;

class ModulationSourceValue {
public:

    virtual void prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock) {
        jassert(sampleRate > 0);
        jassert(samplesPerBlock > 0);
        values = Eigen::ArrayX<Decimal>(samplesPerBlock);
    }

    virtual void processBlock(const ModulationData* modulationData, int startSample, int numSamples) = 0;

    [[nodiscard]] inline Eigen::ArrayX<Decimal> get(int startSample, int numSamples) const {
        return values(Eigen::seqN(startSample, numSamples));
    }

    [[nodiscard]] bool hasChangedLastBlock() const {
        return changedLastBlock;
    }

protected:

    Eigen::ArrayX<Decimal> values;
    bool changedLastBlock = true;

};

#endif //MPESYNTH_MODULATIONSOURCE_H
