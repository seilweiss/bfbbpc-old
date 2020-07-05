#include "xEnt.h"

#include "xMemMgr.h"
#include "iModel.h"
#include "zBase.h"
#include "zPlatform.h"
#include "xEvent.h"
#include "xString.h"

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

static float nsn_angle = DEG2RAD(30.0f);
static float sEntityTimePassed;

static xBox all_ents_box;
static int all_ents_box_init;

namespace
{
namespace anim_coll
{
void reset(xEnt &ent)
{
    BFBBSTUB("anim_coll::reset");
}
}
}

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

static void xEntAddHittableFlag(xEnt *ent)
{
    if (ent->baseType == eBaseTypeNPC ||
        ent->baseType == eBaseTypeDestructObj ||
        ent->baseType == eBaseTypeButton ||
        ent->baseType == eBaseTypeBoulder ||
        (ent->baseType == eBaseTypePlatform && ent->subType == XPLATFORM_TYPE_PADDLE))
    {
        ent->moreFlags |= XENT_HITTABLE;
    }
    else
    {
        unsigned int i;

        for (i = 0; i < ent->linkCount; i++)
        {
            if (ent->link[i].srcEvent == eEventHit ||
                ent->link[i].srcEvent == eEventHit_Cruise ||
                ent->link[i].srcEvent == eEventHit_Melee ||
                ent->link[i].srcEvent == eEventHit_BubbleBounce ||
                ent->link[i].srcEvent == eEventHit_BubbleBash ||
                ent->link[i].srcEvent == eEventHit_BubbleBowl ||
                ent->link[i].srcEvent == eEventHit_PatrickSlam ||
                ent->link[i].srcEvent == eEventHit_Throw ||
                ent->link[i].srcEvent == eEventHit_PaddleLeft ||
                ent->link[i].srcEvent == eEventHit_PaddleRight)
            {
                ent->moreFlags |= XENT_HITTABLE;
                break;
            }
        }
    }
}

static void hack_receive_shadow(xEnt *ent)
{
    static unsigned int receive_models[] =
    {
        xStrHash("db03_path_a"),
        xStrHash("db03_path_b"),
        xStrHash("db03_path_c"),
        xStrHash("db03_path_d"),
        xStrHash("db03_path_e"),
        xStrHash("db03_path_f"),
        xStrHash("db03_path_g"),
        xStrHash("db03_path_h"),
        xStrHash("db03_path_i"),
        xStrHash("db03_path_j"),
        xStrHash("db03_path_k"),
        xStrHash("db03_path_l"),
        xStrHash("db03_path_m"),
        xStrHash("db03_path_o"),
        xStrHash("db03_path_p")
    };

    unsigned int *r5 = &receive_models[0];
    unsigned int *r4 = &receive_models[sizeof(receive_models) / sizeof(unsigned int)];

    while (r5 != r4)
    {
        if (ent->asset->modelInfoID == *r5)
        {
            ent->baseFlags |= XBASE_RECSHADOW;
            ent->asset->baseFlags |= XBASE_RECSHADOW;

            break;
        }

        r5++;
    }
}

static void xEntAddShadowRecFlag(xEnt *ent)
{
    switch (ent->baseType)
    {
    case eBaseTypePlatform:
    case eBaseTypeStatic:
    case eBaseTypeButton:
    case eBaseTypeDestructObj:
    case eBaseTypeNPC:
    case eBaseTypeBoulder:
    {
        if (ent->model->PipeFlags & XMODEL_PIPE_BLEND_MASK)
        {
            ent->baseFlags &= ~XBASE_RECSHADOW;
        }

        break;
    }
    default:
    {
        ent->baseFlags &= ~XBASE_RECSHADOW;
    }
    }

    hack_receive_shadow(ent);
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

namespace
{
void setup_stacked_entity(xEnt &ent)
{
    ent.pflags = XENT_PUNK4;
}
}

void xEntReset(xEnt *ent)
{
    xMat4x3 frame;
    xModelInstance *minst;

    xBaseReset(ent, ent->asset);

    ent->baseFlags |= XBASE_ISENT;
    ent->flags = ent->asset->flags;
    ent->miscflags = 0;
    ent->moreFlags = ent->asset->moreFlags;

    xEntAddHittableFlag(ent);
    xEntAddShadowRecFlag(ent);

    xMat3x3Euler(&frame, ent->asset->ang.x, ent->asset->ang.y, ent->asset->ang.z);
    
    xVec3SMulBy(&frame.right, ent->asset->scale.x);
    xVec3SMulBy(&frame.up, ent->asset->scale.y);
    xVec3SMulBy(&frame.at, ent->asset->scale.z);

    xVec3Copy(&frame.pos, &ent->asset->pos);

    frame.flags = 0;

    if (ent->model)
    {
        xModelSetFrame(ent->model, &frame);

        if (ent->collModel)
        {
            xModelSetFrame(ent->collModel, &frame);
        }

        if (ent->moreFlags & XENT_ANIMCOLL)
        {
            anim_coll::reset(*ent);
        }

        minst = ent->model;

        while (minst)
        {
            minst->RedMultiplier = ent->asset->redMult;
            minst->GreenMultiplier = ent->asset->greenMult;
            minst->BlueMultiplier = ent->asset->blueMult;

            minst->Alpha =
                (minst->Data->geometry->matList.materials[0]->color.alpha) / 255.0f;

            minst->Scale.x = 0.0f;
            minst->Scale.y = 0.0f;
            minst->Scale.z = 0.0f;

            minst = minst->Next;
        }
    }

    if (ent->frame)
    {
        xMat4x3Copy(&ent->frame->mat, &frame);
        
        ent->frame->oldmat = ent->frame->mat;

        xVec3Copy(&ent->frame->dpos, &g_O3);
        xVec3Copy(&ent->frame->dvel, &g_O3);
        xVec3Copy(&ent->frame->vel, &g_O3);
        xVec3Copy(&ent->frame->oldvel, &g_O3);

        xVec3Copy(&ent->frame->rot.axis, &ent->asset->ang);

        ent->frame->rot.angle = 0.0f;

        xRotCopy(&ent->frame->oldrot, &ent->frame->rot);
    }

    if (ent->bupdate && ent->model)
    {
        ent->bupdate(ent, (xVec3 *)&ent->model->Mat->pos);
    }

    ent->num_updates = xrand() & 127;

    if (ent->flags & XENT_STACKABLE)
    {
        setup_stacked_entity(*ent);
    }
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

void xEntBeginCollide(xEnt *ent, xScene *, float)
{
    BFBBSTUB("xEntBeginCollide");
}

void xEntSetNostepNormAngle(float angle)
{
    nsn_angle = angle;
}

xBox *xEntGetAllEntsBox()
{
    return &all_ents_box;
}

void xEntInitShadow(xEnt &ent, xEntShadow &shadow)
{
    ent.entShadow = &shadow;

    shadow.vec.assign(0.0f, 1.0f, 0.0f);
    shadow.pos = ent.asset->pos;
    shadow.shadowModel = NULL;
    shadow.dst_cast = -1.0f;
    shadow.radius[0] = -1.0f;
    shadow.radius[1] = -1.0f;
}

void xEntShow(xEnt *ent)
{
    ent->flags |= XENT_VISIBLE;
}