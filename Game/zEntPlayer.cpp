#include "zEntPlayer.h"

#include "zMain.h"
#include "xstransvc.h"
#include "iModel.h"
#include "xString.h"
#include "zEntCruiseBubble.h"
#include "zBase.h"
#include "xFFX.h"
#include "zGust.h"
#include "zEntPlayerBungeeState.h"
#include "zEntPlayerOOBState.h"
#include "xEntBoulder.h"
#include "xSnd.h"
#include "zGameState.h"
#include "zCamera.h"
#include "xScrFx.h"

#include "print.h"

static float sHackStuckTimer;

static float stuck_timer;
static float not_stuck_timer;

static int sPlayerDiedLastTime;

static float minVelmag = 0.01f;
static float maxVelmag = 10.0f;
static float curVelmag;
static float curVelangle;
static int surfSlickness = 1;
static float surfFriction = 1.0f;
static float surfDamping;
static int lastSlickness = 1;
static unsigned int surfSticky;
static float surfSlideStart = DEG2RAD(20.0f);
float surfSlickRatio;
float surfSlickTimer;
static float sPlayerCollAdjust;
static zPlayerLassoInfo *sLassoInfo;
static zLasso *sLasso;
static xEnt *sHitch[32];
static int sNumHitches;
static float sHitchAngle;

static int sGooKnockedToSafety;
static float sGooKnockedTimer;

xEntBoulder *boulderVehicle;
static int boulderRollShouldEnd;
static int boulderRollShouldStart;

static zParEmitter *sEmitSpinBubbles;
static zParEmitter *sEmitSundae;
static zParEmitter *sEmitStankBreath;

static xModelTag sStankTag[3];

static RpAtomic *sReticleModel;
static xMat4x3 sReticleMat;
static float sTimeToRetarget;
xEnt *gReticleTarget;
static RwRaster *sBowlingLaneRast;

struct
{
    unsigned int level;
    unsigned int total;
} static patsock_totals[] =
{
    'HB\0\0', 8,
    'JF\0\0', 14,
    'BB\0\0', 9,
    'GL\0\0', 11,
    'BC\0\0', 4,
    'RB\0\0', 9,
    'SM\0\0', 10,
    'KF\0\0', 7,
    'GY\0\0', 3,
    'DB\0\0', 5,
    0
};

zGustData gust_data;

xMat4x3 gPlayerAbsMat;
xMat4x3 sCameraLastMat;

_CurrentPlayer lastgCurrentPlayer = eCurrentPlayerCount;

static float bbash_vel;
static float tslide_maxspd;
static float tslide_maxspd_tmr;
static float tslide_inair_tmr;
static unsigned int tslide_ground;
static xVec3 tslide_lastrealvel;
static int in_goo;
static float in_goo_tmr;
int player_hit;
static unsigned int mount_type;
static xEnt *mount_object;
static float mount_tmr;

static void zEntPlayer_Move(xEnt *ent, xScene *, float dt, xEntFrame *frame);

static void zEntPlayerKillCarry()
{
    BFBBSTUB("zEntPlayerKillCarry");
}

void zEntPlayerControlOn(zControlOwner owner)
{
    unsigned int originalValue = globals.player.ControlOff;

    globals.player.ControlOff &= ~owner;

    if (originalValue != globals.player.ControlOff)
    {
        if (globals.player.ControlOff & (CONTROL_OWNER_EVENT | CONTROL_OWNER_FLY_CAM))
        {
            xSndSelectListenerMode(SND_LISTENER_MODE_CAMERA);
        }
        else
        {
            xSndSelectListenerMode(SND_LISTENER_MODE_PLAYER);
        }
    }
}

void zEntPlayerControlOff(zControlOwner owner)
{
    unsigned int originalValue = globals.player.ControlOff;

    globals.player.ControlOff |= owner;
    globals.player.ControlOffTimer = 1.0f;

    if (originalValue != globals.player.ControlOff)
    {
        if (globals.player.ControlOff & (CONTROL_OWNER_EVENT | CONTROL_OWNER_FLY_CAM))
        {
            xSndSelectListenerMode(SND_LISTENER_MODE_CAMERA);
        }
        else
        {
            xSndSelectListenerMode(SND_LISTENER_MODE_PLAYER);
        }
    }

    zEntPlayerKillCarry();
}

static void HealthReset();
static void InvReset();

