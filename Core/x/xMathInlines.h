#ifndef XMATHINLINES_H
#define XMATHINLINES_H

#include "xMath.h"
#include <cmath>

inline float xatan2(float y, float x)
{
    return xAngleClampFast(std::atan2f(y, x));
}

inline float xsqrt(float x)
{
    return std::sqrtf(x);
}

inline float xfmod(float x, float y)
{
    return std::fmodf(x, y);
}

#endif