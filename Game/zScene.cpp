#include "zScene.h"

#include "zVolume.h"
#include "xutil.h"
#include "xString.h"
#include "xFog.h"
#include "xSnd.h"
#include "zGame.h"
#include "zParPTank.h"
#include "zMain.h"
#include "xstransvc.h"
#include "iSnd.h"
#include "xPtankPool.h"
#include "xDecal.h"
#include "xFX.h"
#include "zLasso.h"
#include "zDispatcher.h"
#include "zNPCMgr.h"
#include "zAnimList.h"
#include "zGoo.h"
#include "zCollGeom.h"
#include "zUI.h"
#include "zUIFont.h"
#include "zTextBox.h"
#include "zTalkBox.h"
#include "zTaskBox.h"
#include "xCounter.h"
#include "zSurface.h"
#include "zDiscoFloor.h"
#include "zBase.h"
#include "zEntTrigger.h"
#include "zCamMarker.h"
#include "xSurface.h"
#include "zGust.h"
#include "zVolume.h"
#include "zLight.h"
#include "zEntPickup.h"
#include "zEntSimpleObj.h"
#include "xParSys.h"
#include "xParEmitter.h"
#include "zPlatform.h"
#include "zPendulum.h"
#include "zEntHangable.h"
#include "zEntDestructObj.h"
#include "xEntBoulder.h"
#include "zEntButton.h"
#include "xTimer.h"
#include "xCounter.h"
#include "xSFX.h"
#include "xGroup.h"
#include "zPortal.h"
#include "zConditional.h"
#include "zEnv.h"
#include "zUI.h"
#include "zUIFont.h"
#include "xFog.h"
#include "zLight.h"
#include "zCutsceneMgr.h"
#include "zEGenerator.h"
#include "zScript.h"
#include "xFFX.h"
#include "xPartition.h"
#include "zRenderState.h"
#include "zFeet.h"
#include "zParCmd.h"
#include "zCameraTweak.h"
#include "zActionLine.h"
#include "xScrFx.h"
#include "xShadow.h"
#include "zAssetTypes.h"
#include "zGameExtras.h"
#include "zLOD.h"
#include "zEntTeleportBox.h"
#include "zBusStop.h"
#include "zTaxi.h"
#include "zCameraFly.h"
#include "zThrown.h"
#include "zGrid.h"
#include "zNPCTypeBossSandy.h"
#include "zNPCTypeBossPatrick.h"
#include "zNPCTypeTiki.h"
#include "xEvent.h"
#include "xFloatAndVoid.h"
#include "xMathInlines.h"
#include "zFX.h"
#include "zParSys.h"
#include "xSystem.h"

#include "print.h"

#include <stdio.h>

unsigned char HACK_BASETYPE;

static unsigned int enableScreenAdj;

static int sMemDepthSceneStart = -1;
static int sMemDepthJustHIPStart = -1;

static unsigned int zSceneInitFunc_Default(zScene *s, zSceneObjectInstanceDesc *desc,
                                           unsigned int base_idx);
static unsigned int zSceneInitFunc_DefaultEnt(zScene *s, zSceneObjectInstanceDesc *desc,
                                              unsigned int base_idx);
static unsigned int zSceneInitFunc_Dispatcher(zScene *s, zSceneObjectInstanceDesc *desc,
                                              unsigned int base_idx);
static unsigned int zSceneInitFunc_LobMaster(zScene *s, zSceneObjectInstanceDesc *desc,
                                             unsigned int base_idx);
static unsigned int zSceneInitFunc_Volume(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx);
static unsigned int zSceneInitFunc_Gust(zScene *s, zSceneObjectInstanceDesc *desc,
                                        unsigned int base_idx);
static unsigned int zSceneInitFunc_Surface(zScene *s, zSceneObjectInstanceDesc *desc,
                                           unsigned int base_idx);
static unsigned int zSceneInitFunc_Camera(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx);
static unsigned int zSceneInitFunc_Player(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx);
static unsigned int zSceneInitFunc_SBNPC(zScene *s, zSceneObjectInstanceDesc *desc,
                                         unsigned int base_idx);
static unsigned int zSceneInitFunc_MovePoint(zScene *s, zSceneObjectInstanceDesc *desc,
                                             unsigned int base_idx);

