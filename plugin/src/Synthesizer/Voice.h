#pragma once

#include "../Types.h"
#include "../Parameter/Modulation.h"
#include "ADSR.h"


class Voice : public juce::MPESynthesiserVoice {
public:

    explicit Voice();

    static Decimal frequencyToModulationValue(double frequency);
    Decimal relativeFrequencyToModulationValue(double frequency);

    void noteStarted() override;

    void noteStopped (bool allowTailOff) override;

    void notePitchbendChanged() override;

    void noteTimbreChanged() override;

    void notePressureChanged() override;

    void noteKeyStateChanged() override {}

    void prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock);

    // Does the preprocessing of midi and modulation sources
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    using MPESynthesiserVoice::renderNextBlock; // TODO: Why? Necessary?

    // Generates the audio
    Eigen::ArrayX<Decimal> generateNextBlock();

    ModulationData* getModulationData();

    [[nodiscard]] bool isActiveThisBlock() const;


protected:

    // TEMPORARY
    Decimal currentPhase0To1 = 0.0;

    Decimal sampleRate = 0;
    Eigen::Index samplesPerBlock = 0;

    bool activeThisBlock = false;

    ModulationData modulationData;

    juce::SmoothedValue<Decimal> velocity;
    juce::SmoothedValue<Decimal> x;
    juce::SmoothedValue<Decimal> y;
    juce::SmoothedValue<Decimal> z;
    juce::SmoothedValue<Decimal> xRelative;
    juce::SmoothedValue<Decimal> yRelative;
    juce::SmoothedValue<Decimal> yCentered;

    Decimal initialFrequency;
    Decimal initialY;

    ADSR envelope1;


};