#ifndef ZENTPLAYER_H
#define ZENTPLAYER_H

#include "zEnt.h"
#include "zLasso.h"

enum _zPlayerType
{
    ePlayer_SB,
    ePlayer_Patrick,
    ePlayer_Sandy,
    ePlayer_MAXTYPES
};

enum _zPlayerWallJumpState
{
    k_WALLJUMP_NOT,
    k_WALLJUMP_LAUNCH,
    k_WALLJUMP_FLIGHT,
    k_WALLJUMP_LAND
};

struct zCheckPoint
{
    xVec3 pos;
    float rot;
    unsigned int initCamID;
};

struct zJumpParam
{
    float PeakHeight;
    float TimeGravChange;
    float TimeHold;
    float ImpulseVel;
};

struct zLedgeGrabParams
{
    float animGrab;
    float zdist;
    xVec3 tranTable[60];
    int tranCount;
    xEnt *optr;
    xMat4x3 omat;
    float y0det;
    float dydet;
    float r0det;
    float drdet;
    float thdet;
    float rtime;
    float ttime;
    float tmr;
    xVec3 spos;
    xVec3 epos;
    xVec3 tpos;
    int nrays;
    int rrand;
    float startrot;
    float endrot;
};

struct zPlayerSettings
{
    _zPlayerType pcType;
    float MoveSpeed[6];
    float AnimSneak[3];
    float AnimWalk[3];
    float AnimRun[3];
    float JumpGravity;
    float GravSmooth;
    float FloatSpeed;
    float ButtsmashSpeed;
    zJumpParam Jump;
    zJumpParam Bounce;
    zJumpParam Spring;
    zJumpParam Wall;
    zJumpParam Double;
    zJumpParam SlideDouble;
    zJumpParam SlideJump;
    float WallJumpVelocity;
    zLedgeGrabParams ledge;
    float spin_damp_xz;
    float spin_damp_y;
    unsigned char talk_anims;
    unsigned char talk_filter_size;
    unsigned char talk_filter[4];
};

struct zPlayerCarryInfo
{
    xEnt *grabbed;
    unsigned int grabbedModelID;
    xMat4x3 spin;
    xEnt *throwTarget;
    xEnt *flyingToTarget;
    float minDist;
    float maxDist;
    float minHeight;
    float maxHeight;
    float maxCosAngle;
    float throwMinDist;
    float throwMaxDist;
    float throwMinHeight;
    float throwMaxHeight;
    float throwMaxStack;
    float throwMaxCosAngle;
    float throwTargetRotRate;
    float targetRot;
    unsigned int grabTarget;
    xVec3 grabOffset;
    float grabLerpMin;
    float grabLerpMax;
    float grabLerpLast;
    unsigned int grabYclear;
    float throwGravity;
    float throwHeight;
    float throwDistance;
    float fruitFloorDecayMin;
    float fruitFloorDecayMax;
    float fruitFloorBounce;
    float fruitFloorFriction;
    float fruitCeilingBounce;
    float fruitWallBounce;
    float fruitLifetime;
    xEnt *patLauncher;
};

struct zPlayerLassoInfo
{
    xEnt *target;
    float dist;
    unsigned char destroy;
    unsigned char targetGuide;
    float lassoRot;
    xEnt *swingTarget;
    xEnt *releasedSwing;
    float copterTime;
    int canCopter;
    zLasso lasso;
    xAnimState *zeroAnim;
};

enum zControlOwner
{
    CONTROL_OWNER_GLOBAL = 0x1,
    CONTROL_OWNER_EVENT = 0x2,
    CONTROL_OWNER_OOB = 0x4,
    CONTROL_OWNER_BOSS = 0x8,
    CONTROL_OWNER_TALK_BOX = 0x10,
    CONTROL_OWNER_TAXI = 0x20,
    CONTROL_OWNER_BUS_STOP = 0x40,
    CONTROL_OWNER_TELEPORT_BOX = 0x80,
    CONTROL_OWNER_CRUISE_BUBBLE = 0x100,
    CONTROL_OWNER_FLY_CAM = 0x200,
    CONTROL_OWNER_FROZEN = 0x400,
    CONTROL_OWNER_TURRET = 0x800,
    CONTROL_OWNER_REWARDANIM = 0x1000,
    CONTROL_OWNER_BUNGEE = 0x2000,
    CONTROL_OWNER_SPRINGBOARD = 0x4000,
    CONTROL_OWNER_CUTSCENE = 0x8000
};

enum _CurrentPlayer
{
    eCurrentPlayerSpongeBob,
    eCurrentPlayerPatrick,
    eCurrentPlayerSandy,
    eCurrentPlayerCount
};

struct xEntBoulder;

extern xEntBoulder *boulderVehicle;
extern _CurrentPlayer lastgCurrentPlayer;

void zEntPlayerControlOn(zControlOwner owner);
void CalcJumpImpulse(zJumpParam *param, const zPlayerSettings *settings);
void zEntPlayer_Update(xEnt *ent, xScene *sc, float dt);
void zEntPlayer_Render(zEnt *ent);
int zEntPlayerEventCB(xBase *from, xBase *to, unsigned int toEvent, const float *toParam,
                      xBase *toParamWidget);
void zEntPlayerPreReset();
void zEntPlayerReset(xEnt *ent);
void zEntPlayer_RestoreSounds();
void zEntPlayer_Init(xEnt *ent, xEntAsset *asset);
void zEntPlayer_ShadowModelEnable();
void zEntPlayer_ShadowModelDisable();
void zEntPlayerExit(xEnt *ent);
void zEntPlayer_StoreCheckPoint(xVec3 *pos, float rot, unsigned int initCamID);
void zEntPlayer_LoadSounds();
void zEntPlayer_UnloadSounds();

#endif