static zSceneObjectInstanceDesc sInitTable[] =
{
//  "Trig", eBaseTypeTrigger, 'TRIG', sizeof(zEntTrigger), zSceneInitFunc_DefaultEnt, zEntTriggerInit, NULL,
//  "Move Point", eBaseTypeMovePoint, 'MVPT', sizeof(zMovePoint), zSceneInitFunc_MovePoint, NULL, NULL,
//  "Pickup", eBaseTypePickup, 'PKUP', sizeof(zEntPickup), zSceneInitFunc_DefaultEnt, zEntPickupInit, NULL,
//  "Simple", eBaseTypeStatic, 'SIMP', sizeof(zEntSimpleObj), zSceneInitFunc_DefaultEnt, zEntSimpleObj_Init, NULL,
//  "ParticleSystem", eBaseTypeParticleSystem, 'PARS', sizeof(xParSys), zSceneInitFunc_Default, xParSysInit, NULL,
//  "ParticleEmitter", eBaseTypeParticleEmitter, 'PARE', sizeof(xParEmitter), zSceneInitFunc_Default, xParEmitterInit, NULL,
//  "Track", eBaseTypeTrackPhysics, 'TRCK', sizeof(zEntSimpleObj), zSceneInitFunc_DefaultEnt, zEntTrackPhysics_Init, NULL,
//  "Platform", eBaseTypePlatform, 'PLAT', sizeof(zPlatform), zSceneInitFunc_DefaultEnt, zPlatform_Init, NULL,
//  "Pendulum", eBaseTypePendulum, 'PEND', sizeof(_zPendulum), zSceneInitFunc_DefaultEnt, zPendulum_Init, NULL,
//  "Hanger", eBaseTypeHangable, 'HANG', sizeof(zEntHangable), zSceneInitFunc_DefaultEnt, zEntHangable_Init, NULL,
//  "DestructObj", eBaseTypeDestructObj, 'DSTR', sizeof(zEntDestructObj), zSceneInitFunc_DefaultEnt, zEntDestructObj_Init, NULL,
//  "Boulder", eBaseTypeBoulder, 'BOUL', sizeof(xEntBoulder), zSceneInitFunc_DefaultEnt, xEntBoulder_Init, NULL,
//  "NPC", eBaseTypeNPC, 'VIL ', 0, zSceneInitFunc_SBNPC, NULL, NULL,
//  "Button", eBaseTypeButton, 'BUTN', sizeof(_zEntButton), zSceneInitFunc_DefaultEnt, zEntButton_Init, NULL,
//  "Player", eBaseTypePlayer, 'PLYR', sizeof(zEnt), zSceneInitFunc_Player, NULL, NULL,
//  "Timer", eBaseTypeTimer, 'TIMR', sizeof(xTimer), zSceneInitFunc_Default, xTimerInit, NULL,
//  "Counter", eBaseTypeCounter, 'CNTR', sizeof(_xCounter), zSceneInitFunc_Default, xCounterInit, NULL,
//  "SFX", eBaseTypeSFX, 'SFX ', sizeof(xSFX), zSceneInitFunc_Default, xSFXInit, NULL,
//  "Group", eBaseTypeGroup, 'GRUP', sizeof(xGroup), zSceneInitFunc_Default, xGroupInit, NULL,
//  "Portal", eBaseTypePortal, 'PORT', sizeof(_zPortal), zSceneInitFunc_Default, zPortalInit, NULL,
//  "Camera", eBaseTypeCamera, 'CAM ', sizeof(zCamMarker), zSceneInitFunc_Camera, NULL, NULL,
//  "Surface", eBaseTypeSurface, 'SURF', sizeof(xSurface), zSceneInitFunc_Surface, NULL, NULL,
//  "Gust", eBaseTypeGust, 'GUST', sizeof(zGust), zSceneInitFunc_Gust, NULL, NULL,
//  "Volume", eBaseTypeVolume, 'VOLU', sizeof(zVolume), zSceneInitFunc_Volume, NULL, NULL,
//  "Conditional", eBaseTypeCond, 'COND', sizeof(_zConditional), zSceneInitFunc_Default, zConditionalInit, NULL,
//  "Lob Master", eBaseTypeLobMaster, 'LOBM', 0, zSceneInitFunc_LobMaster, NULL, NULL,
    "Env", eBaseTypeEnv, 'ENV ', sizeof(_zEnv), zSceneInitFunc_Default, zEnvInit, NULL,
//  "Dispatcher", eBaseTypeDispatcher, 'DPAT', 0, zSceneInitFunc_Dispatcher, NULL, NULL,
//  "UI", eBaseTypeUI, 'UI  ', sizeof(_zUI), zSceneInitFunc_DefaultEnt, zUI_Init, NULL,
//  "UI Font", eBaseTypeUIFont, 'UIFT', sizeof(zUIFont), zSceneInitFunc_Default, zUIFont_Init, NULL,
//  "Fog", eBaseTypeFog, 'FOG ', sizeof(_xFog), zSceneInitFunc_Default, xFogInit, NULL,
//  "Light", eBaseTypeLight, 'LITE', sizeof(_zLight), zSceneInitFunc_Default, zLightInit, NULL,
//  "CutSceneMgr", eBaseTypeCutsceneMgr, 'CSNM', sizeof(zCutsceneMgr), zSceneInitFunc_Default, zCutsceneMgrInit, NULL,
//  "EGenerator", eBaseTypeEGenerator, 'EGEN', sizeof(zEGenerator), zSceneInitFunc_DefaultEnt, zEGenerator_Init, NULL,
//  "Script", eBaseTypeScript, 'SCRP', sizeof(_zScript), zSceneInitFunc_Default, zScriptInit, NULL,
//  "Disco Floor", eBaseTypeDiscoFloor, 'DSCO', sizeof(z_disco_floor), zSceneInitFunc_Default, z_disco_floor::init, NULL,
    0
};

/*
static zSceneObjectInstanceDesc sInitTable[] =
{
    0
};
*/

_tagClimate gClimate;
_zEnv *gCurEnv;

unsigned int gTransitionSceneID;

struct IDBasePair
{
    unsigned int id;
    xBase *base;
};

static IDBasePair *scobj_idbps;
static int scobj_size = -1;
static int nidbps = -1;

static void zSceneObjHashtableAdd(unsigned int id, xBase *base);

namespace
{
struct dynamic_type_data
{
    char *name;
    int type;
    unsigned int size;
    unsigned char is_ent;
    void(*load)(xBase &, xDynAsset &, unsigned int);
};

void add_dynamic_types(zScene &s)
{
    BFBBSTUB("add_dynamic_types");
}

unsigned int init_dynamic_type(zScene &s, unsigned int index, dynamic_type_data &d)
{
    BFBBSTUB("init_dynamic_type");
    return 0;
}

unsigned int init_dynamic_types(zScene &s, unsigned int index)
{
    BFBBSTUB("init_dynamic_types");
    return 0;
}
}

static void zSceneObjHashtableInit(int size);
static int zSceneObjHashtableUsage();
static xBase *zSceneExitSoundIteratorCB(xBase *b, zScene *s, void *data);
static void zSceneObjHashtableExit();
static xBase *zSceneObjHashtableGet(unsigned int id);

