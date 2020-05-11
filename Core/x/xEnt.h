#ifndef XENT_H
#define XENT_H

#include "xBase.h"
#include "xMath3.h"
#include "xModel.h"
#include "xCollide.h"
#include "xGrid.h"
#include "xBound.h"
#include "xShadowSimple.h"

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
typedef unsigned int(*xEntCollisDepenqCallBack)(xEnt *, xEnt *, xScene *, float, xCollis *);

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

typedef void(*xEntUpdateCallBack)(xEnt *, xScene *, float);
typedef void(*xEntBoundUpdateCallBack)(xEnt *, xVec3 *);
typedef void(*xEntMoveCallBack)(xEnt *, xScene *, float, xEntFrame *);
typedef void(*xEntRenderCallBack)(xEnt *);
typedef void(*xEntTranslateCallBack)(xEnt *, xVec3 *, xMat4x3 *);

struct xFFX;
struct anim_coll_data;

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

void xEntSceneInit();

#endif