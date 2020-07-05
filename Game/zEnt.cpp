#include "zEnt.h"

#include "zScene.h"
#include "zAnimList.h"
#include "zMain.h"
#include "xstransvc.h"
#include "iModel.h"

#include "print.h"

void zEntInit(zEnt *ent, xEntAsset *asset, unsigned int type)
{
    xEntInit(ent, asset);

    ent->update = (xEntUpdateCallBack)zEntUpdate;

    if (type == 'PLYR')
    {
        ent->collType = XENT_COLLTYPE_PLYR;
        ent->collLev = XENT_COLLLEV_UNK4;
        ent->bound.type = XBOUND_TYPE_UNK1;

        zEntParseModelInfo(ent, asset->modelInfoID);
    }
    else if (type == 'VIL ')
    {
        ent->collType = XENT_COLLTYPE_NPC;
        ent->collLev = XENT_COLLLEV_UNK4;
        ent->bound.type = XBOUND_TYPE_UNK1;
        ent->moreFlags |= XENT_HITTABLE;

        zEntParseModelInfo(ent, asset->modelInfoID);
    }
    else if (type == 'ITEM')
    {
        ent->collType = XENT_COLLTYPE_STAT;
        ent->collLev = XENT_COLLLEV_UNK4;
        ent->bound.type = XBOUND_TYPE_UNK1;
        ent->eventFunc = NULL;

        zEntParseModelInfo(ent, asset->modelInfoID);
    }
    else if (type == 'PKUP')
    {
        ent->collType = XENT_COLLTYPE_NA;
        ent->bound.type = XBOUND_TYPE_UNK0;
    }
    else if (type == 'PLAT')
    {
        ent->collType = XENT_COLLTYPE_DYN;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.type = XBOUND_TYPE_UNK4;
        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'PEND')
    {
        ent->collType = XENT_COLLTYPE_DYN;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.type = XBOUND_TYPE_UNK4;
        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'TRIG')
    {
        ent->collType = XENT_COLLTYPE_TRIG;
        ent->bound.type = XBOUND_TYPE_UNK0;
    }
    else if (type == 'HANG')
    {
        ent->collType = XENT_COLLTYPE_DYN;
        ent->collLev = XENT_COLLLEV_UNK4;
        ent->bound.type = XBOUND_TYPE_UNK1;

        zEntParseModelInfo(ent, asset->modelInfoID);
    }
    else if (type == 'SIMP')
    {
        ent->collType = XENT_COLLTYPE_STAT;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        ent->bound.type = XBOUND_TYPE_UNK4;

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'UI  ')
    {
        // Nothing
    }
    else if (type == 'BUTN')
    {
        ent->collType = XENT_COLLTYPE_DYN;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        ent->bound.type = XBOUND_TYPE_UNK4;
        ent->moreFlags |= XENT_HITTABLE;

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'DSTR')
    {
        ent->collType = XENT_COLLTYPE_DYN;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        ent->bound.type = XBOUND_TYPE_UNK4;
        ent->moreFlags |= XENT_HITTABLE;

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'EGEN')
    {
        ent->collType = XENT_COLLTYPE_DYN;

        if (asset->moreFlags & XENT_MESHCOLL)
        {
            ent->collLev = XENT_COLLLEV_UNK5;
        }
        else
        {
            ent->collLev = XENT_COLLLEV_UNK4;
        }

        ent->bound.type = XBOUND_TYPE_UNK4;

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }
    else if (type == 'TBOX')
    {
        ent->collType = XENT_COLLTYPE_STAT;
        ent->collLev = XENT_COLLLEV_UNK5;
        ent->bound.type = XBOUND_TYPE_UNK4;

        zEntParseModelInfo(ent, asset->modelInfoID);

        ent->bound.mat = (xMat4x3 *)ent->model->Mat;
    }

    if (asset->animListID)
    {
        int num_used = zAnimListGetNumUsed(asset->animListID);

        if (num_used > 0)
        {
            ent->atbl = zAnimListGetTable(asset->animListID);

            xAnimPoolAlloc(&globals.sceneCur->mempool, ent, ent->atbl, ent->model);

            xAnimState *ast = xAnimTableGetState(ent->atbl, "idle");

            if (ast)
            {
                xAnimSingle *single = ent->model->Anim->Single;

                single->State = ast;
                single->Time = 0.0f;
                single->CurrentSpeed = 1.0f;

                xModelEval(ent->model);
            }
        }
        else
        {
            ent->atbl = NULL;
        }
    }
    else
    {
        ent->atbl = NULL;
    }

    xEntInitForType(ent);
}

