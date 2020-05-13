#include "iEnv.h"

#include "iCamera.h"
#include "iSystem.h"
#include "xMemMgr.h"

static RpWorld *sPipeWorld;
static RwCamera *sPipeCamera;

static RpAtomic *SetPipelineCB(RpAtomic *atomic, void *data)
{
    if (RwCameraBeginUpdate(sPipeCamera))
    {
        RpAtomicInstance(atomic);
        RwCameraEndUpdate(sPipeCamera);
    }

    if (data)
    {
        RpAtomicSetPipeline(atomic, (RxPipeline *)data);
    }

    return atomic;
}

static void iEnvSetBSP(iEnv *env, int envDataType, RpWorld *bsp)
{
    if (envDataType == 0)
    {
        env->world = bsp;
    }
    else if (envDataType == 1)
    {
        env->collision = bsp;
    }
    else if (envDataType == 2)
    {
        env->fx = bsp;
    }
    else if (envDataType == 3)
    {
        env->camera = bsp;
    }
}

void iEnvStartup()
{
    return;
}

void iEnvLoad(iEnv *env, const void *data, unsigned int datasize, int dataType)
{
    RpWorld *bsp;
    xJSPHeader *jsp;
    RwBBox tmpbbox;

    jsp = (xJSPHeader *)data;

    if (jsp->idtag[0] == 'J' &&
        jsp->idtag[1] == 'S' &&
        jsp->idtag[2] == 'P' &&
        jsp->idtag[3] == '\0')
    {
        if (dataType == 0)
        {
            tmpbbox = { 1000.0f, 1000.0f, 1000.0f, -1000.0f, -1000.0f, -1000.0f };

            env->world = RpWorldCreate(&tmpbbox);

            sPipeCamera = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
            sPipeWorld = env->world;

            RpWorldAddCamera(sPipeWorld, sPipeCamera);

            env->jsp = jsp;

            RpClumpForAllAtomics(env->jsp->clump, SetPipelineCB, NULL);

            xClumpColl_InstancePointers(env->jsp->colltree, env->jsp->clump);

            RpWorldRemoveCamera(sPipeWorld, sPipeCamera);
            iCameraDestroy(sPipeCamera);

            sPipeWorld = NULL;
            sPipeCamera = NULL;
        }
    }
    else
    {
        if (dataType == 0)
        {
            env->jsp = NULL;
        }

        bsp = (RpWorld *)data;
        iEnvSetBSP(env, dataType, bsp);
    }

    if (dataType == 0)
    {
        env->memlvl = xMemGetBase();
    }
}

void iEnvDefaultLighting(iEnv *env)
{
    return;
}