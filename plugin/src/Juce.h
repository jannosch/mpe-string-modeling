#ifndef JUCE_H
#define JUCE_H

// Avoid library warnings: https://stackoverflow.com/a/6321977
#ifdef __GNUC__
#ifndef __clang__
#pragma GCC system_header
#else
#pragma clang system_header
#endif
#endif

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"

namespace juce {

    template <typename Type>
    inline bool approximatelyEqual(Type a, Type b, Type c) {
        return approximatelyEqual(a, b) && approximatelyEqual(a, c);
    }

}

#endif //JUCE_H