static void InvReset()
{
    globals.player.MaxHealth = 3;

    HealthReset();

    globals.player.Inv_Shiny = globals.player.g.InitialShinyCount;
    globals.player.Inv_Spatula = globals.player.g.InitialSpatulaCount;
    globals.player.Inv_PatsSock_Total = 0;

    if (globals.player.Inv_Shiny > 99999)
    {
        globals.player.Inv_Shiny = 99999;
    }

    for (unsigned int i = 0; i < LEVEL_COUNT; i++)
    {
        unsigned int &maxsocks = globals.player.Inv_PatsSock_Max[i];

        globals.player.Inv_PatsSock[i] = 0;
        globals.player.Inv_LevelPickups[i] = 0;
        globals.player.Inv_PatsSock_Max[i] = 0;

        const char *level_prefix = zSceneGetLevelPrefix(i);

        if (level_prefix)
        {
            unsigned int level_mask = (level_prefix[0] << 24) | (level_prefix[1] << 16);

            for (unsigned int j = 0;
                 j < (sizeof(patsock_totals) / sizeof(*patsock_totals));
                 j++)
            {
                if (level_mask == patsock_totals[j].level)
                {
                    maxsocks = patsock_totals[j].total;
                }
            }
        }
    }

    memcpy(&globals.player.g.PowerUp, &globals.player.g.InitialPowerUp,
           sizeof(globals.player.g.PowerUp));
}

static void HealthReset()
{
    globals.player.Health = globals.player.MaxHealth;
}

void zEntPlayerSpeakStop()
{
    BFBBSTUB("zEntPlayerSpeakStop");
}

unsigned int sandyHitMax;
unsigned int patrickHitMax;
xAnimTransition sandyHitTran[8];
xAnimTransition patrickHitTran[8];

zParEmitter *gEmitBFX;

static xModelTag sSandyLFoot;
static xModelTag sSandyRFoot;
static xModelTag sSandyLHand;
static xModelTag sSandyRHand;
static xModelTag sSandyLKnee;
static xModelTag sSandyRKnee;
static xModelTag sSandyLElbow;
static xModelTag sSandyRElbow;

static xModelTag sSpongeBobLKnee;
static xModelTag sSpongeBobRKnee;
static xModelTag sSpongeBobLElbow;
static xModelTag sSpongeBobRElbow;
static xModelTag sSpongeBobLFoot;
static xModelTag sSpongeBobRFoot;
static xModelTag sSpongeBobLHand;
static xModelTag sSpongeBobRHand;

static xModelTag sPatrickLFoot;
static xModelTag sPatrickRFoot;
static xModelTag sPatrickLHand;
static xModelTag sPatrickRHand;
static xModelTag sPatrickLKnee;
static xModelTag sPatrickRKnee;
static xModelTag sPatrickLElbow;
static xModelTag sPatrickRElbow;
static xModelTag sPatrickMelee;

float sRingDelay;

static void load_player_ini(zPlayerSettings &settings, xModelInstance &model,
                            xModelAssetParam *params, unsigned int params_size)
{
    BFBBSTUB("load_player_ini");
}

static void load_player_ini()
{
    if (globals.player.model_spongebob)
    {
        unsigned int params_size;
        xModelAssetParam *params;
        
        params = zEntGetModelParams(globals.player.ent.asset->modelInfoID, &params_size);

        load_player_ini(globals.player.sb, *globals.player.model_spongebob,
                        params, params_size);
    }

    if (globals.player.model_patrick)
    {
        unsigned int params_size;
        xModelAssetParam *params;

        params = zEntGetModelParams(0x791025AC, &params_size); // patrick_bind.MINF

        load_player_ini(globals.player.patrick, *globals.player.model_patrick,
                        params, params_size);
    }

    if (globals.player.model_sandy)
    {
        unsigned int params_size;
        xModelAssetParam *params;

        params = zEntGetModelParams(0xC0E34B23, &params_size); // sandy_bind.MINF

        load_player_ini(globals.player.sandy, *globals.player.model_sandy,
                        params, params_size);
    }
}

static void PlayerHackFixBbashMiss(xModelInstance *model);
static void PlayerRotMatchUpdateEnt(xEnt *ent, xScene *, float dt, void *fdata);
static int BoulderVEventCB(xBase *from, xBase *to, unsigned int toEvent,
                           const float *toParam, xBase *toParamWidget);
static void zEntPlayer_SNDInit();

