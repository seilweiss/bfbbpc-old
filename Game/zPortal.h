#ifndef ZPORTAL_H
#define ZPORTAL_H

#include "xBase.h"

struct xPortalAsset : xBaseAsset
{
    unsigned int assetCameraID;
    unsigned int assetMarkerID;
    float ang;
    unsigned int sceneID;
};

struct _zPortal : xBase
{
    xPortalAsset *passet;
};

void zPortalInit(void *portal, void *passet);

#endif