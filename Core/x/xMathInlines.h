#ifndef XMATHINLINES_H
#define XMATHINLINES_H

#include "xMath.h"
#include <cmath>

#define xsqr(x) ((x) * (x))

#define xmin(a, b) (((a) < (b)) ? (a) : (b))
#define xmax(a, b) (((b) < (a)) ? (a) : (b))

#define xclamp(x, a, b) (xmax(xmin((x), (b)), (a)))

inline float xasin(float x)
{
    return std::asinf(x);
}

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

inline float xacos(float x)
{
    return std::acosf(x);
}

inline float xfloor(float x)
{
    return std::floorf(x);
}

inline float xceil(float x)
{
    return std::ceilf(x);
}

#endif