static unsigned int zSceneInitFunc_DefaultEnt(zScene *s, zSceneObjectInstanceDesc *desc,
                                              unsigned int base_idx)
{
    unsigned char *block = NULL;
    int count;
    unsigned int assetSize = 0;
    unsigned int offset;
    xBase *b;
    int i;
    void *asset;

    count = s->baseCount[desc->baseType];

    if (count)
    {
        block = (unsigned char *)xMemAlloc(count * desc->sizeRuntime, 0);

        s->baseList[desc->baseType] = (xBase *)block;
    }

    for (i = 0; i < count; i++)
    {
        asset = xSTFindAssetByType(desc->assetType, i, &assetSize);
        offset = i * desc->sizeRuntime;
        b = (xBase *)(block + offset);

        zSceneSet(b, base_idx);

        if (desc->objectInitFunc)
        {
            desc->objectInitFunc(b, asset);
        }

        xSceneAddEnt(s, s->ents[base_idx]);
        zSceneObjHashtableAdd(b->id, b);

        base_idx++;
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Default(zScene *s, zSceneObjectInstanceDesc *desc,
                                           unsigned int base_idx)
{
    unsigned char *block = NULL;
    int count;
    unsigned int assetSize = 0;
    unsigned int offset;
    xBase *b;
    int i;
    void *asset;

    count = s->baseCount[desc->baseType];

    if (count)
    {
        block = (unsigned char *)xMemAlloc(count * desc->sizeRuntime, 0);

        s->baseList[desc->baseType] = (xBase *)block;
    }

    for (i = 0; i < count; i++)
    {
        asset = xSTFindAssetByType(desc->assetType, i, &assetSize);
        offset = i * desc->sizeRuntime;
        b = (xBase *)(block + offset);

        zSceneSet(b, base_idx);

        if (desc->objectInitFunc)
        {
            desc->objectInitFunc(b, asset);
        }

        zSceneObjHashtableAdd(b->id, b);

        base_idx++;
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_MovePoint(zScene *s, zSceneObjectInstanceDesc *desc,
                                             unsigned int base_idx)
{
    int count;
    unsigned int assetSize = 0;
    zMovePoint *movpBlock;
    int idx;
    xBase *b;
    xBaseAsset *basset;

    count = s->baseCount[desc->baseType];
    movpBlock = zMovePoint_GetMemPool(count);

    if (movpBlock)
    {
        s->baseList[desc->baseType] = movpBlock;
    }

    for (idx = 0; idx < count; idx++)
    {
        b = zMovePoint_GetInst(idx);
        basset = (xBaseAsset *)xSTFindAssetByType('MVPT', idx, &assetSize);

        zSceneSet(b, base_idx);
        
        zMovePointInit(zMovePoint_GetInst(idx), (xMovePointAsset *)basset);

        zSceneObjHashtableAdd(b->id, b);

        base_idx++;
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_SBNPC(zScene *s, zSceneObjectInstanceDesc *desc,
                                         unsigned int base_idx)
{
    int count;
    int i;
    xEnt *npc;
    xEntAsset *assdat;

    count = s->baseCount[desc->baseType];

    if (!count)
    {
        return base_idx;
    }

    s->baseList[desc->baseType] = NULL;

    for (i = 0; i < count; i++)
    {
        assdat = (xEntAsset *)xSTFindAssetByType('VIL ', i, NULL);
        npc = zNPCMgr_createNPCInst(assdat);

        zSceneSet(npc, base_idx);
        xSceneAddEnt(s, s->ents[base_idx]);
        zSceneObjHashtableAdd(npc->id, npc);

        base_idx++;
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Player(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx)
{
    int count;
    zEnt *entBlock;
    int idx;
    xBase *b;
    xEntAsset *asset;

    count = s->baseCount[desc->baseType];

    if (count)
    {
        entBlock = (zEnt *)xMemAlloc(count * sizeof(zEnt), 0);
        b = &globals.player.ent;

        s->baseList[desc->baseType] = entBlock;

        for (idx = 0; idx < count; idx++)
        {
            zSceneSet(b, base_idx);

            if (idx == (count - 1))
            {
                xSceneAddEnt(s, s->ents[base_idx]);
            }

            asset = (xEntAsset *)xSTFindAssetByType('PLYR', idx, NULL);

            b->id = asset->id;

            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Camera(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx)
{
    int count;
    zCamMarker *camBlock;
    int idx;
    xBase *b;
    xCamAsset *assetCam;

    count = s->baseCount[desc->baseType];

    if (count)
    {
        camBlock = (zCamMarker *)xMemAlloc(count * sizeof(zCamMarker), 0);

        s->baseList[desc->baseType] = camBlock;

        for (idx = 0; idx < count; idx++)
        {
            b = &camBlock[idx];
            assetCam = (xCamAsset *)xSTFindAssetByType('CAM ', idx, NULL);

            zSceneSet(b, base_idx);

            zCamMarkerInit(b, assetCam);

            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Surface(zScene *s, zSceneObjectInstanceDesc *desc,
                                           unsigned int base_idx)
{
    int count;
    int idx;
    xBase *b;

    count = s->baseCount[desc->baseType];

    if (count)
    {
        s->baseList[desc->baseType] = xSurfaceGetByIdx(0);

        for (idx = 0; idx < count; idx++)
        {
            b = xSurfaceGetByIdx(idx);

            zSceneSet(b, base_idx);
            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Gust(zScene *s, zSceneObjectInstanceDesc *desc,
                                        unsigned int base_idx)
{
    int count;
    int idx;
    xBase *b;

    count = s->baseCount[desc->baseType];

    zGustInit();

    if (count)
    {
        s->baseList[desc->baseType] = zGustGetGust(0);

        for (idx = 0; idx < count; idx++)
        {
            b = zGustGetGust(idx);

            zSceneSet(b, base_idx);
            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_Volume(zScene *s, zSceneObjectInstanceDesc *desc,
                                          unsigned int base_idx)
{
    int count;
    int idx;
    xBase *b;

    count = s->baseCount[desc->baseType];

    zVolumeInit();

    if (count)
    {
        s->baseList[desc->baseType] = zVolumeGetVolume(0);

        for (idx = 0; idx < count; idx++)
        {
            b = zVolumeGetVolume(idx);

            zLightSetVolume((zVolume *)b);

            zSceneSet(b, base_idx);
            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

static unsigned int zSceneInitFunc_LobMaster(zScene *s, zSceneObjectInstanceDesc *desc,
                                             unsigned int base_idx)
{
    return base_idx;
}

static unsigned int zSceneInitFunc_Dispatcher(zScene *s, zSceneObjectInstanceDesc *desc,
                                              unsigned int base_idx)
{
    int count;
    st_ZDISPATCH_DATA *dpat_pool;
    int idx;
    xBase *b;
    xEntAsset *asset; /* should be xBaseAsset lol */

    count = s->baseCount[desc->baseType];

    if (count)
    {
        dpat_pool = zDispatcher_memPool(count);

        s->baseList[desc->baseType] = dpat_pool;

        for (idx = 0; idx < count; idx++)
        {
            b = zDispatcher_getInst(dpat_pool, idx);
            asset = (xEntAsset *)xSTFindAssetByType('DPAT', idx, NULL);

            zSceneSet(b, base_idx);

            zDispatcher_Init((st_ZDISPATCH_DATA *)b, asset);

            zSceneObjHashtableAdd(b->id, b);

            base_idx++;
        }
    }

    return base_idx;
}

void zSceneSet(xBase *b, unsigned int idx)
{
    globals.sceneCur->base[idx] = b;
}

static void PipeCountStuffCB(RpAtomic *, unsigned int pipeFlags, unsigned int subObjects)
{
    BFBBSTUB("PipeCountStuffCB");
}

static void PipeAddStuffCB(RpAtomic *data, unsigned int pipeFlags, unsigned int subObjects)
{
    BFBBSTUB("PipeAddStuffCB");
}

static void PipeForAllSceneModels(
    void(*pipeCB)(RpAtomic *data, unsigned int pipeFlags, unsigned int subObjects))
{
    BFBBSTUB("PipeForAllSceneModels");
}

void zSceneInitEnvironmentalSoundEffect()
{
    BFBBSTUB("zSceneInitEnvironmentalSoundEffect");
}

static unsigned int BaseTypeNeedsUpdate(unsigned char baseType)
{
    switch (baseType)
    {
    case eBaseTypeUnknown:
    case eBaseTypePlayer:
    case eBaseTypeEnv:
    case eBaseTypeCamera:
    case eBaseTypeStatic:
    case eBaseTypeMovePoint:
    case eBaseTypeBubble:
    case eBaseTypePortal:
    case eBaseTypeGroup:
    case eBaseTypeSFX:
    case eBaseTypeFFX:
    case eBaseTypeVFX:
    case eBaseTypeCounter:
    case eBaseTypeProjectile:
    case eBaseTypeGust:
    case eBaseTypeVolume:
    case eBaseTypeDispatcher:
    case eBaseTypeCond:
    case eBaseTypeUI:
    case eBaseTypeProjectileType:
    case eBaseTypeLobMaster:
    case eBaseTypeCutsceneMgr:
    case eBaseTypeHud:
    case eBaseTypeNPCProps:
    case eBaseTypeParticleEmitterProps:
    case eBaseTypeCruiseBubble:
    case eBaseTypeTextBox:
    case eBaseTypeTalkBox:
    case eBaseTypeTaskBox:
    case eBaseTypeBoulderGenerator:
    case eBaseTypeNPCSettings:
    case eBaseTypeTurret:
        return 0;
    }

    return 1;
}

void zSceneInit(unsigned int theSceneID, int reloadInProgress)
{
    float pdone;
    zScene *s;
    unsigned char rgba_bkgrd[4] = { 15, 15, 15, 00 };

    gTransitionSceneID = theSceneID;
    gOccludeCount = 0;

    char b[5] = {};

    sprintf(b, xUtil_idtag2string(theSceneID, 0));
    xStrupr(b);

    theSceneID =
        (b[0] << 24) |
        (b[1] << 16) |
        (b[2] << 8) |
        b[3];

    xUtil_idtag2string(theSceneID, 0);

    xFogClearFog();
    xSndSceneInit();

    if (!reloadInProgress)
    {
        sMemDepthSceneStart = xMemPushBase();
    }

    zGameScreenTransitionBegin();

    zParPTankInit();

    pdone = 2.0f;

    if (globals.useHIPHOP && !reloadInProgress)
    {
        zGameScreenTransitionUpdate(pdone, "... scene preload ...\n");

        int ver_hop = xSTPreLoadScene(theSceneID, NULL, XST_SCENE_HOP);

        if (ver_hop >= 0x000A000F)
        {
            xSTQueueSceneAssets(theSceneID, XST_SCENE_HOP);

            do
            {
                rgba_bkgrd[0] = 0;
                rgba_bkgrd[1] = 0;
                rgba_bkgrd[2] = 0;
                rgba_bkgrd[3] = 0;

                pdone += 2.0f;

                zGameScreenTransitionUpdate(pdone, "... scene loading ...\n", rgba_bkgrd);
            }
            while (xSTLoadStep(theSceneID) < 1.0f);

            xSTDisconnect(theSceneID, XST_SCENE_HOP);
        }
    }

    sMemDepthJustHIPStart = xMemPushBase();

    s = (zScene *)xMemAlloc(sizeof(zScene), 0);
    globals.sceneCur = s;

    xSceneInit(s, 200, 2048, 2068, 250);

    s->env = (xEnv *)xMemAlloc(sizeof(xEnv), 0);
    s->sceneID = theSceneID;

    iTime time;

    time = iTimeGet();
    xUtil_idtag2string(theSceneID, 0);
    iTimeDiffSec(time);

    xSTPreLoadScene(theSceneID, NULL, XST_SCENE_HIP);

    time = iTimeGet();
    xUtil_idtag2string(theSceneID, 0);
    iTimeDiffSec(time);

    xSTQueueSceneAssets(theSceneID, XST_SCENE_HIP);

    time = iTimeGet();
    xUtil_idtag2string(theSceneID, 0);
    iTimeDiffSec(time);

    pdone += 2.0f;

    zGameScreenTransitionUpdate(pdone, "... scene asset queue ...\n");

    time = iTimeGet();
    xUtil_idtag2string(theSceneID, 0);
    iTimeDiffSec(time);

    do
    {
        rgba_bkgrd[0] = 0;
        rgba_bkgrd[1] = 0;
        rgba_bkgrd[2] = 0;
        rgba_bkgrd[3] = 0;

        pdone += 2.0f;

        zGameScreenTransitionUpdate(pdone, "... scene loading ...\n", rgba_bkgrd);
    }
    while (xSTLoadStep(theSceneID) < 1.0f);

    xSTDisconnect(theSceneID, XST_SCENE_HIP);

    time = iTimeGet();
    xUtil_idtag2string(theSceneID, 0);
    iTimeDiffSec(time);

    pdone += 2.0f;

    zGameScreenTransitionUpdate(pdone, "...initializing scene - sound\n");

    zEntPlayer_LoadSounds();
    iSndInitSceneLoaded();

    xPadRumbleEnable(globals.currentActivePad, globals.option_vibration);

    xSTSwitchScene(theSceneID, NULL, NULL);

    globals.sceneCur->resolvID = zSceneFindObject;
    globals.sceneCur->id2Name = zSceneGetName;
    globals.sceneCur->base2Name = zSceneGetName;

    g_xSceneCur = globals.sceneCur;

    xModelPipeNumTables = xSTAssetCountByType('PIPT');

    for (int i = 0; i < xModelPipeNumTables; i++)
    {
        void *data = xSTFindAssetByType('PIPT', i, NULL);

        xModelPipeCount[i] = *(int *)data;
        xModelPipeData[i] = (xModelPipeInfo *)((int *)data + 1);
    }

    xModelLookupCount = 0;
    PipeForAllSceneModels(PipeCountStuffCB);

    if (xModelLookupCount)
    {
        xModelLookupList = (xModelPipeLookup *)xMemAlloc(
            xModelLookupCount * sizeof(xModelPipeLookup), 0);

        PipeForAllSceneModels(PipeAddStuffCB);
    }

    xModelBucket_PreCountReset();
    PipeForAllSceneModels(xModelBucket_PreCountBucket);
    xModelBucket_PreCountAlloc(256);
    PipeForAllSceneModels(xModelBucket_InsertBucket);

    xModelBucket_Init();

    add_scene_tweaks();

    xPTankPoolSceneEnter();
    zParPTankSceneEnter();
    xDecalInit();
    xFXScenePrepare();
    zLasso_scenePrepare();
    zDispatcher_scenePrepare();

    int total_npcs = xSTAssetCountByType('VIL ');
    zNPCMgr_scenePrepare(total_npcs);

    zAnimListInit();
    zGooInit(24);

    zGameScreenTransitionUpdate(100.0f, "...initializing scene - base types\n");

    for (int i = 0; i < 72; i++)
    {
        s->baseCount[i] = 0;
        s->baseList[i] = NULL;
    }

    zCollGeom_Init();
    zUI_Init();
    zUIFontInit();
    ztextbox::init();
    ztalkbox::init();
    ztaskbox::init();
    xCounterInit();
    zSurfaceInit();
    z_disco_floor::init();

    xModelInstStaticAlloc = 1;

    s->num_base = 0;

    zSceneObjectInstanceDesc *desc = sInitTable;

    while (desc->name)
    {
        unsigned int typeCount = xSTAssetCountByType(desc->assetType);

        s->baseCount[desc->baseType] = typeCount;
        s->num_base += typeCount;

        if (desc->querySubObjects)
        {
            for (unsigned int j = 0; j < typeCount; j++)
            {
                s->num_base +=
                    desc->querySubObjects(xSTFindAssetByType(desc->assetType, j, NULL));
            }
        }

        desc++;
    }

    add_dynamic_types(*s);

    if (s->num_base)
    {
        s->base = (xBase **)xMemAlloc(s->num_base * sizeof(xBase *), 0);
    }
    else
    {
        s->base = NULL;
    }

    zSceneObjHashtableInit(4096);

    xFFXPoolInit(12);
    xFFXShakePoolInit(3);
    xFFXRotMatchPoolInit(1);

    xEntSceneInit();

    xEntMotionDebugInit(
        s->baseCount[eBaseTypePlatform] +
        s->baseCount[eBaseTypePendulum] +
        s->baseCount[eBaseTypeButton]);

    zLightSetVolume(NULL);
    xPartitionReset();
    xFXRibbonSceneEnter();

    unsigned int base_idx = 0;
    desc = sInitTable;

    while (desc->name)
    {
        HACK_BASETYPE = desc->baseType;

        if (desc->func)
        {
            base_idx = desc->func(s, desc, base_idx);
        }

        desc++;
    }

    init_dynamic_types(*s, base_idx);

    s->num_update_base = 0;

    for (int i = 0; i < s->num_base; i++)
    {
        if (BaseTypeNeedsUpdate(s->base[i]->baseType))
        {
            s->num_update_base++;
        }
    }

    s->update_base = (xBase **)xMemAlloc(s->num_update_base * sizeof(xBase *), 0);

    base_idx = 0;

    for (int i = 0; i < s->num_base; i++)
    {
        if (BaseTypeNeedsUpdate(s->base[i]->baseType))
        {
            s->update_base[base_idx] = s->base[i];

            base_idx++;
        }
    }

    xModelInstStaticAlloc = 0;

    zGameScreenTransitionEnd();

    zSceneObjHashtableUsage();
    zUI_ScenePortalInit(s);
    zLightResolveLinks();
    zRenderStateInit();
    xFXStreakInit();
    xFXShineInit();
    xFXFireworksInit("PAREMIT_FIREWORKS_TRAIL", "PAREMIT_FIREWORKS1",
                     "PAREMIT_FIREWORKS2", "Fireworks_explode", "Fireworks_trail");
    zFeetGetIDs();
    zLightningInit();
    zParCmdFindClipVolumes();
    zEntDestructObj_FindFX();
    zShrapnel_SceneInit(globals.sceneCur);
    zCameraTweakGlobal_Reset();
    zActionLineInit();
    xScrFxLetterboxReset();
    xShadowManager_Init(s->baseCount[eBaseTypeNPC] + 10);

    int lkitCount = xSTAssetCountByType('LKIT');

    for (int i = 0; i < lkitCount; i++)
    {
        void *lkitData = xSTFindAssetByType('LKIT', i, NULL);
        xLightKit_Prepare(lkitData);
    }

    xClimateInit(&gClimate);
    zSceneInitEnvironmentalSoundEffect();

    sHackSmoothedUpdate = 1;

    FootstepHackSceneEnter();
    zEntPickup_SceneEnter();
    xFXSceneInit();
    zGame_HackGalleryInit();
    iSndSuspendCD(0);
}

void add_scene_tweaks()
{
    return;
}

void zSceneExit(int beginReload)
{
    zScene *s = globals.sceneCur;

    zThrown_Reset();
    zEntPickup_FlushGrabbed();
    zGridExit(s);

    zSceneForAllBase(zSceneExitSoundIteratorCB, NULL);

    xSndStopAll(~0x4);
    xSndUpdate();

    iSndWaitForDeadSounds();
    iSndSceneExit();

    xSFXEnvironmentalStreamSceneExit();

    iSndSuspendCD(1);

    iFuncProfileDump();

    RpWorld *world = s->env->geom->world;

    xModel_SceneExit(world);
    zFX_SceneExit(world);

    zEntEventAll(NULL, eEventUnknown, eEventRoomEnd, NULL);
    zEntEventAll(NULL, eEventUnknown, eEventSceneEnd, NULL);

    if (globals.sceneCur->pendingPortal)
    {
        char nextScene[8];
        char curScene[8];

        strcpy(nextScene,
               xUtil_idtag2string(globals.sceneCur->pendingPortal->passet->sceneID, 0));

        strcpy(curScene,
               xUtil_idtag2string(globals.sceneCur->sceneID, 0));
    }

    gOccludeCount = 0;

    if (globals.sceneCur->sceneID != 'MNU3')
    {
        zSceneSave(s, NULL);
    }

    zEntPlayerExit(&globals.player.ent);
    zSurfaceExit();
    zLightDestroyAll();
    xEntSceneExit();
    xEntMotionDebugExit();
    zSceneObjHashtableExit();

    if (s->baseCount[eBaseTypeParticleSystem])
    {
        zParSys *ps = (zParSys *)s->baseList[eBaseTypeParticleSystem];

        for (unsigned int i = 0; i < s->baseCount[eBaseTypeParticleSystem]; i++)
        {
            if (xBaseIsValid(&ps[i]))
            {
                xParSysExit(&ps[i]);
            }
        }
    }

    xParEmitterDestroy();
    xModelBucket_Deinit();
    xFXSceneFinish();
    zGooExit();
    zParPTankExit();
    zAnimListExit();
    zNPCMgr_sceneFinish();
    zDispatcher_sceneFinish();
    z_disco_floor::destroy();
    xDecalDestroy();
    zParPTankSceneExit();
    xPTankPoolSceneExit();
    zEntPlayer_UnloadSounds();

    if (beginReload)
    {
        xSTUnLoadScene(globals.sceneCur->sceneID, XST_SCENE_HIP);

        xMemPopBase(sMemDepthJustHIPStart);

        sMemDepthJustHIPStart = -1;
    }
    else
    {
        xSTUnLoadScene(globals.sceneCur->sceneID, XST_SCENE_HIP);

        if (globals.useHIPHOP)
        {
            xSTUnLoadScene(globals.sceneCur->sceneID, XST_SCENE_HOP);
        }

        xMemPopBase(sMemDepthSceneStart);

        sMemDepthSceneStart = -1;
        sMemDepthJustHIPStart = -1;
    }

    xSystem_GapTrackReport();

    xUtil_idtag2string(globals.sceneCur->sceneID, 0);

    globals.sceneCur = NULL;

    xSceneExit(s);
}

void zSceneSave(zScene *ent, xSerial *s)
{
    BFBBSTUB("zSceneSave");
}

static void ActivateCB(xBase *base)
{
    BFBBSTUB("ActivateCB");
}

static void DeactivateCB(xBase *base)
{
    BFBBSTUB("DeactivateCB");
}

void zSceneSetup()
{
    zScene *s = globals.sceneCur;

    globals.cmgr = NULL;

    xSceneSetup(s);

    gUIMgr.Setup(s);

    s->gravity = -50.0f;
    s->drag = 0.08f;
    s->flags = 0x5;

    zNPCMgr_scenePostInit();

    if (s->baseCount[eBaseTypeGust])
    {
        zGustSetup();
    }

    if (s->baseCount[eBaseTypeVolume])
    {
        zVolumeSetup();
    }

    unsigned int dontcaresize;

    xCutscene_Init(xSTFindAssetByType('CTOC', 0, &dontcaresize));
    zLOD_Setup();

    gCurEnv = NULL;

    for (unsigned int i = 0; i < s->num_base; i++)
    {
        if (s->base[i])
        {
            switch (s->base[i]->baseType)
            {
            case eBaseTypeEnv:
            {
                gCurEnv = (_zEnv *)s->base[i];

                zEnvSetup(gCurEnv);
                xClimateInitAsset(&gClimate, gCurEnv->easset);

                break;
            }
            case eBaseTypeNPC:
            {
                ((xNPCBasic *)s->base[i])->PostInit();

                break;
            }
            case eBaseTypePlatform:
            {
                zPlatform_Reset((zPlatform *)s->base[i], s);
                zPlatform_Setup((zPlatform *)s->base[i], s);

                break;
            }
            case eBaseTypeMovePoint:
            {
                zMovePointSetup((zMovePoint *)s->base[i], s);

                break;
            }
            case eBaseTypePendulum:
            {
                zPendulum_Reset((_zPendulum *)s->base[i], s);
                zPendulum_Setup((_zPendulum *)s->base[i], s);

                break;
            }
            case eBaseTypeButton:
            {
                zEntButton_Reset((_zEntButton *)s->base[i], s);
                zEntButton_Setup((_zEntButton *)s->base[i], s);

                break;
            }
            case eBaseTypeStatic:
            {
                zEntSimpleObj_Setup((zEntSimpleObj *)s->base[i]);

                break;
            }
            case eBaseTypeTrackPhysics:
            {
                zEntSimpleObj_Setup((zEntSimpleObj *)s->base[i]);

                break;
            }
            case eBaseTypeDispatcher:
            {
                zDispatcher_InitDep((st_ZDISPATCH_DATA *)s->base[i], s);

                break;
            }
            case eBaseTypeEGenerator:
            {
                zEGenerator_Setup((zEGenerator *)s->base[i], s);
                zEGenerator_Reset((zEGenerator *)s->base[i], s);

                break;
            }
            case eBaseTypePickup:
            {
                zEntPickup_Setup((zEntPickup *)s->base[i]);
                zEntPickup_Reset((zEntPickup *)s->base[i]);

                break;
            }
            case eBaseTypeParticleSystem:
            {
                xParSysSetup((xParSys *)s->base[i]);

                break;
            }
            case eBaseTypeSurface:
            {
                zSurfaceSetup((xSurface *)s->base[i]);

                break;
            }
            case eBaseTypeParticleEmitter:
            {
                xParEmitterSetup((xParEmitter *)s->base[i]);

                break;
            }
            case eBaseTypeGroup:
            {
                xGroupSetup((xGroup *)s->base[i]);

                break;
            }
            case eBaseTypeTeleportBox:
            {
                zEntTeleportBox_Setup((_zEntTeleportBox *)s->base[i]);

                break;
            }
            case eBaseTypeBusStop:
            {
                zBusStop_Setup((zBusStop *)s->base[i]);

                break;
            }
            case eBaseTypeDiscoFloor:
            {
                ((z_disco_floor *)s->base[i])->setup();

                break;
            }
            case eBaseTypeTaxi:
            {
                zTaxi_Setup((zTaxi *)s->base[i]);

                break;
            }
            case eBaseTypeCameraFly:
            {
                zCameraFly_Setup((zCameraFly *)s->base[i]);

                break;
            }
            case eBaseTypeDestructObj:
            {
                zEntDestructObj_Setup((zEntDestructObj *)s->base[i]);

                break;
            }
            case eBaseTypeBoulder:
            {
                xEntBoulder_Setup((xEntBoulder *)s->base[i]);

                break;
            }
            }
        }
    }

    zEntPickup_Setup();
    zEntTeleportBox_InitAll();
    zEntHangable_SetupFX();
    zThrown_Setup(globals.sceneCur);

    for (unsigned int i = 0; i < s->num_base; i++)
    {
        if (s->base[i] && s->base[i]->baseType == eBaseTypeMovePoint)
        {
            xMovePointSplineSetup((xMovePoint *)s->base[i]);
        }
    }

    xFXSceneSetup();
    zSceneEnableVisited(s);
    xSerialTraverse(zSceneSetup_serialTraverseCB);
    xQuickCullInit(xEntGetAllEntsBox());
    zGridInit(s);
    zNPCBSandy_AddBoundEntsToGrid(s);
    zNPCBPatrick_AddBoundEntsToGrid(s);
    zNPCTiki_InitStacking(globals.sceneCur);
    zNPCTiki_InitFX(globals.sceneCur);

    enableScreenAdj = 0;

    for (unsigned int i = 0; i < s->num_ents; i++)
    {
        if (s->ents[i]->baseType == eBaseTypeNPC)
        {
            xEnt *ent = s->ents[i];

            zLODTable *lod = zLOD_Get(ent);
            RpAtomic *tryshad = NULL;

            if (lod->lodBucket[2])
            {
                tryshad = (*lod->lodBucket[2])->Data;
            }
            else if (lod->lodBucket[1])
            {
                tryshad = (*lod->lodBucket[1])->Data;
            }
            else if (lod->lodBucket[0])
            {
                tryshad = (*lod->lodBucket[0])->Data;
            }

            if (tryshad && RpClumpGetNumAtomics(RpAtomicGetClump(tryshad)) == 1)
            {
                ent->entShadow->shadowModel = tryshad;
            }
        }
    }

    int max_drivensort_iters = 256;
    unsigned int driven_swapped;

    do
    {
        driven_swapped = 0;

        for (unsigned int i = 0; i < s->num_update_base; i++)
        {
            if (s->update_base[i]->baseFlags & XBASE_ISENT)
            {
                xEnt *bdriven = (xEnt *)s->update_base[i];

                if (bdriven->driver)
                {
                    for (unsigned int j = i + 1; j < s->num_update_base; j++)
                    {
                        if (bdriven->driver == s->update_base[j])
                        {
                            driven_swapped = 1;

                            xBase *btmp = s->update_base[i];

                            s->update_base[i] = s->update_base[j];
                            s->update_base[j] = btmp;
                        }
                    }
                }
            }
        }

        max_drivensort_iters--;
    }
    while (max_drivensort_iters && driven_swapped);

    {
        int i;
        unsigned int f;

        xEnvAsset *easset = globals.sceneCur->zen->easset;

        if (easset->bspLightKit)
        {
            globals.sceneCur->env->lightKit =
                (xLightKit *)xSTFindAsset(easset->bspLightKit, NULL);
        }

        if (easset->objectLightKit)
        {
            xLightKit *objLightKit =
                (xLightKit *)xSTFindAsset(easset->objectLightKit, NULL);

            if (objLightKit)
            {
                zScene *zsc = globals.sceneCur;

                for (i = 0; i < zsc->num_ents; i++)
                {
                    if ((zsc->ents[i]->baseFlags & XBASE_ISENT))
                    {
                        xEnt *tgtent = zsc->ents[i];

                        if (tgtent->model &&
                            (tgtent->model->PipeFlags & 0xC0) == 0x40)
                        {
                            tgtent->lightKit = objLightKit;
                        }
                    }
                }
            }
        }

        int lkitCount = xSTAssetCountByType('LKIT');

        for (i = 0; i < lkitCount; i++)
        {
            xLightKit *lkit = (xLightKit *)xSTFindAssetByType('LKIT', i, NULL);

            if (lkit->groupID)
            {
                xGroup *group = (xGroup *)zSceneFindObject(lkit->groupID);

                if (group)
                {
                    unsigned int j, nitam;

                    nitam = xGroupGetCount(group);

                    for (j = 0; j < nitam; j++)
                    {
                        xBase *itamz = xGroupGetItemPtr(group, j);

                        if (itamz && (itamz->baseFlags & XBASE_ISENT))
                        {
                            xEnt *entitam = (xEnt *)itamz;

                            if (entitam->model &&
                                (entitam->model->PipeFlags & 0xC0) == 0x40)
                            {
                                entitam->lightKit = lkit;
                            }
                        }
                    }
                }
            }
        }
    }

    xEnt **entList;
    unsigned int entCount;

    entList = &s->act_ents[
        s->baseCount[eBaseTypeTrigger] +
        s->baseCount[eBaseTypePickup]];

    entCount = s->baseCount[eBaseTypeStatic];

    zEntSimpleObj_MgrInit((zEntSimpleObj **)entList, entCount);

    entList = &s->act_ents[
        s->baseCount[eBaseTypeTrigger] +
        s->baseCount[eBaseTypePickup]];

    entCount =
        s->baseCount[eBaseTypeStatic] +
        s->baseCount[eBaseTypePlatform] +
        s->baseCount[eBaseTypePendulum] +
        s->baseCount[eBaseTypeHangable] +
        s->baseCount[eBaseTypeDestructObj] +
        s->baseCount[eBaseTypeBoulder] +
        s->baseCount[eBaseTypeNPC] +
        s->baseCount[eBaseTypeButton];

    unsigned int i, j, k;
    unsigned int numPrimeMovers = 0; // r24
    unsigned int numDriven = 0; // r25

    for (i = 0; i < s->num_ents; i++)
    {
        if (s->ents[i]->baseFlags & XBASE_ISENT)
        {
            s->ents[i]->isCulled = 0;
        }
    }

    for (i = 0; i < s->num_ents; i++)
    {
        if (s->ents[i]->baseFlags & XBASE_ISENT)
        {
            xEnt *ent = s->ents[i];

            if (ent->driver)
            {
                if (!ent->isCulled)
                {
                    numPrimeMovers++;
                    ent->isCulled = 2;
                }

                while (ent->driver)
                {
                    ent = ent->driver;
                }

                if (!ent->isCulled)
                {
                    numDriven++;
                    ent->isCulled = 1;
                }
            }
        }
    }

    unsigned int numGroups = 0; // r23

    for (i = 0; i < s->num_base; i++)
    {
        if (s->base[i]->baseType == eBaseTypeGroup)
        {
            xGroup *grp = (xGroup *)s->base[i];

            for (j = 0; j < grp->linkCount; j++)
            {
                // should be grp->link[j] ...?
                if (grp->link[0].srcEvent == eEventSceneBegin &&
                    grp->link[0].dstEvent == eEventGroupUpdateTogether)
                {
                    numGroups++;

                    unsigned int gcnt = xGroupGetCount(grp);

                    for (k = 0; k < gcnt; k++)
                    {
                        xBase *gbase = xGroupGetItemPtr(grp, k);

                        if (gbase && (gbase->baseFlags & XBASE_ISENT))
                        {
                            xEnt *gent = (xEnt *)gbase;

                            if (gent->isCulled)
                            {
                                if (gent->isCulled == 1)
                                {
                                    numDriven--;
                                }

                                if (gent->isCulled != 1)
                                {
                                    numPrimeMovers--;
                                }

                                gent->isCulled = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    xGroup *driveGroupList = NULL;

    if (numDriven)
    {
        unsigned int allocsize =
            numDriven * sizeof(xGroup) +
            numDriven * sizeof(xGroupAsset) +
            (numDriven + numPrimeMovers) * sizeof(xBase *);

        driveGroupList = (xGroup *)RwMalloc(allocsize, rwMEMHINTDUR_FUNCTION);

        memset(driveGroupList, 0, allocsize);

        xGroupAsset *grpAssetList = (xGroupAsset *)(driveGroupList + numDriven);
        xBase **grpBaseList = (xBase **)(grpAssetList + numDriven);

        xBase **r7 = grpBaseList;

        for (i = 0; i < numDriven; i++)
        {
            driveGroupList[i].baseType = eBaseTypeGroup;
            driveGroupList[i].asset = &grpAssetList[i];
            driveGroupList[i].flg_group |= 0x1;
        }

        xGroup *r8 = driveGroupList;

        for (i = 0; i < s->num_ents; i++)
        {
            if (s->ents[i]->baseFlags & XBASE_ISENT)
            {
                xEnt *ent = s->ents[i];

                if (ent->isCulled == 1)
                {
                    xGroupAsset *gasset = r8->asset;

                    r8->item = r7;

                    *r7 = ent;
                    r7++;

                    gasset->itemCount++;

                    for (j = 0; j < s->num_ents; j++)
                    {
                        if ((s->ents[j]->baseFlags & XBASE_ISENT))
                        {
                            xEnt *other = s->ents[j];

                            if (other->isCulled == 2)
                            {
                                while (other->driver)
                                {
                                    other = other->driver;
                                }

                                if (ent == other)
                                {
                                    *r7 = other;
                                    r7++;

                                    gasset->itemCount++;
                                }
                            }
                        }
                    }

                    if (gasset->itemCount > 1)
                    {
                        numGroups++;
                    }

                    r8++;
                }
            }
        }
    }

    xGroup **tempGrpList = NULL;

    if (numGroups)
    {
        tempGrpList = (xGroup **)RwMalloc(
            numGroups * sizeof(xGroup *), rwMEMHINTDUR_FUNCTION);

        xGroup **tempGrpCurr = tempGrpList;

        for (i = 0; i < s->num_base; i++)
        {
            if (s->base[i]->baseType == eBaseTypeGroup)
            {
                xGroup *grp = (xGroup *)s->base[i];

                for (j = 0; j < grp->linkCount; j++)
                {
                    // should be grp->link[j] ...?
                    if (grp->link[0].srcEvent == eEventSceneBegin &&
                        grp->link[0].dstEvent == eEventGroupUpdateTogether)
                    {
                        *tempGrpCurr = grp;
                        tempGrpCurr++;
                    }
                }
            }
        }

        for (i = 0; i < numDriven; i++)
        {
            if (driveGroupList[i].asset->itemCount > 1)
            {
                *tempGrpCurr = &driveGroupList[i];
                tempGrpCurr++;
            }
        }
    }

    globals.updateMgr =
        xUpdateCull_Init((void **)entList, entCount, tempGrpList, numGroups);

    //globals.updateMgr->activateCB = (xUpdateCullMgrCallBack)ActivateCB;
    //globals.updateMgr->deactivateCB = (xUpdateCullMgrCallBack)DeactivateCB;

    FloatAndVoid defaultDist;
    defaultDist.f = 4900.0f;

    FloatAndVoid lodDist;
    lodDist.f = 0.0f;

    for (i = 0; i < entCount; i++)
    {
        zLODTable *lod = zLOD_Get(entList[i]);

        if (lod)
        {
            if (0.0f == lod->noRenderDist)
            {
                lod = NULL;
            }
            else
            {
                lodDist.f = xsqr(10.0f + xsqrt(lod->noRenderDist));
            }
        }

        xUpdateCull_SetCB(globals.updateMgr, entList[i], xUpdateCull_DistanceSquaredCB,
                          (lod) ? lodDist.v : defaultDist.v);
    }

    if (tempGrpList)
    {
        RwFree(tempGrpList);
    }

    if (driveGroupList)
    {
        RwFree(driveGroupList);
    }

    for (i = 0; i < s->num_ents; i++)
    {
        if (s->ents[i]->baseFlags & XBASE_ISENT)
        {
            s->ents[i]->isCulled = 0;
        }
    }

    zNPCMgr_scenePostSetup();
    z_disco_floor::post_setup();
    zEntPickup_RewardPostSetup();

    iColor_tag black = { 0, 0, 0, 255 };
    iColor_tag clear = { 0, 0, 0, 0 };

    xScrFxFade(&black, &clear, 1.0f, NULL, 0);
}

int zSceneSetup_serialTraverseCB(unsigned int clientID, xSerial *xser)
{
    BFBBSTUB("zSceneSetup_serialTraverseCB");
    return 1;
}

static void zSceneObjHashtableInit(int size)
{
    scobj_idbps = (IDBasePair *)xMemAlloc(size * sizeof(IDBasePair), 0);

    memset(scobj_idbps, 0, size * sizeof(IDBasePair));

    scobj_size = size;
    nidbps = 0;
}

static void zSceneObjHashtableExit()
{
    scobj_idbps = NULL;
    scobj_size = -1;
    nidbps = -1;
}

static int zSceneObjHashtableUsage()
{
    return nidbps;
}

static void zSceneObjHashtableAdd(unsigned int id, xBase *base)
{
    int k;
    int chkd;
    IDBasePair *idbp;

    chkd = id & (scobj_size - 1);

    for (k = 0; k < scobj_size; k++)
    {
        idbp = &scobj_idbps[chkd];

        if (idbp->id == 0)
        {
            idbp->id = id;
            idbp->base = base;

            nidbps++;
            return;
        }

        chkd++;

        if (chkd == scobj_size)
        {
            chkd = 0;
        }
    }
}

static xBase *zSceneObjHashtableGet(unsigned int id)
{
    int k;
    int chkd;
    IDBasePair *idbp;

    chkd = id & (scobj_size - 1);

    for (k = 0; k < scobj_size; k++)
    {
        idbp = &scobj_idbps[chkd];

        if (idbp->id == id)
        {
            return idbp->base;
        }

        if (idbp->id == 0)
        {
            return NULL;
        }

        chkd++;

        if (chkd == scobj_size)
        {
            chkd = 0;
        }
    }
}

xBase *zSceneFindObject(unsigned int gameID)
{
    BFBBSTUB("zSceneFindObject");
    return NULL;
}

const char *zSceneGetName(unsigned int gameID)
{
    BFBBSTUB("zSceneGetName");
    return NULL;
}

const char *zSceneGetName(xBase *b)
{
    BFBBSTUB("zSceneGetName");
    return NULL;
}

void zSceneForAllBase(zSceneForAllBaseCallBack func, void *data)
{
    BFBBSTUB("zSceneForAllBase");
}

void zSceneForAllBase(zSceneForAllBaseCallBack func, int baseType, void *data)
{
    BFBBSTUB("zSceneForAllBase");
}

static xBase *zSceneExitSoundIteratorCB(xBase *b, zScene *s, void *data)
{
    BFBBSTUB("zSceneExitSoundIteratorCB");
    return b;
}

void zSceneMemLvlChkCB()
{
    return;
}

void zSceneEnableVisited(zScene *s)
{
    BFBBSTUB("zSceneEnableVisited");
}