void zEntPlayer_Init(xEnt *ent, xEntAsset *asset)
{
    BFBBSTUB("zEntPlayer_Init");

    zEntInit((zEnt *)ent, asset, 'PLYR');
    xEntInitShadow(*ent, globals.player.entShadow_embedded);

    ent->simpShadow = &globals.player.simpShadow_embedded;

    xShadowSimple_CacheInit(ent->simpShadow, ent, 80);

    unsigned int lightKitID =
        *(unsigned int *)((xLinkAsset *)(asset + 1) + ent->linkCount);

    if (lightKitID)
    {
        ent->lightKit = (xLightKit *)xSTFindAsset(lightKitID, NULL);

        if (ent->lightKit && ent->lightKit->tagID != 'TIKL')
        {
            ent->lightKit = NULL;
        }
    }

    globals.player.model_spongebob = ent->model;

    memset(&globals.player.sb_models, 0, sizeof(globals.player.sb_models));

    unsigned char index = 0;
    xModelInstance *m = globals.player.model_spongebob;

    while (m)
    {
        for (int i = 0; i < SB_MODEL_COUNT; i++)
        {
            if (globals.player.sb_model_indices[i] == index)
            {
                globals.player.sb_models[i] = m;
            }
        }

        m = m->Next;
        index++;
    }

    PlayerHackFixBbashMiss(globals.player.model_spongebob);

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Parent = globals.player.model_spongebob;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Parent = globals.player.model_spongebob;

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Flags = XMODEL_UNK2000;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Flags = XMODEL_UNK2000;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Flags = XMODEL_UNK2000;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Flags = XMODEL_UNK2000;

    globals.player.sb_models[SB_MODEL_SHADOW_BODY]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_L]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_ARM_R]->Mat = globals.player.model_spongebob->Mat;
    globals.player.sb_models[SB_MODEL_SHADOW_WAND]->Mat = globals.player.model_spongebob->Mat;

    globals.player.model_wand = globals.player.sb_models[SB_MODEL_WAND];

    iModelTagSetup(&globals.player.BubbleWandTag[0], globals.player.model_wand->Data,
                   -0.604f, 0.46f, 0.63f);
    iModelTagSetup(&globals.player.BubbleWandTag[1], globals.player.model_wand->Data,
                   -0.563f, 0.427f, 0.294f);
    iModelTagSetup(&sSpongeBobLFoot, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_L]->Data,
                   0.119f, 0.043f, -0.032f);
    iModelTagSetup(&sSpongeBobRFoot, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_R]->Data,
                   -0.119f, 0.043f, -0.032f);
    iModelTagSetup(&sSpongeBobLKnee, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_L]->Data,
                   0.119f, 0.161f, 0.024f);
    iModelTagSetup(&sSpongeBobRKnee, globals.player.sb_models[SB_MODEL_BUBBLE_SHOE_R]->Data,
                   -0.118f, 0.161f, 0.024f);
    iModelTagSetup(&sSpongeBobLHand, globals.player.sb_models[SB_MODEL_ARM_L]->Data,
                   0.55f, 0.496f, 0.02f);
    iModelTagSetup(&sSpongeBobRHand, globals.player.sb_models[SB_MODEL_ARM_R]->Data,
                   -0.55f, 0.496f, -0.011f);
    iModelTagSetup(&sSpongeBobRElbow, globals.player.sb_models[SB_MODEL_ARM_R]->Data,
                   -0.442f, 0.458f, -0.007f);
    iModelTagSetup(&sSpongeBobLElbow, globals.player.sb_models[SB_MODEL_ARM_L]->Data,
                   0.43f, 0.458f, -0.004f);
    iModelTagSetup(&sStankTag[0], globals.player.ent.model->Data,
                   -0.014f, 0.546f, 0.168f);

    sEmitSpinBubbles = zParEmitterFind("PAREMIT_GRAB_BUBBLES");
    sEmitSundae = zParEmitterFind("PAREMIT_CLOUD");
    sEmitStankBreath = zParEmitterFind("PAREMIT_STANK");

    gEmitBFX = zParEmitterFind("PAREMIT_BFX");

    sLassoInfo = &globals.player.lassoInfo;
    sLasso = &sLassoInfo->lasso;

    float bbncvtm = globals.player.g.BBashTime - globals.player.g.BBashCVTime;

    bbash_vel =
        (0.5f * globals.player.g.Gravity * xsqr(bbncvtm) + globals.player.g.BBashHeight) /
        globals.player.g.BBashTime;

    unsigned int bufsize;
    void *info;
    
    info = xSTFindAsset(0x791025AC, &bufsize); // patrick_bind.MINF

    if (info)
    {
        globals.player.model_patrick = zEntRecurseModelInfo(info, ent);

        iModelTagSetup(&sPatrickMelee, globals.player.model_patrick->Data,
                       0.0f, 0.475f, 0.252f);
        iModelTagSetup(&sPatrickLFoot, globals.player.model_patrick->Data,
                       0.187f, 0.0f, -0.068f);
        iModelTagSetup(&sPatrickRFoot, globals.player.model_patrick->Data,
                       -0.187f, 0.0f, -0.068f);
        iModelTagSetup(&sPatrickLKnee, globals.player.model_patrick->Data,
                       0.19f, 0.099f, -0.138f);
        iModelTagSetup(&sPatrickRKnee, globals.player.model_patrick->Data,
                       -0.19f, 0.099f, -0.138f);
        iModelTagSetup(&sPatrickRHand, globals.player.model_patrick->Data,
                       -0.632f, 0.711f, -0.235f);
        iModelTagSetup(&sPatrickLHand, globals.player.model_patrick->Data,
                       0.684f, 0.694f, -0.215f);
        iModelTagSetup(&sPatrickRElbow, globals.player.model_patrick->Data,
                       -0.475f, 0.733f, -0.269f);
        iModelTagSetup(&sPatrickLElbow, globals.player.model_patrick->Data,
                       0.475f, 0.733f, -0.269f);
    }
    else
    {
        globals.player.model_patrick = NULL;
    }

    info = xSTFindAsset(0xC0E34B23, &bufsize); // sandy_bind.MINF

    if (info)
    {
        globals.player.model_sandy = zEntRecurseModelInfo(info, ent);

        zLasso_Init(sLasso, globals.player.model_sandy, -0.599f, 0.645f, 0.051f);

        iModelTagSetup(&sSandyLFoot, globals.player.model_sandy->Data,
                       0.159f, 0.0f, 0.045f);
        iModelTagSetup(&sSandyRFoot, globals.player.model_sandy->Data,
                       -0.012f, 0.0f, 0.258f);
        iModelTagSetup(&sSandyLKnee, globals.player.model_sandy->Data,
                       0.129f, 0.287f, 0.089f);
        iModelTagSetup(&sSandyRKnee, globals.player.model_sandy->Data,
                       -0.071f, -0.071f, 0.089f);
        iModelTagSetup(&sSandyRHand, globals.player.model_sandy->Data,
                       -0.642f, 0.747f, 0.006f);
        iModelTagSetup(&sSandyLHand, globals.player.model_sandy->Data,
                       0.641f, 0.747f, 0.006f);
        iModelTagSetup(&sSandyRElbow, globals.player.model_sandy->Data,
                       -0.37f, 0.661f, 0.086f);
        iModelTagSetup(&sSandyLElbow, globals.player.model_sandy->Data,
                       0.37f, 0.661f, 0.086f);
    }
    else
    {
        globals.player.model_sandy = NULL;
    }

    static int drybob_anim_count;

    static float drybob_oldTime[64];
    static float *drybob_chgTime[64];
    static void *drybob_oldData[64];
    static void **drybob_chgData[64];

    for (int i = 0; i < drybob_anim_count; i++)
    {
        *drybob_chgData[i] = drybob_oldData[i];
        *drybob_chgTime[i] = drybob_oldTime[i];
    }

    drybob_anim_count = 0;

    RpAtomic *r23 =
        (RpAtomic *)xSTFindAsset(xStrHash("spongebob_bind_treedome.dff"), NULL);

    if (r23)
    {
        RpAtomic *r22 = iModelFile_RWMultiAtomic(r23);
        RpAtomic *_ = iModelFile_RWMultiAtomic(r22);
        RpAtomic *r3 = iModelFile_RWMultiAtomic(_);

        globals.player.sb_models[SB_MODEL_ARM_L]->Data = r23;
        globals.player.sb_models[SB_MODEL_ARM_R]->Data = r22;
        globals.player.sb_models[SB_MODEL_BODY]->Data = r3;

        xAnimTable *wettbl;
        xAnimTable *drytbl;

        xAnimState *wetstate;
        xAnimState *drystate;

        xAnimFile *wetfile;
        xAnimFile *dryfile;

        wettbl = (xAnimTable *)xSTFindAsset(xStrHash("spongebob_bind.ATBL"), NULL);
        drytbl = (xAnimTable *)xSTFindAsset(xStrHash("spongebob_bind_treedome.ATBL"), NULL);

        if (wettbl && drytbl)
        {
            drystate = drytbl->StateList;

            while (drystate)
            {
                if (!(drystate->UserFlags & 0x40000000))
                {
                    wetstate = xAnimTableGetState(wettbl, drystate->Name);
                    
                    wetfile = wetstate->Data;
                    dryfile = drystate->Data;

                    int aa;
                    int numa = wetfile->NumAnims[0] * wetfile->NumAnims[1];

                    for (aa = 0; aa < numa; aa++)
                    {
                        drybob_chgData[drybob_anim_count] = &wetfile->RawData[aa];
                        drybob_oldData[drybob_anim_count] = wetfile->RawData[aa];
                        drybob_chgTime[drybob_anim_count] = &wetfile->Duration;
                        drybob_oldTime[drybob_anim_count] = wetfile->Duration;

                        wetfile->RawData[aa] = dryfile->RawData[aa];

                        drybob_anim_count++;
                    }

                    wetfile->Duration = dryfile->Duration;
                }

                drystate = drystate->Next;
            }
        }
    }

    cruise_bubble::init();
    load_player_ini();

    xEntEnable(ent);
    xEntShow(ent);

    globals.player.Visible = 1;
    globals.player.AutoMoveSpeed = 0;

    ent->pflags &= ~XENT_PUNK4;
    ent->collis->chk &= ~0x1;
    ent->update = zEntPlayer_Update;
    ent->move = zEntPlayer_Move;
    ent->render = (xEntRenderCallBack)zEntPlayer_Render;
    ent->eventFunc = zEntPlayerEventCB;

    if (ent->linkCount)
    {
        ent->link = (xLinkAsset *)(asset + 1);
    }
    else
    {
        ent->link = NULL;
    }

    if (globals.sceneCur->baseCount[eBaseTypeGust])
    {
        xFFXAddEffect(ent, zGustUpdateEnt, &gust_data);
    }

    xFFXRotMatchState *rms = xFFXRotMatchAlloc();

    if (rms)
    {
        rms->max_decl = globals.player.g.RotMatchMaxAngle;
        rms->tmatch = globals.player.g.RotMatchMatchTime;
        rms->trelax = globals.player.g.RotMatchRelaxTime;
        rms->tmr = 0.0f;

        xFFXAddEffect(ent, PlayerRotMatchUpdateEnt, rms);
    }

    bungee_state::init();
    oob_state::init();

    boulderVehicle = NULL;

    boulderVehicle = (xEntBoulder *)zSceneFindObject(xStrHash("BOULDER_VEHICLE"));

    if (boulderVehicle)
    {
        boulderVehicle->eventFunc = BoulderVEventCB;
    }

    sNumHitches = 0;
    sHitchAngle = 0.0f;

    unsigned int trailerHash = xStrHash("trailer_hitch");

    for (unsigned int i = 0; i < globals.sceneCur->num_ents; i++)
    {
        xEnt *hitch = globals.sceneCur->ents[i];

        if (hitch->baseType == eBaseTypeDestructObj ||
            hitch->baseType == eBaseTypePlatform ||
            hitch->baseType == eBaseTypeStatic)
        {
            if (hitch->asset->modelInfoID == trailerHash)
            {
                sHitch[sNumHitches++] = hitch;
            }
        }
    }

    lastgCurrentPlayer = eCurrentPlayerCount;

    zEntPlayerPreReset();
    //zEntPlayerReset(ent);
    zEntPlayer_SNDInit();

    sPlayerDiedLastTime = 0;

    zEntPlayer_RestoreSounds();
}

