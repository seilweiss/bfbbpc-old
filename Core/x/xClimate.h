#ifndef XCLIMATE_H
#define XCLIMATE_H

#include "zParEmitter.h"
#include "zEnv.h"

struct _tagRain
{
    int rain;
    float strength;
    zParEmitter *rain_emitter;
    zParEmitter *snow_emitter;
};

struct _tagWind
{
    float strength;
    float angle;
    xVec3 dir;
};

struct _tagClimate
{
    _tagRain rain;
    _tagWind wind;
};

void xClimateInit(_tagClimate *climate);
void xClimateInitAsset(_tagClimate *climate, xEnvAsset *easset);

#endif