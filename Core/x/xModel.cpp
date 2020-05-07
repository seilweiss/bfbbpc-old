#include "xModel.h"

#include "iModel.h"
#include "xMemMgr.h"

static xModelPool *sxModelPoolList;
static RwCamera *subcamera;

int xModelPipeNumTables;

static RwCamera *CameraCreate(int width, int height, int zbuffer);
static void CameraDestroy(RwCamera *camera);

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
        gActiveHeap,
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