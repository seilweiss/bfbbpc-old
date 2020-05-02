#ifndef XFFX_H
#define XFFX_H

#include "xScene.h"

typedef void(*xFFXDoEffectCallBack)(xEnt *, xScene *, float, void *);

struct xFFX
{
    unsigned int flags;
    xFFXDoEffectCallBack doEffect;
    void *fdata;
    xFFX *next;
};

#endif