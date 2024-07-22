#include "Synthesizer.h"


Synthesizer::Synthesizer(juce::MPEInstrument &instrument_) : juce::MPESynthesiser(instrument_) {

}



void Synthesizer::prepareToPlay(Decimal newSampleRate, Eigen::Index newSamplesPerBlock) {
    juce::MPESynthesiser::setCurrentPlaybackSampleRate(newSampleRate);
    samplesPerBlock = newSamplesPerBlock;

    for (auto mpeVoice : voices) {
        if (auto* voice = dynamic_cast<Voice*>(mpeVoice)) {
            voice->prepareToPlay(newSampleRate, newSamplesPerBlock);
        }
    }
}



List<Voice *> Synthesizer::getActiveVoices() {
    List<Voice*> activeVoices;

    for (auto mpeVoice : voices) {
        if (auto *voice = dynamic_cast<Voice *>(mpeVoice)) {
            if (voice->isActiveThisBlock()) {
                activeVoices.append(voice);
            }
        }
    }

    return activeVoices;
}



Eigen::ArrayX<Decimal> Synthesizer::generateNextBlock() {
    auto samples = Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();

    getActiveVoices().forEach([&](Voice* v){ samples += v->generateNextBlock(); });

    return samples;
}
