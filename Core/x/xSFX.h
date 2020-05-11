#ifndef XSFX_H
#define XSFX_H

#include "xBase.h"
#include "xMath3.h"

struct xSFXAsset : xBaseAsset
{
    unsigned short flagsSFX;
    unsigned short freq;
    float freqm;
    unsigned int soundAssetID;
    unsigned int attachID;
    unsigned char loopCount;
    unsigned char priority;
    unsigned char volume;
    unsigned char pad;
    xVec3 pos;
    float innerRadius;
    float outerRadius;
};

struct xSFX : xBase
{
    xSFXAsset *asset;
    unsigned int sndID;
    float cachedOuterDistSquared;
};

void xSFXInit(void *t, void *asset);

#endif