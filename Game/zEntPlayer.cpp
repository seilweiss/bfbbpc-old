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

#include "print.h"

static int sPlayerDiedLastTime;

static zPlayerLassoInfo *sLassoInfo;
static zLasso *sLasso;
static xEnt *sHitch[32];
static int sNumHitches;
static float sHitchAngle;

xEntBoulder *boulderVehicle;

static zParEmitter *sEmitSpinBubbles;
static zParEmitter *sEmitSundae;
static zParEmitter *sEmitStankBreath;

static xModelTag sStankTag[3];

zGustData gust_data;

_CurrentPlayer lastgCurrentPlayer = eCurrentPlayerCount;

static float bbash_vel;

static void zEntPlayer_Move(xEnt *ent, xScene *, float dt, xEntFrame *frame);

void zEntPlayerControlOn(zControlOwner owner)
{
    BFBBSTUB("zEntPlayerControlOn");
}

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

static void PlayerHackFixBbashMiss(xModelInstance *model);
static void PlayerRotMatchUpdateEnt(xEnt *ent, xScene *, float dt, void *fdata);
static int BoulderVEventCB(xBase *from, xBase *to, unsigned int toEvent,
                           const float *toParam, xBase *toParamWidget);
static void zEntPlayer_SNDInit();

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

void zEntPlayer_Init(xEnt *ent, xEntAsset *asset)
{
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
    zEntPlayerReset(ent);
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

void CalcJumpImpulse(zJumpParam *param, const zPlayerSettings *settings)
{
    BFBBSTUB("CalcJumpImpulse");
}

void zEntPlayerExit(xEnt *ent)
{
    BFBBSTUB("zEntPlayerExit");
}

void zEntPlayerPreReset()
{
    BFBBSTUB("zEntPlayerPreReset");
}

void zEntPlayerReset(xEnt *ent)
{
    BFBBSTUB("zEntPlayerReset");
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

void zEntPlayer_LoadSounds()
{
    BFBBSTUB("zEntPlayer_LoadSounds");
}

void zEntPlayer_UnloadSounds()
{
    BFBBSTUB("zEntPlayer_UnloadSounds");
}