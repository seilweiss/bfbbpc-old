#include "iModel.h"

#include "xMathInlines.h"
#include "iCamera.h"
#include "iSystem.h"
#include "xModel.h"

#include <rwcore.h>
#include <rpworld.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rpmatfx.h>
#include <rpusrdat.h>

RpWorld *instance_world;
RwCamera *instance_camera;

static unsigned int gLastAtomicCount;
static RpAtomic *gLastAtomicList[256];

static RpLight *sEmptyDirectionalLight[4];
static RpLight *sEmptyAmbientLight;

static RwFrame *GetChildFrameHierarchy(RwFrame *frame, void *data)
{
    RpHAnimHierarchy *hierarchy = RpHAnimFrameGetHierarchy(frame);

    if (!hierarchy)
    {
        RwFrameForAllChildren(frame, GetChildFrameHierarchy, data);
        return frame;
    }

    *(RpHAnimHierarchy **)data = hierarchy;
    return NULL;
}

static RpHAnimHierarchy *GetHierarchy(RpAtomic *atomic)
{
    RpHAnimHierarchy *hierarchy = NULL;
    RwFrame *frame = RpAtomicGetFrame(atomic);

    GetChildFrameHierarchy(frame, &hierarchy);

    return hierarchy;
}

void iModelInit()
{
    RwFrame *frame;
    RwRGBAReal black = { 0, 0, 0, 0 };
    int i;

    if (!sEmptyDirectionalLight[0])
    {
        for (i = 0; i < 4; i++)
        {
            sEmptyDirectionalLight[i] = RpLightCreate(rpLIGHTDIRECTIONAL);

            RpLightSetColor(sEmptyDirectionalLight[i], &black);

            frame = RwFrameCreate();
            RpLightSetFrame(sEmptyDirectionalLight[i], frame);
        }

        sEmptyAmbientLight = RpLightCreate(rpLIGHTAMBIENT);

        RpLightSetColor(sEmptyAmbientLight, &black);
    }
}

static RpAtomic *FindAndInstanceAtomicCallback(RpAtomic *atomic, void *data)
{
    RpHAnimHierarchy *hierarchy;
    RpGeometry *geometry;
    RpSkin *skin;
    unsigned int numBones;
    RwFrame *frame;
    RpMaterialList *matList;
    unsigned int numMaterials;
    unsigned int i;
    RpMaterial *material;
    RxPipeline *pipeline;

    hierarchy = GetHierarchy(atomic);
    geometry = RpAtomicGetGeometry(atomic);
    skin = RpSkinGeometryGetSkin(geometry);

    if (skin && !hierarchy)
    {
        numBones = RpSkinGetNumBones(skin);
        hierarchy = RpHAnimHierarchyCreate(numBones, NULL, NULL,
                                           rpHANIMHIERARCHYLOCALSPACEMATRICES, 36);
        frame = RpAtomicGetFrame(atomic);

        RpHAnimFrameSetHierarchy(frame, hierarchy);
    }

    if (hierarchy && skin)
    {
        RpSkinAtomicSetHAnimHierarchy(atomic, hierarchy);
    }

    if (hierarchy)
    {
        RpHAnimHierarchySetFlags(hierarchy, rpHANIMHIERARCHYLOCALSPACEMATRICES);
    }

    if (gLastAtomicCount < 256)
    {
        gLastAtomicList[gLastAtomicCount++] = atomic;
    }

    RwFrameGetRoot(RpAtomicGetFrame(atomic));

    matList = &geometry->matList;
    numMaterials = RpGeometryGetNumMaterials(geometry);

    for (i = 0; i < numMaterials; i++)
    {
        material = _rpMaterialListGetMaterial(matList, i);

        if (material && RpMatFXMaterialGetEffects(material) != rpMATFXEFFECTNULL)
        {
            RpMatFXAtomicEnableEffects(atomic);

#ifdef _WIN32
            pipeline = RpMatFXGetD3D9Pipeline(rpMATFXD3D9ATOMICPIPELINE);
#elif PS2
            pipeline = RpMatFXGetSkyPipeline(rpMATFXSKYATOMICPS2ALLPIPELINE);
#else
            // gamecube, xbox, etc.
#endif

            RpAtomicSetPipeline(atomic, pipeline);

            if (RpSkinGeometryGetSkin(geometry))
            {
                RpSkinAtomicSetType(atomic, rpSKINTYPEMATFX);
            }

            break;
        }
    }

    if (gLastAtomicCount < 256)
    {
        gLastAtomicList[gLastAtomicCount++] = atomic;
    }

    return atomic;
}

