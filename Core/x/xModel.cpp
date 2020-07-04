#include "xModel.h"

#include "iModel.h"
#include "xMemMgr.h"

#include "print.h"

static xModelPool *sxModelPoolList;
static RwCamera *subcamera;

int xModelPipeNumTables;
int xModelPipeCount[16];
xModelPipeInfo *xModelPipeData[16];
int xModelLookupCount;
xModelPipeLookup *xModelLookupList;
int xModelInstStaticAlloc;

static RwCamera *CameraCreate(int width, int height, int zbuffer);
static void CameraDestroy(RwCamera *camera);

unsigned int xModelGetPipeFlags(RpAtomic *model)
{
    int i;

    for (i = 0; i < xModelLookupCount; i++)
    {
        if (xModelLookupList[i].model == model)
        {
            return xModelLookupList[i].PipeFlags;
        }
    }

    return 0;
}

void xModelInit()
{
    iModelInit();

    sxModelPoolList = NULL;

    if (!subcamera)
    {
        subcamera = CameraCreate(0, 0, 1);
    }

    xModelPipeNumTables = 0;
}

void xModelPoolInit(unsigned int count, unsigned int numMatrices)
{
    int i;
    unsigned char *buffer;
    RwMatrix *mat;
    xModelPool *pool;
    xModelPool *curr;
    xModelPool **prev;
    xModelInstance *inst;

    if (numMatrices < 1)
    {
        numMatrices = 1;
    }

    buffer = (unsigned char *)xMemAlloc(
        count * numMatrices * sizeof(RwMatrix) +
            count * sizeof(xModelInstance) +
            sizeof(xModelPool),
        16);

    mat = (RwMatrix *)buffer;
    pool = (xModelPool *)(mat + count * numMatrices);
    inst = (xModelInstance *)(pool + 1);

    for (i = 0; i < count; i++)
    {
        inst[i].Next = &inst[i + 1];
        inst[i].Pool = pool;
        inst[i].Mat = &mat[i];
    }

    inst[count - 1].Next = NULL;

    pool->NumMatrices = numMatrices;
    pool->List = inst;

    prev = &sxModelPoolList;
    curr = sxModelPoolList;

    while (curr && numMatrices < curr->NumMatrices)
    {
        prev = &curr->Next;
        curr = curr->Next;
    }

    pool->Next = curr;
    *prev = pool;
}

xModelInstance *xModelInstanceAlloc(RpAtomic *data, void *object, unsigned short flags,
                                    unsigned char boneIndex, unsigned char *boneRemap)
{
    int i;
    unsigned int boneCount;
    unsigned int matCount;
    xModelPool *curr;
    xModelPool *found = NULL;
    xModelInstance *dude;
    RwMatrix *allocmats;

    boneCount = iModelNumBones(data);
    matCount = boneCount + XMODEL_EXT_UNK40(flags) + 1;

    if (xModelInstStaticAlloc)
    {
        if (flags & XMODEL_UNK2000)
        {
            dude = (xModelInstance *)xMemAlloc(sizeof(xModelInstance), 0);

            dude->Pool = NULL;
            dude->Mat = NULL;
        }
        else
        {
            allocmats = (RwMatrix *)xMemAlloc(
                matCount * sizeof(RwMatrix) + sizeof(xModelInstance), 16);

            dude = (xModelInstance *)(allocmats + matCount);

            dude->Pool = NULL;
            dude->Mat = allocmats;
        }
    }
    else
    {
        if (flags & XMODEL_UNK2000)
        {
            flags = (flags & ~XMODEL_UNK2000 | XMODEL_UNK8);
        }

        curr = sxModelPoolList;

        while (curr)
        {
            if (curr->List && matCount <= curr->NumMatrices)
            {
                found = curr;
            }

            curr = curr->Next;
        }

        if (!found)
        {
            return NULL;
        }

        dude = found->List;
        found->List = dude->Next;
    }

    dude->Next = NULL;
    dude->Parent = NULL;
    dude->Anim = NULL;
    dude->Data = data;
    dude->Object = object;
    dude->Flags = (flags | XMODEL_UNK1 | XMODEL_UNK2);
    dude->BoneCount = boneCount;
    dude->BoneIndex = boneIndex;
    dude->BoneRemap = boneRemap;
    dude->modelID = 0;
    dude->shadowID = 0xDEADBEEF;
    dude->Scale.x = 0.0f;
    dude->Scale.y = 0.0f;
    dude->Scale.z = 0.0f;
    dude->RedMultiplier = 1.0f;
    dude->GreenMultiplier = 1.0f;
    dude->BlueMultiplier = 1.0f;
    dude->Alpha = (data->geometry->matList.materials[0]->color.alpha) / 255.0f;
    dude->Surf = NULL;
    dude->FadeStart = 9e+37f;
    dude->FadeEnd = 1e+38f;

    if (dude->Mat)
    {
        for (i = 0; i < matCount; i++)
        {
            xMat4x3Identity((xMat4x3 *)&dude->Mat[i]);
        }
    }

    dude->Bucket = xModelBucket_GetBuckets(dude->Data);
    dude->LightKit = NULL;

    if (dude->Bucket)
    {
        dude->PipeFlags = dude->Bucket[0]->PipeFlags;
    }
    else
    {
        dude->PipeFlags = xModelGetPipeFlags(dude->Data);
    }

    if (!(dude->PipeFlags & (XMODEL_PIPE_UNK80000 | XMODEL_PIPE_UNKF00000)))
    {
        dude->PipeFlags |= (XMODEL_PIPE_UNK80000 |
                            XMODEL_PIPE_UNK100000 |
                            XMODEL_PIPE_UNK800000);
    }

    dude->anim_coll.verts = NULL;

    return dude;
}

void xModelInstanceAttach(xModelInstance *inst, xModelInstance *parent)
{
    xModelInstance *curr;

    curr = parent;

    while (curr->Next)
    {
        curr = curr->Next;
    }

    curr->Next = inst;

    inst->Parent = parent;

    if (inst->Flags & XMODEL_UNK2000)
    {
        inst->Mat = parent->Mat;
    }
}

void xModelEval(xModelInstance *modelInst)
{
    BFBBSTUB("xModelEval");
}

static RwCamera *CameraCreate(int width, int height, int zbuffer)
{
    RwCamera *camera;

    camera = RwCameraCreate();

    if (camera)
    {
        RwCameraSetFrame(camera, RwFrameCreate());
        RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));

        if (zbuffer)
        {
            RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));
        }

        if (RwCameraGetFrame(camera) &&
            RwCameraGetRaster(camera) &&
            (!zbuffer || RwCameraGetZRaster(camera)))
        {
            return camera;
        }
    }

    CameraDestroy(camera);
    return NULL;
}

static void CameraDestroy(RwCamera *camera)
{
    RwRaster *raster;
    RwFrame *frame;

    if (camera)
    {
        _rwFrameSyncDirty();

        frame = RwCameraGetFrame(camera);

        if (frame)
        {
            RwCameraSetFrame(camera, NULL);
            RwFrameDestroy(frame);
        }

        raster = RwCameraGetRaster(camera);

        if (raster)
        {
            RwRasterDestroy(raster);
            RwCameraSetRaster(camera, NULL);
        }

        raster = RwCameraGetZRaster(camera);

        if (raster)
        {
            RwRasterDestroy(raster);
            RwCameraSetRaster(camera, NULL);
        }

        RwCameraDestroy(camera);
    }
}

void xModel_SceneExit(RpWorld *world)
{
    RpWorldRemoveCamera(world, subcamera);
}