void zEntPlayer_RestoreSounds()
{
    BFBBSTUB("zEntPlayer_RestoreSounds");
}

static int BoulderVEventCB(xBase *from, xBase *to, unsigned int toEvent,
                           const float *toParam, xBase *toParamWidget)
{
    BFBBSTUB("BoulderVEventCB");
    return 1;
}

void zEntPlayer_Update(xEnt *ent, xScene *sc, float dt)
{
    BFBBSTUB("zEntPlayer_Update");
}

void zEntPlayer_ShadowModelEnable()
{
    BFBBSTUB("zEntPlayer_ShadowModelEnable");
}

void zEntPlayer_ShadowModelDisable()
{
    BFBBSTUB("zEntPlayer_ShadowModelDisable");
}

void zEntPlayer_Render(zEnt *ent)
{
    BFBBSTUB("zEntPlayer_Render");
}

static void zEntPlayer_Move(xEnt *ent, xScene *, float dt, xEntFrame *frame)
{
    BFBBSTUB("zEntPlayer_Move");
}

void zEntPlayer_setBoulderMode(unsigned int mode)
{
    if (mode)
    {
        boulderRollShouldStart = 1;
        boulderRollShouldEnd = 0;
    }
    else
    {
        boulderRollShouldStart = 0;
        boulderRollShouldEnd = 1;
    }
}

