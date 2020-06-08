#ifndef ZENTPICKUP_H
#define ZENTPICKUP_H

#include "zEnt.h"
#include "zPickupTable.h"
#include "zParEmitter.h"
#include "xEntDrive.h"

struct zEntPickup : zEnt
{
    xShadowSimpleCache simpShadow_embedded;
    unsigned int state;
    zAssetPickup *p;
    void *anim;
    float animTime;
    float timer;
    float shake_timer;
    xVec3 shake_pos;
    int snackGateInfoDisplayed;
    float fx_timer;
    xPar *fx_par;
    zParEmitter *fx_emit;
    float fx_scale;
    xVec3 grab_pos;
    unsigned int flyflags;
    int flg_opts;
    xVec3 vel;
    xVec3 droppos;
    xBase *followTarget;
    xVec3 followOffset;
    xBase *dropParent;
    xEnt *useThisEntPos;
    xEntDrive drv;
    unsigned short pickupFlags;
};

void zEntPickup_Startup();
void zEntPickupInit(void *ent, void *asset);
void zEntPickup_Setup(zEntPickup *p);
void zEntPickup_Setup();
void zEntPickup_Reset(zEntPickup *ent);
void zEntPickup_FlushGrabbed();
void zEntPickup_Render(zEntPickup *plist, unsigned int pcount);
void zEntPickup_RenderList(zEntPickup *plist, unsigned int pcount);
void zEntPickup_SceneEnter();
void zEntPickup_RewardPostSetup();

#endif