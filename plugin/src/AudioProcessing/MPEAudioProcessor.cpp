#include "MPEAudioProcessor.h"
#include "../Data.h"

extern Data sharedData;

MPEAudioProcessor::MPEAudioProcessor() {
    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice(new Voice());

}



void MPEAudioProcessor::prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock) {
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;

    sharedData.parameters->prepareToPlay(newSampleRate, newSamplesPerBlock);
    synth.prepareToPlay(newSampleRate, newSamplesPerBlock);
}



void MPEAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {

    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Process Audio
    auto samples = synth.generateNextBlock();

    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            buffer.addSample(channel, i, static_cast<float>(samples[i]));
        }
    }

}
