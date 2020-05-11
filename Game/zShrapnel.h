#ifndef ZSHRAPNEL_H
#define ZSHRAPNEL_H

#include "xModel.h"
#include "zParEmitter.h"
#include "xCurveAsset.h"
#include "xCollide.h"
#include "zLightning.h"
#include "zScene.h"

#include <rwcore.h>

struct zShrapnelAsset;

enum zFragType
{
    eFragInactive,
    eFragGroup,
    eFragShrapnel,
    eFragParticle,
    eFragProjectile,
    eFragLightning,
    eFragSound,
    eFragShockwave,
    eFragCount,
    eFragForceSize = RWFORCEENUMSIZEINT
};

struct zFragAsset
{
    zFragType type;
    unsigned int id;
    unsigned int parentID[2];
    float lifetime;
    float delay;
};

struct zFrag;

struct zFragGroup
{
    zFrag *list[21];
};

enum zFragLocType
{
    eFragLocBone,
    eFragLocBoneUpdated,
    eFragLocBoneLocal,
    eFragLocBoneLocalUpdated,
    eFragLocTag,
    eFragLocTagUpdated,
    eFragLocCount,
    eFragLocForceSize = RWFORCEENUMSIZEINT
};

struct zFragBone
{
    int index;
    xVec3 offset;
};

union zFragLocInfo
{
    zFragBone bone;
    xModelTag tag;
};

struct zFragLocation
{
    zFragLocType type;
    zFragLocInfo info;
};

struct zFragParticleAsset : zFragAsset
{
    zFragLocation source;
    zFragLocation vel;
    xParEmitterCustomSettings emit;
    unsigned int parEmitterID;
    zParEmitter *parEmitter;
};

struct zFragParticle
{
    zFragParticleAsset *fasset;
};

struct zFragProjectileAsset : zFragAsset
{
    unsigned int modelInfoID;
    RpAtomic *modelFile;
    zFragLocation launch;
    zFragLocation vel;
    float bounce;
    int maxBounces;
    unsigned int flags;
    unsigned int childID;
    zShrapnelAsset *child;
    float minScale;
    float maxScale;
    unsigned int scaleCurveID;
    xCurveAsset *scaleCurve;
    float gravity;
};

struct zFragProjectile
{
    zFragProjectileAsset *fasset;
    xModelInstance *model;
    xParabola path;
    float angVel;
    float t;
    float tColl;
    int numBounces;
    float scale;
    float parentScale;
    float alpha;
    xVec3 N;
    xVec3 axis;
};

struct zFragLightningAsset : zFragAsset
{
    zFragLocation start;
    zFragLocation end;
    unsigned int startParentID;
    unsigned int endParentID;
};

struct zFragLightning
{
    zFragLightningAsset *fasset;
    xModelInstance *startParent;
    xModelInstance *endParent;
    zLightning *lightning;
};

struct zFragSoundAsset : zFragAsset
{
    unsigned int assetID;
    zFragLocation source;
    float volume;
    float innerRadius;
    float outerRadius;
};

struct zFragSound
{
    zFragSoundAsset *fasset;
    xVec3 location;
    unsigned int soundID;
};

struct zFragShockwaveAsset : zFragAsset
{
    unsigned int modelInfoID;
    float birthRadius;
    float deathRadius;
    float birthVelocity;
    float deathVelocity;
    float birthSpin;
    float deathSpin;
    float birthColor[4];
    float deathColor[4];
};

struct zFragShockwave
{
    zFragShockwaveAsset *fasset;
    float currSize;
    float currVelocity;
    float deltVelocity;
    float currSpin;
    float deltSpin;
    float currColor[4];
    float deltColor[4];
};

union zFragInfo
{
    zFragGroup group;
    zFragParticle particle;
    zFragProjectile projectile;
    zFragLightning lightning;
    zFragSound sound;
    zFragShockwave shockwave;
};

struct zFrag
{
    zFragType type;
    zFragInfo info;
    float delay;
    float alivetime;
    float lifetime;
    void(*update)(zFrag *, float);
    xModelInstance *parent[2];
    zFrag *prev;
    zFrag *next;
};

typedef void(*type_39)(zFrag *, zFragAsset *);
typedef void(*zShrapnelInitCallBack)(zShrapnelAsset *, xModelInstance *, xVec3 *, type_39);

struct zShrapnelAsset
{
    int fassetCount;
    unsigned int shrapnelID;
    zShrapnelInitCallBack initCB;
};

void zShrapnel_GameInit();
void zShrapnel_SceneInit(zScene *sc);

#endif