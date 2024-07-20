#ifndef TYPES_H
#define TYPES_H

// suppress warnings: https://stackoverflow.com/a/28166605
#ifdef __GNUC__
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wshadow"
#else
#pragma clang diagnostic ignored "-Wshadow"
#endif
#endif

#include "Juce.h"
#include "Container/List.h"
#include "Container/Vector.h"
#include "Eigen.h"

typedef double Decimal;


#endif //TYPES_H