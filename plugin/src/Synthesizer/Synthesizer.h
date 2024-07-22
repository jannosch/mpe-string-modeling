#pragma once
#include "Voice.h"


class Synthesizer : public juce::MPESynthesiser {
public:

    explicit Synthesizer(juce::MPEInstrument& instrument_);

    void prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock);

    List<Voice*> getActiveVoices();

    Eigen::ArrayX<Decimal> generateNextBlock();


protected:

    Eigen::Index samplesPerBlock = 0;

};