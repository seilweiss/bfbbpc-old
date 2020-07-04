#include "iEnv.h"

#include "iCamera.h"
#include "iSystem.h"
#include "xMemMgr.h"
#include "iModel.h"
#include "xJSP.h"

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

void iEnvLightingBasics(iEnv *env, xEnvAsset *asset)
{
    return;
}

static void Jsp_ClumpRender(RpClump *clump, xJSPNodeInfo *nodeInfo)
{
    int backcullon = 1;
    int zbufferon = 1;

    RwLLLink *cur = rwLinkListGetFirstLLLink(&clump->atomicList);
    RwLLLink *end = rwLinkListGetTerminator(&clump->atomicList);

    while (cur != end)
    {
        RpAtomic *apAtom = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

        if (rwObjectTestFlags(apAtom, rpATOMICRENDER))
        {
            RwFrame *frame = (RwFrame *)rwObjectGetParent(apAtom);

            if (!iModelCull(apAtom, &frame->ltm))
            {
                if (backcullon)
                {
                    if (nodeInfo->nodeFlags & XJSPNODE_TOGGLEBACKCULL)
                    {
                        backcullon = 0;
                        RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
                    }
                }
                else
                {
                    if (nodeInfo->nodeFlags & XJSPNODE_TOGGLEBACKCULL)
                    {
                        backcullon = 1;
                        RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLBACK);
                    }
                }

                if (zbufferon)
                {
                    if (nodeInfo->nodeFlags & XJSPNODE_TOGGLEZBUFFER)
                    {
                        zbufferon = 0;
                        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);
                    }
                }
                else
                {
                    if (nodeInfo->nodeFlags & XJSPNODE_TOGGLEZBUFFER)
                    {
                        zbufferon = 1;
                        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
                    }
                }

                RpAtomicRender(apAtom);
            }
        }

        cur = rwLLLinkGetNext(cur);
        nodeInfo++;
    }
}

static void JspPS2_ClumpRender(RpClump *clump, xJSPNodeInfo *nodeInfo)
{
    int backcullon;
    int zbufferon;
    RwLLLink *cur;
    RwLLLink *end;
    RpAtomic *apAtom;
    RwFrame *frame;
    int insideFlag;
}

static iEnv *lastEnv;

void iEnvRender(iEnv *env)
{
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    if (env->jsp)
    {
#ifdef PS2
        JspPS2_ClumpRender(env->jsp->clump, env->jsp->jspNodeList);
#else
        Jsp_ClumpRender(env->jsp->clump, env->jsp->jspNodeList);
#endif
    }
    else
    {
        RpWorldRender(env->world);
    }

    lastEnv = env;
}