void CalcJumpImpulse(zJumpParam *param, const zPlayerSettings *settings)
{
    BFBBSTUB("CalcJumpImpulse");
}

static unsigned int PlayerDepenQuery(xEnt *, xEnt *, xScene *, float, xCollis *coll)
{
    BFBBSTUB("PlayerDepenQuery");
    return 1;
}

static void PlayerBoundUpdate(xEnt *ent, xVec3 *pos)
{
    BFBBSTUB("PlayerBoundUpdate");
}

void zEntPlayerExit(xEnt *ent)
{
    BFBBSTUB("zEntPlayerExit");
}

static void PlayerHitAnimInit(xModelInstance *r3, xAnimTransition *r4, unsigned int *r5)
{
    BFBBSTUB("PlayerHitAnimInit");
}

static void PlayerLedgeInit(zLedgeGrabParams *ledge, xModelInstance *model);

int zEntPlayer_ObjIDIsTrack(unsigned int id)
{
    BFBBSTUB("zEntPlayer_ObjIDIsTrack");
    return 0;
}

void zEntPlayerPreReset()
{
    globals.player.ControlOff = 0;

    if (!oob_state::IsPlayerInControl())
    {
        zEntPlayerControlOff(CONTROL_OWNER_OOB);

        globals.player.ControlOffTimer = 1e+38f;
    }
}

