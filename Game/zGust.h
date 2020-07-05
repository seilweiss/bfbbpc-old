#ifndef ZGUST_H
#define ZGUST_H

#include "xBase.h"
#include "xMath3.h"
#include "zVolume.h"
#include "xEnt.h"

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

struct zGustData
{
    unsigned int gust_on;
    zGust *g[4];
    float lerp[4];
};

void zGustInit();
void zGustSetup();
zGust *zGustGetGust(unsigned short n);
void zGustUpdateEnt(xEnt *ent, xScene *, float dt, void *gdata);
void zGustUpdateFX(float seconds);

#endif