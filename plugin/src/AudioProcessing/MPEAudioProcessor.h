#ifndef MPESYNTH_MPEAUDIOPROCESSOR_H
#define MPESYNTH_MPEAUDIOPROCESSOR_H

#include "../Juce.h"
#include "../Eigen.h"
#include "../Synthesizer/Synthesizer.h"

class MPEAudioProcessor {
public:

    MPEAudioProcessor();

    void prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages);

protected:

    double sampleRate = 0;
    Eigen::Index samplesPerBlock = 0;

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesizer synth { instrument };


};


#endif //MPESYNTH_MPEAUDIOPROCESSOR_H