static RpAtomic *iModelStreamRead(RwStream *stream)
{
    static unsigned int num_models = 0;

    RpClump *clump;
    unsigned int i;
    unsigned int maxIndex;
    float maxRadius;
    float testRadius;

    if (!stream)
    {
        return NULL;
    }

    if (!RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL))
    {
        RwStreamClose(stream, NULL);
        return NULL;
    }

    clump = RpClumpStreamRead(stream);

    RwStreamClose(stream, NULL);

    if (!clump)
    {
        return NULL;
    }

    RwBBox bbox = { 1000.0f, 1000.0f, 1000.0f, -1000.0f, -1000.0f, -1000.0f };

    instance_world = RpWorldCreate(&bbox);
    instance_camera = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    RpWorldAddCamera(instance_world, instance_camera);

    gLastAtomicCount = 0;

    RpClumpForAllAtomics(clump, FindAndInstanceAtomicCallback, NULL);

    RpWorldRemoveCamera(instance_world, instance_camera);
    iCameraDestroy(instance_camera);
    RpWorldDestroy(instance_world);

    if (gLastAtomicCount > 1)
    {
        maxRadius = -1.0f;
        maxIndex = 0;

        for (i = 0; i < gLastAtomicCount; i++)
        {
            testRadius = gLastAtomicList[i]->boundingSphere.radius;

            if (testRadius > maxRadius)
            {
                maxRadius = testRadius;
                maxIndex = i;
            }
        }

        for (i = 0; i < gLastAtomicCount; i++)
        {
            if (i != maxIndex)
            {
                testRadius =
                    gLastAtomicList[i]->boundingSphere.radius +
                    xVec3Dist((xVec3 *)&gLastAtomicList[i]->boundingSphere.center,
                              (xVec3 *)&gLastAtomicList[maxIndex]->boundingSphere.center);

                if (testRadius > maxRadius)
                {
                    maxRadius = testRadius;
                }
            }
        }

        maxRadius *= 1.05f;

        for (i = 0; i < gLastAtomicCount; i++)
        {
            if (i != maxIndex)
            {
                gLastAtomicList[i]->boundingSphere.center =
                    gLastAtomicList[maxIndex]->boundingSphere.center;
            }

            gLastAtomicList[i]->boundingSphere.radius = maxRadius;
            gLastAtomicList[i]->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;
        }
    }

    return gLastAtomicList[0];
}

RpAtomic *iModelFileNew(void *buffer, unsigned int size)
{
    RwMemory rwmem;
    rwmem.start = (RwUInt8 *)buffer;
    rwmem.length = size;

    return iModelStreamRead(RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &rwmem));
}

void iModelUnload(RpAtomic *userdata)
{
    RpClump *clump;
    RwFrame *frame;
    RwFrame *root;

    clump = RpAtomicGetClump(userdata);
    frame = RpClumpGetFrame(clump);

    if (frame)
    {
        root = RwFrameGetRoot(frame);

        if (root)
        {
            frame = root;
        }

        RwFrameDestroyHierarchy(frame);
        RpClumpSetFrame(clump, NULL);
    }

    if (clump)
    {
        RpClumpDestroy(clump);
    }
}

static RpAtomic *NextAtomicCallback(RpAtomic *atomic, void *data)
{
    RpAtomic **nextModel = (RpAtomic **)data;

    if (*nextModel == atomic)
    {
        *nextModel = NULL;
    }
    else if (*nextModel == NULL)
    {
        *nextModel = atomic;
    }

    return atomic;
}

RpAtomic *iModelFile_RWMultiAtomic(RpAtomic *model)
{
    if (!model)
    {
        return NULL;
    }

    RpClump *clump = RpAtomicGetClump(model);
    RpAtomic *nextModel = model;

    RpClumpForAllAtomics(clump, NextAtomicCallback, &nextModel);

    return nextModel;
}

unsigned int iModelNumBones(RpAtomic *model)
{
    RpHAnimHierarchy *hierarchy = GetHierarchy(model);

    if (hierarchy)
    {
        return hierarchy->numNodes;
    }

    return 0;
}

