#ifndef XVOLUME_H
#define XVOLUME_H

#include "xBase.h"
#include "xBound.h"

struct xVolumeAsset : xBaseAsset
{
    unsigned int flags;
    xBound bound;
    float rot;
    float xpivot;
    float zpivot;
};

struct xVolume : xBase
{
    xVolumeAsset *asset;
};

#endif