void zEntPlayerReset(xEnt *ent)
{
    xVec3Init(&globals.player.PredictCurrDir, 0.0f, 1.0f, 0.0f);

    globals.player.PredictCurrVel = 0.0f;

    if (globals.player.bubblebowl)
    {
        xEntBoulder_Kill(globals.player.bubblebowl);
    }

    if (boulderVehicle)
    {
        boulderVehicle->flags |= XENT_UNK40;
        boulderVehicle->asset->flags |= XENT_UNK40;

        xEntBoulder_Kill(boulderVehicle);

        if (boulderVehicle->model->Anim)
        {
            xAnimPlaySetState(boulderVehicle->model->Anim->Single,
                              boulderVehicle->model->Anim->Table->StateList,
                              0.0f);
        }
    }

    ent->update = zEntPlayer_Update;
    ent->move = zEntPlayer_Move;
    ent->render = (xEntRenderCallBack)zEntPlayer_Render;

    if (zGameModeGet() != eGameMode_Game ||
           (zGameStateGet() != eGameState_SceneSwitch &&
            zGameStateGet() != eGameState_Play &&
            zGameStateGet() != eGameState_Unk04))
    {
        if (zGameModeGet() == eGameMode_Game &&
            zGameStateGet() == eGameState_LoseChance)
        {
            HealthReset();
        }
        else
        {
            InvReset();
        }
    }

    globals.player.ControlOnEvent = 0;
    globals.player.Speed = 0;
    globals.player.SpeedMult = 1.0f;
    globals.player.Sneak = 0;
    globals.player.Teeter = 0;
    globals.player.Slide = 0;
    globals.player.SlideTimer = 0.0f;
    globals.player.SlideNotGroundedSinceSlide = 0;
    globals.player.Stepping = 0;
    globals.player.JumpState = 0;
    globals.player.LastJumpState = 0;
    globals.player.IdleMinorTimer = 0.0f;
    globals.player.IdleMajorTimer = 0.0f;
    globals.player.IdleSitTimer = 0.0f;
    globals.player.cheat_mode = 0;
    globals.player.FireTarget = NULL;
    globals.player.LeanLerp = 1.0f;
    globals.player.ScareTimer = 0.0f;
    globals.player.DamageTimer = 0.0f;
    globals.player.SundaeTimer = -1.0f;
    globals.player.ControlOffTimer = 0.0f;
    globals.player.HelmetTimer = 0.0f;
    globals.player.Bounced = 0;
    globals.player.FallDeathTimer = 0.0f;
    globals.player.Diggable = NULL;
    globals.player.Visible = 1;
    globals.player.AutoMoveSpeed = 0;
    globals.player.Face_ScareTimer = 0.0f;
    globals.player.Face_EventTimer = 0.0f;
    globals.player.Face_PantTimer = 0.0f;
    globals.player.SpecialReceived = 0;
    globals.player.Jump_CanFloat = 1;
    globals.player.Jump_CanDouble = 1;
    globals.player.Jump_Springboard = NULL;
    globals.player.Jump_SpringboardStart = 0;
    globals.player.CanJump = 1;
    globals.player.CanBubbleSpin = 1;
    globals.player.CanBubbleBounce = 1;
    globals.player.CanBubbleBash = 1;
    globals.player.IsJumping = 0;
    globals.player.IsDJumping = 0;
    globals.player.WasDJumping = 0;
    globals.player.IsBubbleSpinning = 0;
    globals.player.IsBubbleBouncing = 0;
    globals.player.IsBubbleBashing = 0;
    globals.player.IsBubbleBowling = 0;
    globals.player.WallJumpState = k_WALLJUMP_NOT;
    globals.player.ForceSlipperyTimer = -1.0f;
    globals.player.carry.grabbed = NULL;
    globals.player.carry.grabTarget = 0;
    globals.player.carry.throwTarget = NULL;
    globals.player.carry.patLauncher = NULL;
    globals.player.lassoInfo.lasso.flags = 0;
    globals.player.lassoInfo.target = NULL;
    globals.player.lassoInfo.destroy = 0;

    if (globals.player.lassoInfo.swingTarget)
    {
        globals.player.lassoInfo.swingTarget = NULL;
        zCameraDisableLassoCam();
    }

    ent->collis->depenq = PlayerDepenQuery;
    ent->bupdate = PlayerBoundUpdate;

    surfSlickRatio = 0.0f;
    surfSlickTimer = 0.0f;

    globals.player.floor_surf = NULL;
    globals.player.floor_norm = g_Y3;

    ent->collis->chk = 0x2E;

    xEntReset(ent);

    xVec3Copy(&ent->frame->rot.axis, &g_Y3);

    ent->frame->rot.angle = ent->asset->ang.x;

    xEntDriveInit(&globals.player.drv, ent);

    globals.player.drv.flags = XENTDRIVE_UNK1;

    xCameraSetTargetMatrix(&globals.camera, xEntGetFrame(&globals.player.ent));
    xCameraSetTargetOMatrix(&globals.camera, &sCameraLastMat);

    zCameraReset(&globals.camera);

    xMat4x3Copy(&gPlayerAbsMat, &globals.camera.mat);

    globals.player.Jump_CurrGravity = globals.player.g.Gravity;

    xAnimState *curr = globals.player.ent.model->Anim->Table->StateList;

    while (curr)
    {
        if ((curr->UserFlags & 0x40) &&
            (curr->Data->NumAnims[0] != 3 || curr->Data->NumAnims[1] != 1))
        {
            curr->UserFlags &= ~0x40;
        }

        curr = curr->Next;
    }

    xModelInstance *minst = globals.player.ent.model;
    unsigned int index = 0;

    while (minst)
    {
        if (index >= 2)
        {
            minst->Flags &= ~(XMODEL_UNK1 | XMODEL_UNK2);
        }

        minst = minst->Next;
        index++;
    }

    globals.player.RootUp.x = 0.0f;
    globals.player.RootUp.y = 1.0f;
    globals.player.RootUp.z = 0.0f;
    globals.player.RootUpTarget.x = 0.0f;
    globals.player.RootUpTarget.y = 1.0f;
    globals.player.RootUpTarget.z = 0.0f;
    globals.player.HangElapsed = 0.0f;
    globals.player.HangEnt = NULL;
    globals.player.HangEntLast = NULL;
    globals.player.KnockBackTimer = 0.0f;
    globals.player.KnockIntoAirTimer = 0.0f;

    sGooKnockedToSafety = 0;
    sGooKnockedTimer = 0.0f;

    xEntBeginCollide(ent, globals.sceneCur, 0.0f);
    xEntSetNostepNormAngle(surfSlideStart);

    curVelmag = 0.0f;
    curVelangle = 0.0f;
    surfSlickness = 1;
    surfFriction = 1.0f;
    surfDamping = minVelmag;
    lastSlickness = 1;
    surfSticky = 0;
    sPlayerCollAdjust = 0.0f;
    tslide_maxspd_tmr = 0.0f;
    tslide_maxspd = globals.player.g.SlideVelMaxStart;
    tslide_lastrealvel.x = 0.0f;
    globals.player.SlideTrackDecay = 0.0f;
    tslide_lastrealvel.y = 0.0f;
    tslide_lastrealvel.z = 0.0f;
    tslide_inair_tmr = 0.0f;
    tslide_ground = 0;
    sRingDelay = 0.0f;
    player_hit = 0;
    globals.player.SlideTrackSliding = 0;
    globals.player.SlideTrackVel.x = 0.0f;
    globals.player.SlideTrackVel.y = 0.0f;
    globals.player.SlideTrackVel.z = 0.0f;
    mount_type = 0;
    mount_object = NULL;
    mount_tmr = 0.0f;
    sHackStuckTimer = 0.0f;

    if (globals.player.model_spongebob)
    {
        xAnimPlaySetState(globals.player.model_spongebob->Anim->Single,
                          globals.player.model_spongebob->Anim->Table->StateList,
                          0.0f);
    }

    if (globals.player.model_patrick)
    {
        xAnimPlaySetState(globals.player.model_patrick->Anim->Single,
                          globals.player.model_patrick->Anim->Table->StateList,
                          0.0f);
    }

    if (globals.player.model_sandy)
    {
        xAnimPlaySetState(globals.player.model_sandy->Anim->Single,
                          globals.player.model_sandy->Anim->Table->StateList,
                          0.0f);
    }

    xAnimPlaySetState(ent->model->Anim->Single,
                      ent->model->Anim->Table->StateList,
                      0.0f);

    if (globals.sceneCur->sceneID == 'MNU3')
    {
        globals.firstStartPressed = true;
        globals.autoSaveFeature = false;
        globals.pad0 = &mPad[0];
        globals.currentActivePad = 0;

        zEntPlayerControlOff(CONTROL_OWNER_EVENT);

        globals.player.ControlOnEvent = 0;
    }

    iColor_tag black = { 0, 0, 0, 255 };
    iColor_tag clear = { 0, 0, 0, 0 };

    xScrFxFade(&black, &clear, 1.0f, NULL, 0);

    if (globals.player.model_spongebob)
    {
        PlayerLedgeInit(&globals.player.sb.ledge, globals.player.model_spongebob);
    }

    if (globals.player.model_patrick)
    {
        PlayerLedgeInit(&globals.player.patrick.ledge, globals.player.model_patrick);
        PlayerHitAnimInit(globals.player.model_patrick, patrickHitTran, &patrickHitMax);
    }

    if (globals.player.model_sandy)
    {
        PlayerLedgeInit(&globals.player.sandy.ledge, globals.player.model_sandy);
        PlayerHitAnimInit(globals.player.model_sandy, sandyHitTran, &sandyHitMax);
    }

    unsigned int i;

    for (i = 0; i < globals.sceneCur->num_ents; i++)
    {
        if (globals.sceneCur->ents[i]->baseType == eBaseTypeStatic ||
            globals.sceneCur->ents[i]->baseType == eBaseTypePlatform)
        {
            if (zEntPlayer_ObjIDIsTrack(globals.sceneCur->ents[i]->id))
            {
                xEnt *trackEnt = globals.sceneCur->ents[i];

                globals.player.SlideTrackEnt[globals.player.SlideTrackCount++] = trackEnt;

                trackEnt->baseFlags &= ~(XBASE_CSNVISIBLE | XBASE_RECSHADOW);
                trackEnt->chkby = 0;
                trackEnt->penby = 0;
            }
        }
    }

    cruise_bubble::reset();
    bungee_state::reset();

    in_goo_tmr = 0.0f;
    in_goo = 0;

    sReticleModel = (RpAtomic *)xSTFindAsset(xStrHash("target_reticle_hand"), NULL);

    xMat4x3Identity(&sReticleMat);

    gReticleTarget = NULL;
    sTimeToRetarget = 0.0f;

    RwTexture *tex = (RwTexture *)xSTFindAsset(xStrHash("bowling_lane"), NULL);

    sBowlingLaneRast = tex ? RwTextureGetRaster(tex) : NULL;

    if (globals.sceneCur->sceneID == 'PG12')
    {
        zEntPlayer_setBoulderMode(1);
    }
    else
    {
        zEntPlayer_setBoulderMode(0);
    }

    stuck_timer = 0.0f;
    not_stuck_timer = 0.0f;

    zEntPlayerSpeakStop();
}

