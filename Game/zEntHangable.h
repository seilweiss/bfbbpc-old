#ifndef ZENTHANGABLE_H
#define ZENTHANGABLE_H

#include "zEnt.h"

struct xEntHangableAsset
{
    unsigned int flags;
    float pivotOffset;
    float leverArm;
    float gravity;
    float accel;
    float decay;
    float grabDelay;
    float stopDecel;
};

struct zEntHangable : zEnt
{
    xEntHangableAsset *hangInfo;
    xVec3 pivot;
    xVec3 endpos;
    xVec3 vel;
    xVec3 swingplane;
    float grabTimer;
    float spin;
    unsigned int state;
    zEnt *shaggy;
    int enabled;
    zEnt *follow;
    int moving;
    float candle_timer;
    int candle_state;
};

void zEntHangable_SetupFX();
void zEntHangable_Init(void *ent, void *asset);

#endif