#include "ADSR.h"
#include "ModulatedParameterFloat.h"

ADSR::ADSR() = default;

void ADSR::prepareToPlay(Decimal newSampleRate, Eigen::Index samplesPerBlock) {
    jassert(newSampleRate > 0); // Invalid sample rate
    sampleRate = newSampleRate;

    values = Eigen::ArrayX<Decimal>(samplesPerBlock);
    stateValues = List<Eigen::ArrayX<Decimal>>(4, Eigen::ArrayX<Decimal>(samplesPerBlock));
    stateValuesSet = List<bool>(4, false);

    reset();
}

void ADSR::processBlock(const ModulationData* modulationData, int startSample, int numSamples) {
    jassert(sampleRate > 0); // Call prepareToPlay before AudioProcessing
    jassert(stateParameters.size() == 4); // setParameters must be called before

    if (startSample == 0) {
        stateValuesSet = List<bool>(4, false);
    }

    stateValues[State::ATTACK ] = stateParameters[State::ATTACK ]->getModulated(modulationData, startSample, numSamples);
    stateValues[State::DECAY  ] = stateParameters[State::DECAY  ]->getModulated(modulationData, startSample, numSamples);
    stateValues[State::SUSTAIN] = stateParameters[State::SUSTAIN]->getModulated(modulationData, startSample, numSamples);
    stateValues[State::RELEASE] = stateParameters[State::RELEASE]->getModulated(modulationData, startSample, numSamples);

    switch (state) {
        case ATTACK:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue += 1 / (sampleRate * stateValues[State::ATTACK](i - startSample)); // When attack should not be shorter on re-trigger, replace 1 with stateStart
                values(i) = currentValue;

                if (currentValue >= 1) {
                    currentValue = 1;
                    values(i) = currentValue;
                    state = State::DECAY;
                    stateStart = currentValue;
                    if (i < startSample + numSamples) processBlock(modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }
            break;

        case DECAY:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue += (stateValues[State::SUSTAIN](i - startSample)-stateStart) / (sampleRate * stateValues[State::DECAY](i - startSample));
                values(i) = currentValue;

                if (currentValue <= stateValues[State::SUSTAIN](i - startSample)) {
                    currentValue = stateValues[State::SUSTAIN](i - startSample);
                    state = State::SUSTAIN;
                    stateStart = currentValue;
                    if (i < startSample + numSamples) processBlock(modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }

            break;

        case SUSTAIN:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue = stateValues[State::SUSTAIN](i - startSample);
                values(i) = currentValue;
            }
            break;

        case RELEASE:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue -= stateStart / (sampleRate * stateValues[State::RELEASE](i - startSample));
                values(i) = currentValue;

                if (currentValue <= 0) {
                    reset();
                    values(i) = currentValue;
                    if (i < startSample + numSamples) processBlock(modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }
            break;

        case OFF:
            currentValue = 0;
            values(Eigen::seqN(startSample, numSamples)).setZero();
            break;
    }
}


void ADSR::reset() {
    state = State::OFF;
    currentValue = 0;
}

void ADSR::noteOn() {
    state = State::ATTACK;
    stateStart = currentValue;
}

void ADSR::noteOff() {
    state = State::RELEASE;
    stateStart = currentValue;
}


// See: https://www.desmos.com/calculator/wmf37crmss
Eigen::ArrayX<Decimal> ADSR::toGainFactor(const Eigen::ArrayX<Decimal>& buffer) {
    return (1 + LOWEST_VOLUME_GAIN) * Eigen::pow(10.0, std::log10(LOWEST_VOLUME_GAIN / (1 + LOWEST_VOLUME_GAIN)) * (1 - buffer)) - LOWEST_VOLUME_GAIN;
}


ADSR::State ADSR::getCurrentState() const {
    return state;
}

void ADSR::setParameters(ModulatedParameterFloat *attackTimeParameter,
                         ModulatedParameterFloat *decayTimeParameter,
                         ModulatedParameterFloat *sustainLevelParameter,
                         ModulatedParameterFloat *releaseTimeParameter) {

    stateParameters = {attackTimeParameter, decayTimeParameter, sustainLevelParameter, releaseTimeParameter};
}
