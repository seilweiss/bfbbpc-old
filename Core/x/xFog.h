#ifndef XFOG_H
#define XFOG_H

#include "xBase.h"

struct xFogAsset : xBaseAsset
{
    unsigned char bkgndColor[4];
    unsigned char fogColor[4];
    float fogDensity;
    float fogStart;
    float fogStop;
    float transitionTime;
    unsigned char fogType;
    unsigned char padFog[3];
};

struct _xFog : xBase
{
    xFogAsset *tasset;
};

void xFogClearFog();
void xFogInit(void *b, void *tasset);

#endif