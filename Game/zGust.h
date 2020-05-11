#ifndef ZGUST_H
#define ZGUST_H

#include "xBase.h"
#include "xMath3.h"
#include "zVolume.h"

struct zGustAsset : xBaseAsset
{
    unsigned int flags;
    unsigned int volumeID;
    unsigned int effectID;
    xVec3 vel;
    float fade;
    float partMod;
};

struct zGust : xBase
{
    unsigned int flags;
    zGustAsset *asset;
    zVolume *volume;
    zVolume *fx_volume;
    float debris_timer;
};

void zGustInit();
zGust *zGustGetGust(unsigned short n);

#endif