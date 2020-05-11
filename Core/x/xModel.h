#ifndef XMODEL_H
#define XMODEL_H

#include "xMath3.h"
#include "xModelBucket.h"
#include "xLightKit.h"

#include <rwcore.h>
#include <rpworld.h>

struct xModelTag
{
    xVec3 v;
    unsigned int matidx;
    float wt[4];
};

struct xAnimPlay;
struct xModelPool;
struct xSurface;

struct xModelInstance
{
    xModelInstance *Next;
    xModelInstance *Parent;
    xModelPool *Pool;
    xAnimPlay *Anim;
    RpAtomic *Data;
    unsigned int PipeFlags;
    float RedMultiplier;
    float GreenMultiplier;
    float BlueMultiplier;
    float Alpha;
    float FadeStart;
    float FadeEnd;
    xSurface *Surf;
    xModelBucket **Bucket;
    xModelInstance *BucketNext;
    xLightKit *LightKit;
    void *Object;
    unsigned short Flags;
    unsigned char BoneCount;
    unsigned char BoneIndex;
    unsigned char *BoneRemap;
    RwMatrixTag *Mat;
    xVec3 Scale;
    unsigned int modelID;
    unsigned int shadowID;
    RpAtomic *shadowmapAtomic;
    struct
    {
        xVec3 *verts;
    } anim_coll;
};

struct xModelPool
{
    xModelPool *Next;
    unsigned int NumMatrices;
    xModelInstance *List;
};

struct xModelAssetParam
{
    unsigned int HashID;
    unsigned char WordLength;
    unsigned char String[3];
};

struct xModelPipeInfo
{
    unsigned int ModelHashID;
    unsigned int SubObjectBits;
    unsigned int PipeFlags;
};

struct xModelPipeLookup
{
    RpAtomic *model;
    unsigned int PipeFlags;
};

extern int xModelPipeNumTables;
extern int xModelPipeCount[16];
extern xModelPipeInfo *xModelPipeData[16];
extern int xModelLookupCount;
extern xModelPipeLookup *xModelLookupList;
extern int xModelInstStaticAlloc;

void xModelInit();
void xModelPoolInit(unsigned int count, unsigned int numMatrices);

#endif