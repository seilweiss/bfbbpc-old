#ifndef XENTBOULDER_H
#define XENTBOULDER_H

#include "xEnt.h"

struct xEntBoulderAsset
{
    float gravity;
    float mass;
    float bounce;
    float friction;
    float statFric;
    float maxVel;
    float maxAngVel;
    float stickiness;
    float bounceDamp;
    unsigned int flags;
    float killtimer;
    unsigned int hitpoints;
    unsigned int soundID;
    float volume;
    float minSoundVel;
    float maxSoundVel;
    float innerRadius;
    float outerRadius;
};

struct xEntBoulder : xEnt
{
    xEntBoulderAsset *basset;
    xShadowSimpleCache simpShadow_embedded;
    xEntShadow entShadow_embedded;
    xVec3 localCenter;
    xVec3 vel;
    xVec3 rotVec;
    xVec3 force;
    xVec3 instForce;
    float angVel;
    float timeToLive;
    int hitpoints;
    float lastRolling;
    unsigned int rollingID;
    unsigned char collis_chk;
    unsigned char collis_pen;
    unsigned char pad1[2];
};

void xEntBoulder_Init(void *ent, void *asset);
void xEntBoulder_Setup(xEntBoulder *ent);

#endif