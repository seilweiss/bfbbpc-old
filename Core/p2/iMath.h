#ifndef IMATH_H
#define IMATH_H

#include <cmath>

#define EPSILON 0.00001f

// Returns true if two floats are equal, or close to equal, to each other
#define CLOSEENOUGH(x, y) (iabs((x) - (y)) <= EPSILON)
#define CLOSEENOUGH2(x, y) (iabs((x) - (y)) < EPSILON)

inline float isin(float x)
{
    return std::sinf(x);
}

inline float icos(float x)
{
    return std::cosf(x);
}

inline float itan(float x)
{
    return std::tanf(x);
}

inline float iabs(float x)
{
    return std::fabsf(x);
}

#ifdef GAMECUBE
#define iabs(x) (float)__fabs(x)
#endif

#endif