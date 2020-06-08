#ifndef ZENV_H
#define ZENV_H

#include "xBase.h"
#include "xEnv.h"

struct xEnvAsset : xBaseAsset
{
    unsigned int bspAssetID;
    unsigned int startCameraAssetID;
    unsigned int climateFlags;
    float climateStrengthMin;
    float climateStrengthMax;
    unsigned int bspLightKit;
    unsigned int objectLightKit;
    float padF1;
    unsigned int bspCollisionAssetID;
    unsigned int bspFXAssetID;
    unsigned int bspCameraAssetID;
    unsigned int bspMapperID;
    unsigned int bspMapperCollisionID;
    unsigned int bspMapperFXID;
    float loldHeight;
};

struct _zEnv : xBase
{
    xEnvAsset *easset;
};

void zEnvInit(_zEnv *env, xEnvAsset *easset);
void zEnvInit(void *env, void *easset);
int zEnvEventCB(xBase *from, xBase *to, unsigned int toEvent, const float *toParam,
                xBase *toParamWidget);
void zEnvSetup(_zEnv *env);
void zEnvStartingCamera(_zEnv *env);
void zEnvRender(xEnv *env);

#endif