#ifndef QSYNTHI2_MODULATIONDATA_H
#define QSYNTHI2_MODULATIONDATA_H

#include "../../Types.h"
#include "ModulationSourceValue.h"
#include "SmoothedModulationSourceValue.h"
#include "ADSR.h"


class ModulationData {

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
        inline static const Source NOTE_OFF_VELOCITY{"Note-Off-Velocity", 1};
        inline static const Source KEY{"Key", 2};
        inline static const Source X{"Keyboard X", 3};
        inline static const Source Y{"Keyboard Y", 4};
        inline static const Source Z{"Keyboard Z", 5};
        inline static const Source X_RELATIVE{"Keyboard X (relative)", 6};
        inline static const Source Y_RELATIVE{"Keyboard Y (relative)", 7};
        inline static const Source Y_CENTERED{"Keyboard Y (centered)", 8};
        inline static const Source Z_CENTERED{"Keyboard Z (centered)", 9};
        inline static const Source ENVELOPE1{"Envelope 1 (amp)", 10};
        inline static const Source ENVELOPE2{"Envelope 2", 11};

        inline static List<Source> ALL = List<Source>({VELOCITY, NOTE_OFF_VELOCITY, KEY, X, Y, Z, X_RELATIVE, Y_RELATIVE, Y_CENTERED, Z_CENTERED, ENVELOPE1, ENVELOPE2});
    };


    ModulationData();

    void prepareToPlay(Decimal sampleRate, Eigen::Index samplesPerBlock);

    [[nodiscard]] bool isSourceValid(const Source& source) const;

    static Decimal frequencyToModulationValue(Decimal frequency);

    static Decimal relativeFrequencyToModulationValue(Decimal baseFrequency, Decimal relativeFrequency);

    void processSources(int startSample, int numSamples);

    void noteStarted(juce::MPENote note, bool alreadyActive = false);

    void noteStopped(juce::MPENote note, bool allowTailOff = true);

    void notePitchbendChanged(juce::MPENote note);

    void noteTimbreChanged(juce::MPENote note);

    void notePressureChanged(juce::MPENote note);

    [[nodiscard]] Eigen::ArrayX<Decimal> getBySource(const ModulationData::Source &source, int startSample, int numSamples) const;

    [[nodiscard]] Eigen::ArrayX<Decimal> getById(size_t id, int startSample, int numSamples) const;


public:

    SmoothedModulationSourceValue velocity{0.001};
    SmoothedModulationSourceValue noteOffVelocity{0.001}; // TODO: Rethink data type
    SmoothedModulationSourceValue key{0.001}; // TODO: Rethink data type
    SmoothedModulationSourceValue x{0.050};
    SmoothedModulationSourceValue y{0.050};
    SmoothedModulationSourceValue z{0.050};
    SmoothedModulationSourceValue xRelative{0.050};
    SmoothedModulationSourceValue yRelative{0.050};
    SmoothedModulationSourceValue yCentered{0.050};
    SmoothedModulationSourceValue zCentered{0.050};
    ADSR envelope1;
    ADSR envelope2;

protected:

    List<ModulationSourceValue*> modulationSourceValues = {&velocity, &noteOffVelocity, &key, &x, &y, &z, &xRelative, &yRelative, &yCentered, &zCentered, &envelope1, &envelope2};

    Decimal initialFrequency = 0;
    Decimal initialY = 0;
};



#endif //QSYNTHI2_MODULATIONDATA_H