void zEntUpdate(zEnt *ent, zScene *scene, float elapsedSec)
{
    xEntUpdate(ent, scene, elapsedSec);
}

void zEntEventAll(xBase *from, unsigned int fromEvent, unsigned int toEvent,
                  float *toParam)
{
    BFBBSTUB("zEntEventAll");
}

void zEntEventAllOfType(unsigned int toEvent, unsigned int type)
{
    BFBBSTUB("zEntEventAllOfType");
}

xModelInstance *zEntRecurseModelInfo(void *info, xEnt *ent)
{
    unsigned int i;
    unsigned int bufsize;
    RpAtomic *imodel;
    xModelInstance *tempInst[64];
    xModelAssetInfo *zinfo;
    xModelAssetInst *zinst;
    xAnimTable *table;

    zinfo = (xModelAssetInfo *)info;
    zinst = (xModelAssetInst *)(zinfo + 1);

    for (i = 0; i < zinfo->NumModelInst; i++)
    {
        info = xSTFindAsset(zinst[i].ModelID, &bufsize);

        if (*(unsigned int *)info == 'FNIM')
        {
            tempInst[i] = zEntRecurseModelInfo(info, ent);

            if (i != 0)
            {
                tempInst[i]->Flags |= zinst[i].Flags;
                tempInst[i]->BoneIndex = zinst[i].Bone;

                xModelInstanceAttach(tempInst[i], tempInst[zinst[i].Parent]);
            }
        }
        else
        {
            imodel = (RpAtomic *)info;

            if (i == 0)
            {
                tempInst[i] = xModelInstanceAlloc(imodel, ent, 0, 0, NULL);

                tempInst[i]->modelID = zinst[i].ModelID;

                while (imodel = iModelFile_RWMultiAtomic(imodel))
                {
                    xModelInstanceAttach(
                        xModelInstanceAlloc(imodel, ent, XMODEL_UNK2000, 0, NULL),
                        tempInst[i]);
                }
            }
            else
            {
                tempInst[i] = xModelInstanceAlloc(imodel, ent,
                                                  zinst[i].Flags, zinst[i].Bone, NULL);

                xModelInstanceAttach(tempInst[i], tempInst[zinst[i].Parent]);

                while (imodel = iModelFile_RWMultiAtomic(imodel))
                {
                    xModelInstanceAttach(
                        xModelInstanceAlloc(imodel, ent, XMODEL_UNK2000, 0, NULL),
                        tempInst[i]);
                }
            }
        }
    }

    if (zinfo->AnimTableID)
    {
        table = (xAnimTable *)xSTFindAsset(zinfo->AnimTableID, &bufsize);

        tempInst[0]->Anim = xAnimPoolAlloc(&globals.sceneCur->mempool, ent, table,
                                           tempInst[0]);
    }

    return tempInst[0];
}

void zEntParseModelInfo(xEnt *ent, unsigned int assetID)
{
    unsigned int bufsize;

    void *info = xSTFindAsset(assetID, &bufsize);

    if (*(unsigned int *)info == 'FNIM')
    {
        ent->model = zEntRecurseModelInfo(info, ent);
    }
    else
    {
        xEntLoadModel(ent, (RpAtomic *)info);

        ent->model->modelID = assetID;
    }
}

xModelAssetParam *zEntGetModelParams(unsigned int assetID, unsigned int *size)
{
    unsigned int bufsize;
    void *info;
    xModelAssetInfo *minf;

    info = xSTFindAsset(assetID, &bufsize);

    if (*(unsigned int *)info == 'FNIM')
    {
        minf = (xModelAssetInfo *)info;

        *size = bufsize -
            (sizeof(xModelAssetInfo) + minf->NumModelInst * sizeof(xModelAssetInst));

        if (*size)
        {
            return (xModelAssetParam *)
                ((xModelAssetInst *)(minf + 1) + minf->NumModelInst);
        }
    }
    else
    {
        *size = 0;
    }

    return NULL;
}