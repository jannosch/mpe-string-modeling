#pragma once

#include "../Types.h"
#include "../Parameter/Modulation/ModulationData.h"


class Voice : public juce::MPESynthesiserVoice {
public:

    explicit Voice();

    void noteStarted() override;

    void noteStopped(bool allowTailOff) override;

    void notePitchbendChanged() override;

    void noteTimbreChanged() override;

    void notePressureChanged() override;

    void noteKeyStateChanged() override {}

    void prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock);

    // Does the preprocessing of midi and modulation sources
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    ModulationData* getModulationData();

    [[nodiscard]] bool isActiveThisBlock() const;


protected:

    // TEMPORARY
    Decimal currentPhase0To1 = 0.0;

    Decimal sampleRate = 0;
    Eigen::Index samplesPerBlock = 0;

    bool activeThisBlock = false;

    ModulationData modulationData;


};