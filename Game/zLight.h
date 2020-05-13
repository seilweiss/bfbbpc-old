#ifndef ZLIGHT_H
#define ZLIGHT_H

#include "xBase.h"
#include "zVolume.h"
#include "iLight.h"
#include "xEnv.h"

struct zLightAsset : xBaseAsset
{
    unsigned char lightType;
    unsigned char lightEffect;
    unsigned char lightPad[2];
    unsigned int lightFlags;
    float lightColor[4];
    xVec3 lightDir;
    float lightConeAngle;
    xSphere lightSphere;
    unsigned int attachID;
};

struct _zLight : xBase
{
    unsigned int flags;
    zLightAsset *tasset;
    iLight light;
    xBase *attached_to;
    int true_idx;
    float *reg;
    int effect_idx;
};

void zLightResetAll(xEnv *env);
void zLightInit(void *b, void *tasset);
void zLightResolveLinks();
void zLightSetVolume(zVolume *vol);

#endif