#include "Voice.h"
#include "../Data.h"


extern Data sharedData;


Voice::Voice() {

}



Decimal Voice::frequencyToModulationValue(double frequency) {
    return log2(frequency / ModulationData::PITCH_MODULATION_NEUTRAL_FREQUENCY);
}



Decimal Voice::relativeFrequencyToModulationValue(double frequency) {
    return 24 * log2(frequency / initialFrequency);
}



void Voice::noteStarted() {
    jassert (currentlyPlayingNote.isValid());
    jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
             || currentlyPlayingNote.keyState == juce::MPENote::sustained
             || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);


    notePitchbendChanged();

    if (envelope1.getCurrentState() == ADSR::State::OFF) {
        // New note triggerd: Full reset

        initialFrequency = juce::MidiMessage::getMidiNoteInHertz(currentlyPlayingNote.initialNote);
        initialY = static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat());

        x.setCurrentAndTargetValue(frequencyToModulationValue(currentlyPlayingNote.getFrequencyInHertz()));
        y.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()));
        z.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
        xRelative.setCurrentAndTargetValue(0);
        yRelative.setCurrentAndTargetValue(0);
        yCentered.setCurrentAndTargetValue(currentlyPlayingNote.timbre.asSignedFloat());
        velocity.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));
    } else {
        // Voice-Stealing: Soft reset
        x.setTargetValue(frequencyToModulationValue(currentlyPlayingNote.getFrequencyInHertz()));
        y.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()));
        z.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
        xRelative.setTargetValue(0);
        yRelative.setTargetValue(0);
        yCentered.setTargetValue(currentlyPlayingNote.timbre.asSignedFloat());
        velocity.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));
    }


    envelope1.noteOn();

}

void Voice::noteStopped(bool allowTailOff) {
    jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

    velocity.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    envelope1.noteOff();

    if (!allowTailOff) {
        envelope1.reset();
        return;
    }

}


void Voice::notePitchbendChanged() {
    x.setTargetValue(frequencyToModulationValue(currentlyPlayingNote.getFrequencyInHertz()));
    xRelative.setTargetValue(relativeFrequencyToModulationValue(currentlyPlayingNote.getFrequencyInHertz()));
}



void Voice::noteTimbreChanged() {
    y.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()));
    yRelative.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()) - initialY);
    yCentered.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asSignedFloat()));

}



void Voice::notePressureChanged() {
    z.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
}



void Voice::prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock) {
    jassert(newSampleRate > 0);
    jassert(newSamplesPerBlock > 0);
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;

    noteStopped(false);
    juce::MPESynthesiserVoice::setCurrentSampleRate(static_cast<double>(sampleRate));

    velocity.reset(sampleRate, 0.030);
    x.reset(sampleRate, 0.030);
    y.reset(sampleRate, 0.100);
    z.reset(sampleRate, 0.100);
    xRelative.reset(sampleRate, 0.040);
    yRelative.reset(sampleRate, 0.100);
    yCentered.reset(sampleRate, 0.100);

    modulationData.prepareToPlay(samplesPerBlock);

    envelope1.setParameters(sharedData.parameters->envelope1Attack, sharedData.parameters->envelope1Decay, sharedData.parameters->envelope1Sustain, sharedData.parameters->envelope1Release);
    envelope1.prepareToPlay(sampleRate, samplesPerBlock);

}



void Voice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    juce::ignoreUnused(outputBuffer);

    activeThisBlock = true;

    modulationData.write(ModulationData::Sources::VELOCITY, velocity, startSample, numSamples);
    modulationData.write(ModulationData::Sources::X, x, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Y, y, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Z, z, startSample, numSamples);
    modulationData.write(ModulationData::Sources::X_RELATIVE, xRelative, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Y_RELATIVE, yRelative, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Y_CENTERED, yCentered, startSample, numSamples);

    envelope1.processBlock(&modulationData.at(ModulationData::Sources::ENVELOPE1.id), modulationData, startSample, numSamples);

    if (envelope1.getCurrentState() == ADSR::State::OFF) {
        clearCurrentNote();
    }
}



Eigen::ArrayX<Decimal> Voice::generateNextBlock() {
    jassert(samplesPerBlock > 0);

    activeThisBlock = false;

    //juce::Logger::writeToLog(juce::String(modulationData.atSource(ModulationData::Sources::ENVELOPE1)(Eigen::last)) + " -> " + juce::String(envelope1.toGainFactor(modulationData.atSource(ModulationData::Sources::ENVELOPE1))(Eigen::last)));

    Eigen::ArrayX<Decimal> phaseIncrements = sharedData.parameters->baseFrequency->getModulated(modulationData) / sampleRate;
    Eigen::ArrayX<Decimal> phases0To1 = Eigen::ArrayX<Decimal>(samplesPerBlock);

    for (Eigen::Index i = 0; i < samplesPerBlock; i++) {
        phases0To1(i) = currentPhase0To1;
        currentPhase0To1 += phaseIncrements(i);
    }

    return (juce::MathConstants<Decimal>::twoPi * (phases0To1 + sharedData.parameters->timbre->getModulated(modulationData) * (juce::MathConstants<Decimal>::twoPi * phases0To1).sin())).sin() * envelope1.toGainFactor(modulationData.atSource(ModulationData::Sources::ENVELOPE1));
}



bool Voice::isActiveThisBlock() const {
    return activeThisBlock;
}

ModulationData *Voice::getModulationData() {
    return &modulationData;
}