int zEntPlayerEventCB(xBase *from, xBase *to, unsigned int toEvent, const float *toParam,
                      xBase *toParamWidget)
{
    BFBBSTUB("zEntPlayerEventCB");
    return 1;
}

static void PlayerRotMatchUpdateEnt(xEnt *ent, xScene *, float dt, void *fdata)
{
    BFBBSTUB("PlayerRotMatchUpdateEnt");
}

void zEntPlayer_StoreCheckPoint(xVec3 *pos, float rot, unsigned int initCamID)
{
    if (pos)
    {
        globals.player.cp.pos = *pos;
        globals.player.cp.rot = rot;
        globals.player.cp.initCamID = initCamID;
    }
}

static void zEntPlayer_SNDInit()
{
    BFBBSTUB("zEntPlayer_SNDInit");
}

static void PlayerHackFixBbashMiss(xModelInstance *model)
{
    BFBBSTUB("PlayerHackFixBbashMiss");
}

static void PlayerLedgeInit(zLedgeGrabParams *ledge, xModelInstance *model)
{
    BFBBSTUB("PlayerLedgeInit");
}

void zEntPlayer_LoadSounds()
{
    BFBBSTUB("zEntPlayer_LoadSounds");
}

void zEntPlayer_UnloadSounds()
{
    BFBBSTUB("zEntPlayer_UnloadSounds");
}