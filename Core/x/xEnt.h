#ifndef XENT_H
#define XENT_H

#include "xBase.h"
#include "xMath3.h"
#include "xModel.h"
#include "xCollide.h"
#include "xGrid.h"
#include "xBound.h"
#include "xShadowSimple.h"

#include "print.h"

struct xEntAsset : xBaseAsset
{
    unsigned char flags;
    unsigned char subtype;
    unsigned char pflags;
    unsigned char moreFlags;
    unsigned char pad;
    unsigned int surfaceID;
    xVec3 ang;
    xVec3 pos;
    xVec3 scale;
    float redMult;
    float greenMult;
    float blueMult;
    float seeThru;
    float seeThruSpeed;
    unsigned int modelInfoID;
    unsigned int animListID;
};

struct xEntFrame
{
    xMat4x3 mat;
    xMat4x3 oldmat;
    xVec3 oldvel;
    xRot oldrot;
    xRot drot;
    xRot rot;
    xVec3 dpos;
    xVec3 dvel;
    xVec3 vel;
    unsigned int mode;
};

struct xEnt;
struct xScene;
struct xEntCollis;

typedef void(*xEntCollisPostCallBack)(xEnt *, xScene *, float, xEntCollis *);
typedef unsigned int(*xEntCollisDepenqCallBack)(xEnt *, xEnt *, xScene *, float,
                                                xCollis *coll);

struct xEntCollis
{
    unsigned char chk;
    unsigned char pen;
    unsigned char env_sidx;
    unsigned char env_eidx;
    unsigned char npc_sidx;
    unsigned char npc_eidx;
    unsigned char dyn_sidx;
    unsigned char dyn_eidx;
    unsigned char stat_sidx;
    unsigned char stat_eidx;
    unsigned char idx;
    xCollis colls[18];
    xEntCollisPostCallBack post;
    xEntCollisDepenqCallBack depenq;
};

struct xEntShadow
{
    xVec3 pos;
    xVec3 vec;
    RpAtomic *shadowModel;
    float dst_cast;
    float radius[2];
};

typedef void(*xEntUpdateCallBack)(xEnt *ent, xScene *sc, float dt);
typedef void(*xEntBoundUpdateCallBack)(xEnt *ent, xVec3 *pos);
typedef void(*xEntMoveCallBack)(xEnt *ent, xScene *sc, float dt, xEntFrame *frame);
typedef void(*xEntRenderCallBack)(xEnt *ent);
typedef void(*xEntTranslateCallBack)(xEnt *ent, xVec3 *dpos, xMat4x3 *dmat);

struct xFFX;
struct anim_coll_data;

// flags
#define XENT_VISIBLE   0x1
#define XENT_STACKABLE 0x2
#define XENT_UNK40     0x40
#define XENT_UNK80     0x80

// pflags
#define XENT_PUNK1 0x1
#define XENT_PUNK2 0x2
#define XENT_PUNK4 0x4

// moreFlags
#define XENT_MESHCOLL 0x2
#define XENT_HITTABLE 0x10
#define XENT_ANIMCOLL 0x20

// collType
#define XENT_COLLTYPE_NA   0x0
#define XENT_COLLTYPE_TRIG 0x1
#define XENT_COLLTYPE_STAT 0x2
#define XENT_COLLTYPE_DYN  0x4
#define XENT_COLLTYPE_NPC  0x8
#define XENT_COLLTYPE_PLYR 0x10

// collLev
#define XENT_COLLLEV_UNK4 4
#define XENT_COLLLEV_UNK5 5

struct xEnt : xBase
{
    xEntAsset *asset;
    unsigned short idx;
    unsigned short num_updates;
    unsigned char flags;
    unsigned char miscflags;
    unsigned char subType;
    unsigned char pflags;
    unsigned char moreFlags;
    unsigned char isCulled;
    unsigned char driving_count;
    unsigned char num_ffx;
    unsigned char collType;
    unsigned char collLev;
    unsigned char chkby;
    unsigned char penby;
    xModelInstance *model;
    xModelInstance *collModel;
    xModelInstance *camcollModel;
    xLightKit *lightKit;
    xEntUpdateCallBack update;
    xEntUpdateCallBack endUpdate;
    xEntBoundUpdateCallBack bupdate;
    xEntMoveCallBack move;
    xEntRenderCallBack render;
    xEntFrame *frame;
    xEntCollis *collis;
    xGridBound gridb;
    xBound bound;
    xEntTranslateCallBack transl;
    xFFX *ffx;
    xEnt *driver;
    int driveMode;
    xShadowSimpleCache *simpShadow;
    xEntShadow *entShadow;
    anim_coll_data *anim_coll;
    void *user_data;
};

void xEntSetTimePassed(float sec);
void xEntSceneInit();
void xEntSceneExit();
void xEntUpdate(xEnt *ent, xScene *sc, float dt);
void xEntDefaultBoundUpdate(xEnt *ent, xVec3 *pos);
void xEntRender(xEnt *ent);
void xEntDefaultTranslate(xEnt *ent, xVec3 *dpos, xMat4x3 *dmat);
void xEntInit(xEnt *ent, xEntAsset *asset);
void xEntInitForType(xEnt *ent);
void xEntShow(xEnt *ent);
void xEntReset(xEnt *ent);
xModelInstance *xEntLoadModel(xEnt *ent, RpAtomic *imodel);
void xEntBeginCollide(xEnt *ent, xScene *, float);
void xEntSetNostepNormAngle(float angle);
xBox *xEntGetAllEntsBox();
void xEntInitShadow(xEnt &ent, xEntShadow &shadow);

inline void xEntEnable(xEnt *ent)
{
    xBaseEnable(ent);
}

inline unsigned int xEntIsVisible(const xEnt *ent)
{
    return ((ent->flags & (XENT_VISIBLE | XENT_UNK80)) == XENT_VISIBLE);
}

inline xMat4x3 *xEntGetFrame(const xEnt *ent)
{
    return xModelGetFrame(ent->model);
}

#endif