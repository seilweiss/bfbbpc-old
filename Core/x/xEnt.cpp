#include "xEnt.h"

#include "xMemMgr.h"
#include "iModel.h"

struct anim_coll_data
{
    unsigned int flags;
    unsigned int bones;
    xMat4x3 old_mat;
    xMat4x3 new_mat;
    unsigned int verts_size;
    xVec3 *verts;
    xVec3 *normals;
};

static float sEntityTimePassed;

static xBox all_ents_box;
static int all_ents_box_init;

void xEntSetTimePassed(float sec)
{
    sEntityTimePassed = sec;
}

void xEntSceneInit()
{
    all_ents_box_init = 1;
}

void xEntSceneExit()
{
    return;
}

void xEntInit(xEnt *ent, xEntAsset *asset)
{
    xBaseInit(ent, asset);

    ent->asset = asset;
    ent->update = xEntUpdate;
    ent->bupdate = xEntDefaultBoundUpdate;
    ent->render = xEntRender;
    ent->move = NULL;
    ent->transl = xEntDefaultTranslate;
    ent->flags = asset->flags;
    ent->miscflags = 0;
    ent->moreFlags = asset->moreFlags;
    ent->subType = asset->subtype;
    ent->pflags = asset->pflags;
    ent->ffx = NULL;
    ent->num_ffx = 0;
    ent->driver = NULL;
    ent->model = NULL;
    ent->collModel = NULL;
    ent->camcollModel = NULL;
    ent->frame = NULL;
    ent->collis = NULL;
    ent->lightKit = NULL;
    ent->simpShadow = NULL;
    ent->entShadow = NULL;
    ent->baseFlags |= XBASE_ISENT;

    xGridBoundInit(&ent->gridb, ent);

    ent->anim_coll = NULL;

    if (all_ents_box_init)
    {
        iBoxInitBoundVec(&all_ents_box, &asset->pos);
        all_ents_box_init = 0;
    }
    else
    {
        iBoxBoundVec(&all_ents_box, &all_ents_box, &asset->pos);
    }
}

void xEntInitForType(xEnt *ent)
{
    ent->update = xEntUpdate;
    ent->render = xEntRender;

    if (ent->collType == XENT_COLLTYPE_TRIG)
    {
        ent->pflags &= ~(XENT_PUNK1 | XENT_PUNK2);
        ent->chkby = 0x18;
        ent->penby = 0;
    }
    else if (ent->collType == XENT_COLLTYPE_STAT)
    {
        ent->pflags &= ~(XENT_PUNK1 | XENT_PUNK2);
        ent->chkby = 0x18;
        ent->penby = 0x18;
    }
    else if (ent->collType == XENT_COLLTYPE_DYN)
    {
        ent->pflags |= XENT_PUNK1;
        ent->move = NULL;
        ent->frame = (xEntFrame *)xMemAlloc(sizeof(xEntFrame), 0);

        memset(ent->frame, 0, sizeof(xEntFrame));

        ent->pflags &= ~XENT_PUNK2;
        ent->chkby = 0x18;
        ent->penby = 0x18;
    }
    else if (ent->collType == XENT_COLLTYPE_NPC)
    {
        ent->pflags |= XENT_PUNK1;
        ent->move = NULL;
        ent->pflags |= XENT_PUNK2;
        ent->chkby = 0x10;
        ent->penby = 0x10;
    }
    else if (ent->collType == XENT_COLLTYPE_PLYR)
    {
        ent->pflags |= XENT_PUNK1;
        ent->move = NULL;
        ent->frame = (xEntFrame *)xMemAlloc(sizeof(xEntFrame), 0);

        memset(ent->frame, 0, sizeof(xEntFrame));

        ent->pflags |= XENT_PUNK2;
        ent->chkby = 0;
        ent->penby = 0;
        ent->collis = (xEntCollis *)xMemAlloc(sizeof(xEntCollis), 0);
        ent->collis->chk = 0x2F;
        ent->collis->pen = 0x2E;
        ent->collis->post = NULL;
        ent->collis->depenq = NULL;
    }

    if ((ent->moreFlags & XENT_ANIMCOLL) || (ent->flags & XENT_STACKABLE))
    {
        if (!ent->frame)
        {
            ent->frame = (xEntFrame *)xMemAlloc(sizeof(xEntFrame), 0);

            memset(ent->frame, 0, sizeof(xEntFrame));
        }
    }

    ent->baseFlags |= XBASE_ISENT;
}

xModelInstance *xEntLoadModel(xEnt *ent, RpAtomic *imodel)
{
    xModelInstance *model = xModelInstanceAlloc(imodel, ent, 0, 0, NULL);

    while (imodel = iModelFile_RWMultiAtomic(imodel))
    {
        xModelInstanceAttach(xModelInstanceAlloc(imodel, ent, XMODEL_UNK8, 0, NULL),
                             model);
    }

    if (ent)
    {
        ent->model = model;
    }

    return model;
}

void xEntRender(xEnt *ent)
{
    BFBBSTUB("xEntRender");
}

void xEntUpdate(xEnt *ent, xScene *sc, float dt)
{
    BFBBSTUB("xEntUpdate");
}

void xEntDefaultBoundUpdate(xEnt *ent, xVec3 *pos)
{
    BFBBSTUB("xEntDefaultBoundUpdate");
}

void xEntDefaultTranslate(xEnt *ent, xVec3 *dpos, xMat4x3 *dmat)
{
    BFBBSTUB("xEntDefaultTranslate");
}

xBox *xEntGetAllEntsBox()
{
    return &all_ents_box;
}