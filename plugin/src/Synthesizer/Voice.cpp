#include "Voice.h"
#include "../Data.h"
#include "../Parameter/Modulation/ModulatedParameterFloat.h"


extern Data sharedData;


Voice::Voice() = default;

void Voice::noteStarted() {
    jassert (currentlyPlayingNote.isValid());
    jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
             || currentlyPlayingNote.keyState == juce::MPENote::sustained
             || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);

    modulationData.noteStarted(currentlyPlayingNote, modulationData.envelope1.getCurrentState() != ADSR::State::OFF);

}

void Voice::noteStopped(bool allowTailOff) {
    jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

    modulationData.noteStopped(currentlyPlayingNote, allowTailOff);

}


void Voice::notePitchbendChanged() {
    modulationData.notePitchbendChanged(currentlyPlayingNote);
}



void Voice::noteTimbreChanged() {
    modulationData.noteTimbreChanged(currentlyPlayingNote);
}



void Voice::notePressureChanged() {
    modulationData.notePressureChanged(currentlyPlayingNote);
}



void Voice::prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock) {
    jassert(newSampleRate > 0);
    jassert(newSamplesPerBlock > 0);
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;

    noteStopped(false);
    juce::MPESynthesiserVoice::setCurrentSampleRate(static_cast<double>(sampleRate));

    modulationData.prepareToPlay(sampleRate, newSamplesPerBlock);
}



void Voice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    juce::ignoreUnused(outputBuffer);
    modulationData.processSources(startSample, numSamples);

    Eigen::ArrayX<Decimal> phaseIncrements = sharedData.parameters->frequency->getModulated(&modulationData, startSample, numSamples) / sampleRate;
    Eigen::ArrayX<Decimal> phases0To1(numSamples);

    for (Eigen::Index i = 0; i < numSamples; i++) {
        phases0To1(i) = currentPhase0To1;
        currentPhase0To1 += phaseIncrements(i);
    }

    Eigen::ArrayXf waveform = ((juce::MathConstants<Decimal>::twoPi * (phases0To1 + sharedData.parameters->timbre->getModulated(&modulationData, startSample, numSamples) * (juce::MathConstants<Decimal>::twoPi * phases0To1).sin())).sin() * modulationData.envelope1.toGainFactor(
            modulationData.getBySource(ModulationData::Sources::ENVELOPE1, startSample, numSamples)) * modulationData.envelope1.toGainFactor(sharedData.parameters->volume->getModulated(&modulationData, startSample, numSamples))).cast<float>();

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
        auto outputArray = Eigen::Map<Eigen::ArrayXf>(outputBuffer.getWritePointer(channel) + startSample, numSamples);
        outputArray += waveform;
    }


    if (modulationData.envelope1.getCurrentState() == ADSR::State::OFF) {
        clearCurrentNote();
    }
}



bool Voice::isActiveThisBlock() const {
    return activeThisBlock;
}

ModulationData *Voice::getModulationData() {
    return &modulationData;
}