int iModelCull(RpAtomic *model, RwMatrix *mat)
{
    RwCamera *cam = RwCameraGetCurrentCamera();
    RwSphere sph;

    RwV3dTransformPoints(&sph.center, &model->boundingSphere.center, 1, mat);

    RwReal f1 = RwV3dDotProduct(&mat->right, &mat->right);
    RwReal f3 = RwV3dDotProduct(&mat->up, &mat->up);
    RwReal f4 = RwV3dDotProduct(&mat->at, &mat->at);

    sph.radius = model->boundingSphere.radius * xsqrt(xmax(f1, xmax(f3, f4)));

    model->worldBoundingSphere = sph;

    if (!cam)
    {
        return 1;
    }

    return (RwCameraFrustumTestSphere(cam, &sph) == rwSPHEREOUTSIDE);
}

static unsigned int iModelTagUserData(xModelTag *tag, RpAtomic *model, float x, float y,
                                      float z, int closeV)
{
    int i;
    int count;
    RpUserDataArray *array;
    RpUserDataArray *testarray;
    float distSqr;
    float closeDistSqr;
    int numTags;
    int t;
    xModelTag *tagList;

    count = RpGeometryGetUserDataArrayCount(RpAtomicGetGeometry(model));
    array = NULL;

    for (i = 0; i < count; i++)
    {
        testarray = RpGeometryGetUserDataArray(RpAtomicGetGeometry(model), i);

        if (strcmp(testarray->name, "HI_Tags") == 0)
        {
            array = testarray;
            break;
        }
    }

    if (!array)
    {
        memset(tag, 0, sizeof(xModelTag));
        return 0;
    }

    distSqr = 1000000000.0f;

    numTags = *(int *)array->data;
    tagList = (xModelTag *)((int *)array->data + 1);

    if (closeV < 0 || closeV > numTags)
    {
        closeV = 0;

        for (t = 0; t < numTags; t++)
        {
            closeDistSqr =
                xsqr(tagList[t].v.x - x) +
                xsqr(tagList[t].v.y - y) +
                xsqr(tagList[t].v.z - z);

            if (closeDistSqr < distSqr)
            {
                closeV = t;
                distSqr = closeDistSqr;
            }
        }

        if (tag)
        {
            *tag = tagList[closeV];
        }
    }
    else
    {
        if (tag)
        {
            *tag = tagList[closeV];
        }
    }

    return closeV;
}

static unsigned int iModelTagInternal(xModelTag *tag, RpAtomic *model, float x, float y,
                                      float z, int closeV)
{
    RpGeometry *geom;
    RwV3d *vert;
    int v;
    int numV;
    float distSqr;
    float closeDistSqr;
    RpSkin *skin;
    const RwMatrixWeights *wt;

    geom = RpAtomicGetGeometry(model);
    vert = RpMorphTargetGetVertices(RpGeometryGetMorphTarget(geom, 0));

    if (!vert)
    {
        return iModelTagUserData(tag, model, x, y, z, closeV);
    }

    numV = RpGeometryGetNumVertices(geom);
    distSqr = 1000000000.0f;

    if (closeV < 0 || closeV > numV)
    {
        closeV = 0;

        for (v = 0; v < numV; v++)
        {
            closeDistSqr =
                xsqr(vert[v].x - x) +
                xsqr(vert[v].y - y) +
                xsqr(vert[v].z - z);

            if (closeDistSqr < distSqr)
            {
                closeV = v;
                distSqr = closeDistSqr;
            }
        }

        if (tag)
        {
            tag->v.x = x;
            tag->v.y = y;
            tag->v.z = z;
        }
    }
    else
    {
        if (tag)
        {
            tag->v.x = vert[closeV].x;
            tag->v.y = vert[closeV].y;
            tag->v.z = vert[closeV].z;
        }
    }

    if (tag)
    {
        skin = RpSkinGeometryGetSkin(RpAtomicGetGeometry(model));

        if (skin)
        {
            wt = RpSkinGetVertexBoneWeights(skin);

            tag->matidx = RpSkinGetVertexBoneIndices(skin)[closeV];

            tag->wt[0] = wt[closeV].w0;
            tag->wt[1] = wt[closeV].w1;
            tag->wt[2] = wt[closeV].w2;
            tag->wt[3] = wt[closeV].w3;
        }
        else
        {
            tag->matidx = 0;

            tag->wt[0] = 0.0f;
            tag->wt[1] = 0.0f;
            tag->wt[2] = 0.0f;
            tag->wt[3] = 0.0f;
        }
    }

    return closeV;
}

unsigned int iModelTagSetup(xModelTag *tag, RpAtomic *model, float x, float y, float z)
{
    return iModelTagInternal(tag, model, x, y, z, -1);
}