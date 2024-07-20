#ifndef QSYNTHI2_MODULATIONDATA_H
#define QSYNTHI2_MODULATIONDATA_H

#include "../Types.h"
#include "ValueProvider.h"


class ModulationData : public List<Eigen::ArrayX<Decimal>> {

public:

    static constexpr Decimal PITCH_MODULATION_NEUTRAL_FREQUENCY = static_cast<Decimal>(110);

    class Source {
    public:

        explicit Source(juce::String newName, size_t newId) : name(std::move(newName)), id(newId) { }

        const juce::String name;
        const size_t id;

    };

    class Sources {
    public:
        inline static const Source VELOCITY{"Velocity", 0};
        inline static const Source X{"Keyboard X", 1};
        inline static const Source Y{"Keyboard Y", 2};
        inline static const Source Z{"Keyboard Z", 3};
        inline static const Source X_RELATIVE{"Keyboard X (relative)", 4};
        inline static const Source Y_RELATIVE{"Keyboard Y (relative)", 5};
        inline static const Source Y_CENTERED{"Keyboard Y (centered)", 6};
        inline static const Source ENVELOPE1{"Envelope 1 (amp)", 7};

        inline static List<Source> ALL = List<Source>({VELOCITY, X, Y, Z, X_RELATIVE, Y_RELATIVE, Y_CENTERED, ENVELOPE1});
    };


    ModulationData() : List<Eigen::ArrayX<Decimal>>() {
    }

    void prepareToPlay(Eigen::Index samplesPerBlock);

    void write(const Source& source, juce::SmoothedValue<Decimal> &smoothedValue, int startSample, int numSamples);

    [[nodiscard]] bool isSourceValid (const Source& source) const {
        return source.id >= 0 && source.id < this->size() && this->at(source.id).size() > 0;
    }



    Eigen::ArrayX<Decimal> &atSource(const Source &source) {
        return this->at(source.id);
    }

};



#endif //QSYNTHI2_MODULATIONDATA_H
