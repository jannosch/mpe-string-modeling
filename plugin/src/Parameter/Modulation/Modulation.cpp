#include <utility>
#include "Modulation.h"
#include "ModulatedParameterFloat.h"
#include "ModulationData.h"




Modulation::Modulation() : modulationSourceId(static_cast<size_t>(0)) {

}



Modulation::Modulation(size_t modulationSourceId, ModulatedParameterFloat* amount):
    modulationSourceId(modulationSourceId), amount(amount) {


}



Eigen::ArrayX<Decimal> Modulation::getModulatedNormalized(const ModulationData* modulationData, int startSample, int numSamples) {
    return modulationData->getById(modulationSourceId, startSample, numSamples) * amount->getModulated(modulationData, startSample, numSamples);
}



void Modulation::setModulatedParameterId(juce::String newModulatedParameterId) {
    modulatedParameterId = std::move(newModulatedParameterId);
}



juce::String Modulation::getModulatedParameterId() {
    return modulatedParameterId;
}



void Modulation::setModulationSourceId(size_t newModulationSourceId) {
    jassert(newModulationSourceId >= 0);
    modulationSourceId = newModulationSourceId;
}



void Modulation::setAmountParameter(ModulatedParameterFloat* newAmount) {
    jassert(newAmount != nullptr);
    amount = newAmount;
}



void Modulation::prepareToPlay(Eigen::Index newSamplesPerBlock) {
    jassert(newSamplesPerBlock > 0);
    samplesPerBlock = newSamplesPerBlock;
}
