#include "ModulationData.h"
#include "../../Data.h"

extern Data sharedData;

ModulationData::ModulationData() {
    jassert(Sources::ALL.size() == modulationSourceValues.size()); // Lists must have corresponding Values
}

void ModulationData::prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock) {
    envelope1.setParameters(sharedData.parameters->envelope1Attack, sharedData.parameters->envelope1Decay, sharedData.parameters->envelope1Sustain, sharedData.parameters->envelope1Release);
    envelope2.setParameters(sharedData.parameters->envelope2Attack, sharedData.parameters->envelope2Decay, sharedData.parameters->envelope2Sustain, sharedData.parameters->envelope2Release);

    for (ModulationSourceValue* modulationSourceValue : modulationSourceValues) {
        modulationSourceValue->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

bool ModulationData::isSourceValid(const ModulationData::Source &source) const {
    return source.id >= 0 && source.id < modulationSourceValues.size();
}


Decimal ModulationData::frequencyToModulationValue(Decimal frequency) {
    return log2(frequency / ModulationData::PITCH_MODULATION_NEUTRAL_FREQUENCY);
}

Decimal ModulationData::relativeFrequencyToModulationValue(Decimal baseFrequency, Decimal relativeFrequency) {
    return 24 * log2(relativeFrequency / baseFrequency);
}

void ModulationData::processSources(int startSample, int numSamples) {
    for (ModulationSourceValue* modulationSourceValue : modulationSourceValues) {
        modulationSourceValue->processBlock(this, startSample, numSamples);
    }
}

void ModulationData::noteStarted(juce::MPENote note, bool alreadyActive) {

    initialFrequency = static_cast<Decimal>(juce::MidiMessage::getMidiNoteInHertz(note.initialNote));
    initialY = static_cast<Decimal>(note.timbre.asUnsignedFloat());
    noteOffVelocity.resetTo(0);

    if (alreadyActive) {
        // Voice-Stealing: Soft reset
        key.setTarget(frequencyToModulationValue(static_cast<Decimal>(initialFrequency)));
        x.setTarget(frequencyToModulationValue(static_cast<Decimal>(note.getFrequencyInHertz())));
        y.setTarget(static_cast<Decimal>(note.timbre.asUnsignedFloat()));
        z.setTarget(static_cast<Decimal>(note.pressure.asUnsignedFloat()));
        xRelative.setTarget(0);
        yRelative.setTarget(0);
        yCentered.setTarget(note.timbre.asSignedFloat());
        zCentered.setTarget(note.pressure.asSignedFloat());
        velocity.setTarget(static_cast<Decimal>(note.noteOnVelocity.asUnsignedFloat()));
    } else {
        // New Note
        key.resetTo(frequencyToModulationValue(static_cast<Decimal>(initialFrequency)));
        x.resetTo(frequencyToModulationValue(static_cast<Decimal>(note.getFrequencyInHertz())));
        y.resetTo(static_cast<Decimal>(note.timbre.asUnsignedFloat()));
        z.resetTo(static_cast<Decimal>(note.pressure.asUnsignedFloat()));
        xRelative.resetTo(0);
        yRelative.resetTo(0);
        yCentered.resetTo(note.timbre.asSignedFloat());
        zCentered.resetTo(note.pressure.asSignedFloat());
        velocity.resetTo(static_cast<Decimal>(note.noteOnVelocity.asUnsignedFloat()));
    }

    envelope1.noteOn();
    envelope2.noteOn();
}

void ModulationData::noteStopped(juce::MPENote note, bool allowTailOff) {

    noteOffVelocity.setTarget(static_cast<Decimal>(note.noteOffVelocity.asUnsignedFloat()));

    envelope1.noteOff();
    envelope2.noteOff();

    if (!allowTailOff) {
        envelope1.reset();
        envelope2.reset();
    }
}

void ModulationData::notePitchbendChanged(juce::MPENote note) {
    x.setTarget(frequencyToModulationValue(static_cast<Decimal>(note.getFrequencyInHertz())));
    xRelative.setTarget(relativeFrequencyToModulationValue(static_cast<Decimal>(note.getFrequencyInHertz()), initialFrequency));
}

void ModulationData::noteTimbreChanged(juce::MPENote note) {
    y.setTarget(static_cast<Decimal>(note.timbre.asUnsignedFloat()));
    yRelative.setTarget(static_cast<Decimal>(note.timbre.asUnsignedFloat()) - initialY);
    yCentered.setTarget(static_cast<Decimal>(note.timbre.asSignedFloat()));
}

void ModulationData::notePressureChanged(juce::MPENote note) {
    z.setTarget(static_cast<Decimal>(note.pressure.asUnsignedFloat()));
    zCentered.setTarget(static_cast<Decimal>(note.pressure.asSignedFloat()));
}

Eigen::ArrayX<Decimal> ModulationData::getBySource(const ModulationData::Source &source, int startSample, int numSamples) const {
    jassert(isSourceValid(source));
    return modulationSourceValues[source.id]->get(startSample, numSamples);
}

Eigen::ArrayX<Decimal> ModulationData::getById(size_t id, int startSample, int numSamples) const {
    jassert(id > 0 && id < modulationSourceValues.size());
    return modulationSourceValues[id]->get(startSample, numSamples);
}

