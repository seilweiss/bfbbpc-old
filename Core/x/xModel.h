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

#define XMODEL_UNK1 0x1
#define XMODEL_UNK2 0x2
#define XMODEL_UNK8 0x8
#define XMODEL_UNK2000 0x2000

#define XMODEL_EXT_UNK40(flags) ((flags) >> 6 & 0x1)

#define XMODEL_PIPE_UNK100000 0x100000
#define XMODEL_PIPE_UNK800000 0x800000
#define XMODEL_PIPE_UNKF00000 0xF00000

#define XMODEL_PIPE_UNK80000 0x80000

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
    RwMatrix *Mat;
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

struct xModelAssetInfo
{
    unsigned int Magic;
    unsigned int NumModelInst;
    unsigned int AnimTableID;
    unsigned int CombatID;
    unsigned int BrainID;
};

struct xModelAssetInst
{
    unsigned int ModelID;
    unsigned short Flags;
    unsigned char Parent;
    unsigned char Bone;
    float MatRight[3];
    float MatUp[3];
    float MatAt[3];
    float MatPos[3];
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

unsigned int xModelGetPipeFlags(RpAtomic *model);
void xModelInit();
void xModelPoolInit(unsigned int count, unsigned int numMatrices);
xModelInstance *xModelInstanceAlloc(RpAtomic *data, void *object, unsigned short flags,
                                    unsigned char boneIndex, unsigned char *boneRemap);
void xModelInstanceAttach(xModelInstance *inst, xModelInstance *parent);
void xModelEval(xModelInstance *modelInst);
void xModel_SceneExit(RpWorld *world);

inline xMat4x3 *xModelGetFrame(xModelInstance *modelInst)
{
    return (xMat4x3 *)modelInst->Mat;
}

#endif