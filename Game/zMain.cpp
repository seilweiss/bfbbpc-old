#include "zMain.h"

#include "iSystem.h"
#include "iTime.h"
#include "zVar.h"
#include "zAssetTypes.h"
#include "xutil.h"
#include "xstransvc.h"
#include "xFont.h"
#include "zMenu.h"
#include "xTRC.h"
#include "xIni.h"
#include "iFile.h"
#include "zGame.h"
#include "zUI.h"
#include "xScrFx.h"
#include "xMath.h"
#include "zCamera.h"
#include "iMath.h"
#include "xserializer.h"
#include "zEntPlayerBungeeState.h"
#include "zEntPlayerOOBState.h"
#include "zTalkBox.h"
#include "zDispatcher.h"
#include "xFX.h"
#include "xParMgr.h"
#include "zParCmd.h"
#include "zEntPickup.h"
#include "zCameraTweak.h"
#include "xsavegame.h"
#include "xDebug.h"
#include "zNPCMgr.h"
#include "zPickupTable.h"
#include "zCutsceneMgr.h"
#include "zGameState.h"
#include "iTRC.h"
#include "zPad.h"
#include "iWindow.h"

#include <windows.h>
#include <stdio.h>

zGlobals globals;
xGlobals *xglobals = &globals;

static int sShowMenuOnBoot = 1;

static st_SERIAL_PERCID_SIZE g_xser_sizeinfo[] =
{
    'PLYR', 0x148,
    'CNTR', 0x1E0,
    0
};

float gSkipTimeCutscene = 1.0f;
float gSkipTimeFlythrough = 1.0f;

static void zMainOutputMgrSetup();
static void zMainMemLvlChkCB();
static void zMainInitGlobals();
static void zMainLoadFontHIP();
static void zMainReadINI();
static void zMainLoop();
static int ParseFloatList(float *dest, const char *strbuf, int max);
static void zMainParseINIGlobals(xIniFile *ini);

int main(int argc, char **argv)
{
    unsigned int options;

    memset(&globals, 0, sizeof(globals));
    globals.firstStartPressed = true;

    options = 0;
    iSystemInit(options);

    zMainOutputMgrSetup();
    xMemRegisterBaseNotifyFunc(zMainMemLvlChkCB);
    zMainInitGlobals();
    var_init();
    zAssetStartup();
    zMainLoadFontHIP();
    xfont::init();

    zMainFirstScreen(1);

    zMainShowProgressBar();

    xTRCInit();
    zMainReadINI();
    iFuncProfileParse("scooby.elf", globals.profile);
    xUtilStartup();
    xSerialStartup(128, g_xser_sizeinfo);
    zDispatcher_Startup();
    xScrFxInit();
    xFXStartup();

    zMainShowProgressBar();

    xParMgrInit();
    zParCmdInit();
    iEnvStartup();
    zEntPickup_Startup();
    zCameraTweakGlobal_Init();

    globals.option_vibration = 1;
    globals.pad0 = xPadEnable(globals.currentActivePad);
    globals.pad1 = NULL;

    gDebugPad = NULL;

    xPadRumbleEnable(globals.currentActivePad, globals.option_vibration);

    xSGStartup();
    xDebugTimestampScreen();
    zShrapnel_GameInit();

    zMainShowProgressBar();

    xBehaveMgr_Startup();
    zNPCMgr_Startup();

    zMainLoop();

    zNPCMgr_Shutdown();
    xBehaveMgr_Shutdown();
    zAssetShutdown();
    xFXShutdown();
    zDispatcher_Shutdown();
    xSGShutdown();
    xSerialShutdown();
    xUtilShutdown();

    iSystemExit();
    
    //Sleep(3000);

    return 0;
}

static void zMainOutputMgrSetup()
{
    iTimeDiffSec(iTimeGet());
    iTimeGet();
}

static void zMainInitGlobals()
{
    memset(&globals, 0, sizeof(globals));
    globals.sceneFirst = 1;

    iTimeDiffSec(iTimeGet());
    iTimeGet();
}

static int ParseFloatList(float *dest, const char *strbuf, int max)
{
    return xStrParseFloatList(dest, strbuf, max);
}

static void zLedgeAdjust(zLedgeGrabParams *params)
{
    params->animGrab *= 2 * ONE_FRAME;
}

static void zMainParseINIGlobals(xIniFile *ini)
{
    globals.player.g.AnalogMin = xIniGetInt(ini, "g.AnalogMin", ZPAD_ANALOG_MIN);
    globals.player.g.AnalogMax = xIniGetInt(ini, "g.AnalogMax", ZPAD_ANALOG_MAX);

    xScrFxLetterBoxSetSize(xIniGetFloat(ini, "ScrFxLetterBoxSize", 0.0f));
    xScrFxLetterBoxSetAlpha(xIniGetInt(ini, "ScrFxLetterBoxAlpha", 255));

    globals.player.g.InitialShinyCount = xIniGetInt(ini, "g.InitialShinyCount", 0);
    globals.player.g.InitialSpatulaCount = xIniGetInt(ini, "g.InitialSpatulaCount", 0);

    globals.player.g.ShinyValuePurple = xIniGetInt(ini, "g.ShinyValuePurple", 100);
    globals.player.g.ShinyValueBlue = xIniGetInt(ini, "g.ShinyValueBlue", 25);
    globals.player.g.ShinyValueGreen = xIniGetInt(ini, "g.ShinyValueGreen", 10);
    globals.player.g.ShinyValueYellow = xIniGetInt(ini, "g.ShinyValueYellow", 5);
    globals.player.g.ShinyValueRed = xIniGetInt(ini, "g.ShinyValueRed", 1);

    globals.player.g.ShinyValueCombo0 = xIniGetInt(ini, "g.ShinyValueCombo0", 0);
    globals.player.g.ShinyValueCombo1 = xIniGetInt(ini, "g.ShinyValueCombo1", 0);
    globals.player.g.ShinyValueCombo2 = xIniGetInt(ini, "g.ShinyValueCombo2", 2);
    globals.player.g.ShinyValueCombo3 = xIniGetInt(ini, "g.ShinyValueCombo3", 3);
    globals.player.g.ShinyValueCombo4 = xIniGetInt(ini, "g.ShinyValueCombo4", 3);
    globals.player.g.ShinyValueCombo5 = xIniGetInt(ini, "g.ShinyValueCombo5", 5);
    globals.player.g.ShinyValueCombo6 = xIniGetInt(ini, "g.ShinyValueCombo6", 10);
    globals.player.g.ShinyValueCombo7 = xIniGetInt(ini, "g.ShinyValueCombo7", 15);
    globals.player.g.ShinyValueCombo8 = xIniGetInt(ini, "g.ShinyValueCombo8", 20);
    globals.player.g.ShinyValueCombo9 = xIniGetInt(ini, "g.ShinyValueCombo9", 25);
    globals.player.g.ShinyValueCombo10 = xIniGetInt(ini, "g.ShinyValueCombo10", 30);
    globals.player.g.ShinyValueCombo11 = xIniGetInt(ini, "g.ShinyValueCombo11", 40);
    globals.player.g.ShinyValueCombo12 = xIniGetInt(ini, "g.ShinyValueCombo12", 50);
    globals.player.g.ShinyValueCombo13 = xIniGetInt(ini, "g.ShinyValueCombo13", 60);
    globals.player.g.ShinyValueCombo14 = xIniGetInt(ini, "g.ShinyValueCombo14", 75);
    globals.player.g.ShinyValueCombo15 = xIniGetInt(ini, "g.ShinyValueCombo15", 100);

    globals.player.g.ComboTimer = xIniGetFloat(ini, "g.ComboTimer", 1.0f);

    globals.player.g.SundaeTime = xIniGetFloat(ini, "g.SundaeTime", 6.0f);
    globals.player.g.SundaeMult = xIniGetFloat(ini, "g.SundaeMult", 1.5f);

    globals.player.g.BBashTime = xIniGetFloat(ini, "g.BBashTime", 0.3f);
    globals.player.g.BBashDelay = xIniGetFloat(ini, "g.BBashDelay", 0.15f);
    globals.player.g.BBashCVTime = xIniGetFloat(ini, "g.BBashCVTime", 0.15f);
    globals.player.g.BBashHeight = xIniGetFloat(ini, "g.BBashHeight", 3.0f);

    globals.player.g.BBounceSpeed = xIniGetFloat(ini, "g.BBounceSpeed", 10.0f);

    globals.player.g.BSpinMinFrame = xIniGetFloat(ini, "g.BSpinMinFrame", 2.0f);
    globals.player.g.BSpinMaxFrame = xIniGetFloat(ini, "g.BSpinMaxFrame", 20.0f);
    globals.player.g.BSpinRadius = xIniGetFloat(ini, "g.BSpinRadius", 0.3f);

    globals.player.g.BSpinMinFrame *= 2 * ONE_FRAME;
    globals.player.g.BSpinMaxFrame *= 2 * ONE_FRAME;

    globals.player.g.SandyMeleeMinFrame = xIniGetFloat(ini, "g.SandyMeleeMinFrame", 1.0f);
    globals.player.g.SandyMeleeMaxFrame = xIniGetFloat(ini, "g.SandyMeleeMaxFrame", 9.0f);
    globals.player.g.SandyMeleeRadius = xIniGetFloat(ini, "g.SandyMeleeRadius", 0.3f);

    globals.player.g.SandyMeleeMinFrame *= 2 * ONE_FRAME;
    globals.player.g.SandyMeleeMaxFrame *= 2 * ONE_FRAME;

    globals.player.g.DamageTimeHit = xIniGetFloat(ini, "g.DamageTimeHit", 0.5f);
    globals.player.g.DamageTimeSurface = xIniGetFloat(ini, "g.DamageTimeSurface", 1.0f);
    globals.player.g.DamageTimeEGen = xIniGetFloat(ini, "g.DamageTimeEGen", 1.0f);
    globals.player.g.DamageSurfKnock = xIniGetFloat(ini, "g.DamageSurfKnock", 1.75f);
    globals.player.g.DamageGiveHealthKnock = xIniGetFloat(ini, "g.DamageGiveHealthKnock", 1.75f);

    globals.player.g.BubbleBowlTimeDelay = xIniGetFloat(ini, "g.BubbleBowlTimeDelay", 1.0f);
    globals.player.g.BubbleBowlLaunchPosLeft = xIniGetFloat(ini, "g.BubbleBowlLaunchPosLeft", 0.0f);
    globals.player.g.BubbleBowlLaunchPosUp = xIniGetFloat(ini, "g.BubbleBowlLaunchPosUp", 1.0f);
    globals.player.g.BubbleBowlLaunchPosAt = xIniGetFloat(ini, "g.BubbleBowlLaunchPosAt", 1.5f);
    globals.player.g.BubbleBowlLaunchVelLeft = xIniGetFloat(ini, "g.BubbleBowlLaunchVelLeft", 0.0f);
    globals.player.g.BubbleBowlLaunchVelUp = xIniGetFloat(ini, "g.BubbleBowlLaunchVelUp", 0.0f);
    globals.player.g.BubbleBowlLaunchVelAt = xIniGetFloat(ini, "g.BubbleBowlLaunchVelAt", 10.0f);
    globals.player.g.BubbleBowlPercentIncrease = xIniGetFloat(ini, "g.BubbleBowlPercentIncrease", 0.85f);
    globals.player.g.BubbleBowlMinSpeed = xIniGetFloat(ini, "g.BubbleBowlMinSpeed", 0.1f);
    globals.player.g.BubbleBowlMinRecoverTime = xIniGetFloat(ini, "g.BubbleBowlMinRecoverTime", 0.15f);

    globals.player.g.SlideAccelVelMin = xIniGetFloat(ini, "g.SlideAccelVelMin", 6.0f);
    globals.player.g.SlideAccelVelMax = xIniGetFloat(ini, "g.SlideAccelVelMax", 9.0f);
    globals.player.g.SlideAccelStart = xIniGetFloat(ini, "g.SlideAccelStart", 9.0f);
    globals.player.g.SlideAccelEnd = xIniGetFloat(ini, "g.SlideAccelEnd", 4.5f);
    globals.player.g.SlideAccelPlayerFwd = xIniGetFloat(ini, "g.SlideAccelPlayerFwd", 5.0f);
    globals.player.g.SlideAccelPlayerBack = xIniGetFloat(ini, "g.SlideAccelPlayerBack", 5.0f);
    globals.player.g.SlideAccelPlayerSide = xIniGetFloat(ini, "g.SlideAccelPlayerSide", 15.0f);
    globals.player.g.SlideVelMaxStart = xIniGetFloat(ini, "g.SlideVelMaxStart", 12.0f);
    globals.player.g.SlideVelMaxEnd = xIniGetFloat(ini, "g.SlideVelMaxEnd", 17.0f);
    globals.player.g.SlideVelMaxIncTime = xIniGetFloat(ini, "g.SlideVelMaxIncTime", 6.0f);
    globals.player.g.SlideVelMaxIncAccel = xIniGetFloat(ini, "g.SlideVelMaxIncAccel", 1.0f);
    globals.player.g.SlideAirHoldTime = xIniGetFloat(ini, "g.SlideAirHoldTime", 1.0f);
    globals.player.g.SlideAirSlowTime = xIniGetFloat(ini, "g.SlideAirSlowTime", 1.5f);
    globals.player.g.SlideAirDblHoldTime = xIniGetFloat(ini, "g.SlideAirDblHoldTime", 1.0f);
    globals.player.g.SlideAirDblSlowTime = xIniGetFloat(ini, "g.SlideAirDblSlowTime", 1.5f);
    globals.player.g.SlideVelDblBoost = xIniGetFloat(ini, "g.SlideVelDblBoost", 6.0f);

    globals.player.g.TakeDamage = xIniGetInt(ini, "g.TakeDamage", 1);
    globals.player.g.CheatSpongeball = xIniGetInt(ini, "g.CheatSpongeball", 0);
    globals.player.g.CheatPlayerSwitch = xIniGetInt(ini, "g.CheatPlayerSwitch", 0);
    globals.player.g.CheatAlwaysPortal = xIniGetInt(ini, "g.CheatAlwaysPortal", 0);
    globals.player.g.CheatFlyToggle = xIniGetInt(ini, "g.CheatFlyToggle", 0);
    globals.player.g.DisableForceConversation = xIniGetInt(ini, "g.DisableForceConversation", 0);

    // consider removing these lines :)
    globals.player.g.CheatFlyToggle = 0;
    globals.player.g.CheatAlwaysPortal = 0;

    globals.player.g.StartSlideAngle = xIniGetFloat(ini, "g.StartSlideAngle", 30.0f);
    globals.player.g.StopSlideAngle = xIniGetFloat(ini, "g.StopSlideAngle", 10.0f);
    globals.player.g.RotMatchMaxAngle = xIniGetFloat(ini, "g.RotMatchMaxAngle", 30.0f);
    globals.player.g.RotMatchMatchTime = xIniGetFloat(ini, "g.RotMatchMatchTime", 0.1f);
    globals.player.g.RotMatchRelaxTime = xIniGetFloat(ini, "g.RotMatchRelaxTime", 0.3f);
    globals.player.g.Gravity = xIniGetFloat(ini, "g.Gravity", 30.0f);

    globals.player.g.StartSlideAngle = DEG2RAD(globals.player.g.StartSlideAngle);
    globals.player.g.StopSlideAngle = DEG2RAD(globals.player.g.StopSlideAngle);
    globals.player.g.RotMatchMaxAngle = DEG2RAD(globals.player.g.RotMatchMaxAngle);

    unsigned int use_degrees = xIniGetFloat(ini, "zcam_highbounce_pitch", 180.0f) != 180.0f;

    xcam_collis_radius = xIniGetFloat(ini, "xcam_collis_radius", xcam_collis_radius);
    xcam_collis_stiffness = xIniGetFloat(ini, "xcam_collis_stiffness", xcam_collis_stiffness);

    zcam_pad_pyaw_scale = xIniGetFloat(ini, "zcam_pad_pyaw_scale", zcam_pad_pyaw_scale);
    zcam_pad_pitch_scale = xIniGetFloat(ini, "zcam_pad_pitch_scale", zcam_pad_pitch_scale);

    zcam_near_d = xIniGetFloat(ini, "zcam_near_d", zcam_near_d);
    zcam_near_h = xIniGetFloat(ini, "zcam_near_h", zcam_near_h);
    zcam_near_pitch = xIniGetFloat(ini, "zcam_near_pitch",
                                   use_degrees ? RAD2DEG(zcam_near_pitch) : zcam_near_pitch);
    
    zcam_far_d = xIniGetFloat(ini, "zcam_far_d", zcam_far_d);
    zcam_far_h = xIniGetFloat(ini, "zcam_far_h", zcam_far_h);
    zcam_far_pitch = xIniGetFloat(ini, "zcam_far_pitch",
                                  use_degrees ? RAD2DEG(zcam_far_pitch) : zcam_far_pitch);
    
    zcam_above_d = xIniGetFloat(ini, "zcam_above_d", zcam_above_d);
    zcam_above_h = xIniGetFloat(ini, "zcam_above_h", zcam_above_h);
    zcam_above_pitch = xIniGetFloat(ini, "zcam_above_pitch",
                                    use_degrees ? RAD2DEG(zcam_above_pitch) : zcam_above_pitch);
    
    zcam_below_d = xIniGetFloat(ini, "zcam_below_d", zcam_below_d);
    zcam_below_h = xIniGetFloat(ini, "zcam_below_h", zcam_below_h);
    zcam_below_pitch = xIniGetFloat(ini, "zcam_below_pitch",
                                    use_degrees ? RAD2DEG(zcam_below_pitch) : zcam_below_pitch);
    
    zcam_highbounce_d = xIniGetFloat(ini, "zcam_highbounce_d", zcam_highbounce_d);
    zcam_highbounce_h = xIniGetFloat(ini, "zcam_highbounce_h", zcam_highbounce_h);
    zcam_highbounce_pitch = xIniGetFloat(ini, "zcam_highbounce_pitch",
                                         use_degrees ? RAD2DEG(zcam_highbounce_pitch) : zcam_highbounce_pitch);
    
    zcam_wall_d = xIniGetFloat(ini, "zcam_wall_d", zcam_wall_d);
    zcam_wall_h = xIniGetFloat(ini, "zcam_wall_h", zcam_wall_h);
    zcam_wall_pitch = xIniGetFloat(ini, "zcam_wall_pitch",
                                   use_degrees ? RAD2DEG(zcam_wall_pitch) : zcam_wall_pitch);

    zcam_overrot_min = xIniGetFloat(ini, "zcam_overrot_min", zcam_overrot_min);
    zcam_overrot_mid = xIniGetFloat(ini, "zcam_overrot_mid", zcam_overrot_mid);
    zcam_overrot_max = xIniGetFloat(ini, "zcam_overrot_max", zcam_overrot_max);
    zcam_overrot_rate = xIniGetFloat(ini, "zcam_overrot_rate", zcam_overrot_rate);
    zcam_overrot_tstart = xIniGetFloat(ini, "zcam_overrot_tstart", zcam_overrot_tstart);
    zcam_overrot_tend = xIniGetFloat(ini, "zcam_overrot_tend", zcam_overrot_tend);
    zcam_overrot_velmin = xIniGetFloat(ini, "zcam_overrot_velmin", zcam_overrot_velmin);
    zcam_overrot_velmax = xIniGetFloat(ini, "zcam_overrot_velmax", zcam_overrot_velmax);
    zcam_overrot_tmanual = xIniGetFloat(ini, "zcam_overrot_tmanual", zcam_overrot_tmanual);

    if (use_degrees)
    {
        zcam_near_pitch = DEG2RAD(zcam_near_pitch);
        zcam_far_pitch = DEG2RAD(zcam_far_pitch);
        zcam_above_pitch = DEG2RAD(zcam_above_pitch);
        zcam_below_pitch = DEG2RAD(zcam_below_pitch);
        zcam_highbounce_pitch = DEG2RAD(zcam_highbounce_pitch);
        zcam_wall_pitch = DEG2RAD(zcam_wall_pitch);
    }

    zcam_overrot_min = DEG2RAD(zcam_overrot_min);
    zcam_overrot_mid = DEG2RAD(zcam_overrot_mid);
    zcam_overrot_max = DEG2RAD(zcam_overrot_max);

    gSkipTimeCutscene = xIniGetFloat(ini, "gSkipTimeCutscene", gSkipTimeCutscene);
    gSkipTimeFlythrough = xIniGetFloat(ini, "gSkipTimeFlythrough", gSkipTimeFlythrough);

    gSkipTimeCutscene = xmax(gSkipTimeCutscene, 1.0f);
    gSkipTimeFlythrough = xmax(gSkipTimeFlythrough, 1.0f);

    globals.player.carry.minDist = xIniGetFloat(ini, "carry.minDist", 0.675f);
    globals.player.carry.maxDist = xIniGetFloat(ini, "carry.maxDist", 1.9f);
    globals.player.carry.minHeight = xIniGetFloat(ini, "carry.minHeight", -0.2f);
    globals.player.carry.maxHeight = xIniGetFloat(ini, "carry.maxHeight", 0.4f);
    globals.player.carry.maxCosAngle = xIniGetFloat(ini, "carry.maxCosAngle", 45.0f);
    globals.player.carry.throwMinDist = xIniGetFloat(ini, "carry.throwMinDist", 1.5f);
    globals.player.carry.throwMaxDist = xIniGetFloat(ini, "carry.throwMaxDist", 12.0f);
    globals.player.carry.throwMinHeight = xIniGetFloat(ini, "carry.throwMinHeight", -3.0f);
    globals.player.carry.throwMaxHeight = xIniGetFloat(ini, "carry.throwMaxHeight", 5.0f);
    globals.player.carry.throwMaxStack = xIniGetFloat(ini, "carry.throwMaxStack", 2.75f);
    globals.player.carry.throwMaxCosAngle = xIniGetFloat(ini, "carry.throwMaxCosAngle", 25.0f);
    globals.player.carry.grabLerpMin = xIniGetFloat(ini, "carry.grabLerpMin", 0.0f);
    globals.player.carry.grabLerpMax = xIniGetFloat(ini, "carry.grabLerpMax", 0.2f);
    globals.player.carry.throwGravity = xIniGetFloat(ini, "carry.throwGravity", 30.0f);
    globals.player.carry.throwHeight = xIniGetFloat(ini, "carry.throwHeight", 3.75f);
    globals.player.carry.throwDistance = xIniGetFloat(ini, "carry.throwDistance", 10.0f);
    globals.player.carry.fruitFloorDecayMin = xIniGetFloat(ini, "carry.fruitFloorDecayMin", 0.3f);
    globals.player.carry.fruitFloorDecayMax = xIniGetFloat(ini, "carry.fruitFloorDecayMax", 6.0f);
    globals.player.carry.fruitFloorBounce = xIniGetFloat(ini, "carry.fruitFloorBounce", 0.15f);
    globals.player.carry.fruitFloorFriction = xIniGetFloat(ini, "carry.fruitFloorFriction", 0.4f);
    globals.player.carry.fruitCeilingBounce = xIniGetFloat(ini, "carry.fruitCeilingBounce", 0.1f);
    globals.player.carry.fruitWallBounce = xIniGetFloat(ini, "carry.fruitWallBounce", 0.5f);
    globals.player.carry.fruitLifetime = xIniGetFloat(ini, "carry.fruitLifetime", 15.0f);

    globals.player.carry.maxCosAngle = icos(DEG2RAD(globals.player.carry.maxCosAngle));
    globals.player.carry.throwMaxCosAngle = icos(DEG2RAD(globals.player.carry.throwMaxCosAngle));

    globals.player.g.PowerUp[0] = xIniGetInt(ini, "g.BubbleBowl", 0);
    globals.player.g.PowerUp[1] = xIniGetInt(ini, "g.CruiseBubble", 0);

    memcpy(globals.player.g.InitialPowerUp, globals.player.g.PowerUp, 2);

    {
        static float fbuf[] = { 0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f };
        ParseFloatList(
            (float *)memcpy(globals.player.sb.MoveSpeed, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.MoveSpeed", NULL),
            6);
    }
    
    {
        static float fbuf[] = { 1.5f, 0.5f, 1.5f };
        ParseFloatList(
            (float *)memcpy(globals.player.sb.AnimSneak, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.AnimSneak", NULL),
            3);
    }

    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(globals.player.sb.AnimWalk, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.AnimWalk", NULL),
            3);
    }
    
    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(globals.player.sb.AnimRun, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.AnimRun", NULL),
            3);
    }

    globals.player.sb.JumpGravity = xIniGetFloat(ini, "sb.JumpGravity", 7.0f);
    globals.player.sb.GravSmooth = xIniGetFloat(ini, "sb.GravSmooth", 0.25f);
    globals.player.sb.FloatSpeed = xIniGetFloat(ini, "sb.FloatSpeed", 1.0f);
    globals.player.sb.ButtsmashSpeed = xIniGetFloat(ini, "sb.ButtsmashSpeed", 5.0f);

    globals.player.sb.ledge.animGrab = xIniGetFloat(ini, "sb.ledge.animGrab", 3.0f);
    zLedgeAdjust(&globals.player.sb.ledge);

    bungee_state::load_settings(*ini);
    oob_state::load_settings(*ini);
    ztalkbox::load_settings(*ini);

    {
        static float fbuf[] = { 2.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sb.Jump, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.Jump", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sb.Double, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.Double", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.5f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sb.Bounce, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.Bounce", NULL),
            4);
    }

    {
        static float fbuf[] = { 3.0f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sb.Spring, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.Spring", NULL),
            4);
    }

    {
        static float fbuf[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sb.Wall, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sb.Wall", NULL),
            4);
    }

    globals.player.sb.WallJumpVelocity = xIniGetFloat(ini, "sb.WallJumpVelocity", 0.0f);
    globals.player.sb.spin_damp_xz = xIniGetFloat(ini, "sb.spin_damp_xz", 15.0f);
    globals.player.sb.spin_damp_y = xIniGetFloat(ini, "sb.spin_damp_y", 15.0f);

    globals.player.s = &globals.player.sb;

    CalcJumpImpulse(&globals.player.sb.Jump, NULL);
    CalcJumpImpulse(&globals.player.sb.Double, NULL);
    CalcJumpImpulse(&globals.player.sb.Bounce, NULL);
    CalcJumpImpulse(&globals.player.sb.Spring, NULL);
    CalcJumpImpulse(&globals.player.sb.Wall, NULL);

    globals.player.sb_model_indices[0] = xIniGetInt(ini, "SB.model_index.body", 0);
    globals.player.sb_model_indices[1] = xIniGetInt(ini, "SB.model_index.arm_l", 1);
    globals.player.sb_model_indices[2] = xIniGetInt(ini, "SB.model_index.arm_r", 2);
    globals.player.sb_model_indices[3] = xIniGetInt(ini, "SB.model_index.ass", 3);
    globals.player.sb_model_indices[4] = xIniGetInt(ini, "SB.model_index.underwear", 4);
    globals.player.sb_model_indices[5] = xIniGetInt(ini, "SB.model_index.wand", 5);
    globals.player.sb_model_indices[6] = xIniGetInt(ini, "SB.model_index.tongue", 6);
    globals.player.sb_model_indices[7] = xIniGetInt(ini, "SB.model_index.bubble_helmet", 7);
    globals.player.sb_model_indices[8] = xIniGetInt(ini, "SB.model_index.bubble_shoe_l", 8);
    globals.player.sb_model_indices[9] = xIniGetInt(ini, "SB.model_index.bubble_shoe_r", 9);
    globals.player.sb_model_indices[13] = xIniGetInt(ini, "SB.model_index.shadow_wand", 10);
    globals.player.sb_model_indices[11] = xIniGetInt(ini, "SB.model_index.shadow_arm_l", 11);
    globals.player.sb_model_indices[12] = xIniGetInt(ini, "SB.model_index.shadow_arm_r", 12);
    globals.player.sb_model_indices[10] = xIniGetInt(ini, "SB.model_index.shadow_body", 13);

    {
        static float fbuf[] = { 0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.MoveSpeed, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.MoveSpeed", NULL),
            6);
    }

    {
        static float fbuf[] = { 1.5f, 0.5f, 1.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.AnimSneak, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.AnimSneak", NULL),
            3);
    }

    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.AnimWalk, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.AnimWalk", NULL),
            3);
    }

    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.AnimRun, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.AnimRun", NULL),
            3);
    }

    globals.player.patrick.JumpGravity = xIniGetFloat(ini, "patrick.JumpGravity", 7.0f);
    globals.player.patrick.GravSmooth = xIniGetFloat(ini, "patrick.GravSmooth", 0.25f);
    globals.player.patrick.FloatSpeed = xIniGetFloat(ini, "patrick.FloatSpeed", 1.0f);
    globals.player.patrick.ButtsmashSpeed = xIniGetFloat(ini, "patrick.ButtsmashSpeed", 5.0f);

    globals.player.patrick.ledge.animGrab = xIniGetFloat(ini, "patrick.ledge.animGrab", 3.0f);
    zLedgeAdjust(&globals.player.patrick.ledge);

    {
        static float fbuf[] = { 2.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.Jump, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.Jump", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.Double, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.Double", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.5f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.Bounce, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.Bounce", NULL),
            4);
    }

    {
        static float fbuf[] = { 3.0f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.Spring, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.Spring", NULL),
            4);
    }

    {
        static float fbuf[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.patrick.Wall, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "patrick.Wall", NULL),
            4);
    }

    // should be globals.player.patrick...?
    globals.player.s = &globals.player.sb;

    CalcJumpImpulse(&globals.player.patrick.Jump, NULL);
    CalcJumpImpulse(&globals.player.patrick.Double, NULL);
    CalcJumpImpulse(&globals.player.patrick.Bounce, NULL);
    CalcJumpImpulse(&globals.player.patrick.Spring, NULL);
    CalcJumpImpulse(&globals.player.patrick.Wall, NULL);

    globals.player.patrick.WallJumpVelocity = xIniGetFloat(ini, "patrick.WallJumpVelocity", 0.0f);

    {
        static float fbuf[] = { 0.6f, 3.0f, 7.0f, 0.1f, 0.5f, 1.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.MoveSpeed, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.MoveSpeed", NULL),
            6);
    }

    {
        static float fbuf[] = { 1.5f, 0.5f, 1.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.AnimSneak, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.AnimSneak", NULL),
            3);
    }

    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.AnimWalk, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.AnimWalk", NULL),
            3);
    }

    {
        static float fbuf[] = { 3.0f, 0.5f, 2.5f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.AnimRun, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.AnimRun", NULL),
            3);
    }

    globals.player.sandy.JumpGravity = xIniGetFloat(ini, "sandy.JumpGravity", 7.0f);
    globals.player.sandy.GravSmooth = xIniGetFloat(ini, "sandy.GravSmooth", 0.25f);
    globals.player.sandy.FloatSpeed = xIniGetFloat(ini, "sandy.FloatSpeed", 1.0f);
    globals.player.sandy.ButtsmashSpeed = xIniGetFloat(ini, "sandy.ButtsmashSpeed", 5.0f);

    globals.player.sandy.ledge.animGrab = xIniGetFloat(ini, "sandy.ledge.animGrab", 3.0f);
    zLedgeAdjust(&globals.player.sandy.ledge);

    {
        static float fbuf[] = { 2.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.Jump, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.Jump", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.0f, 0.7f, 0.35f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.Double, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.Double", NULL),
            4);
    }

    {
        static float fbuf[] = { 1.5f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.Bounce, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.Bounce", NULL),
            4);
    }

    {
        static float fbuf[] = { 3.0f, 0.2f, 0.2f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.Spring, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.Spring", NULL),
            4);
    }

    {
        static float fbuf[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ParseFloatList(
            (float *)memcpy(&globals.player.sandy.Wall, fbuf, sizeof(fbuf)),
            xIniGetString(ini, "sandy.Wall", NULL),
            4);
    }

    // should be globals.player.sandy...?
    globals.player.s = &globals.player.sb;

    CalcJumpImpulse(&globals.player.sandy.Jump, NULL);
    CalcJumpImpulse(&globals.player.sandy.Double, NULL);
    CalcJumpImpulse(&globals.player.sandy.Bounce, NULL);
    CalcJumpImpulse(&globals.player.sandy.Spring, NULL);
    CalcJumpImpulse(&globals.player.sandy.Wall, NULL);

    globals.player.sandy.WallJumpVelocity = xIniGetFloat(ini, "sandy.WallJumpVelocity", 0.0f);

    sShowMenuOnBoot = xIniGetInt(ini, "ShowMenuOnBoot", 1);
    gGameSfxReport = xIniGetInt(ini, "SFXReport", 0);

    globals.player.s = &globals.player.sb;

    globals.player.sb.pcType = ePlayer_SB;
    globals.player.patrick.pcType = ePlayer_Patrick;
    globals.player.sandy.pcType = ePlayer_Sandy;
}

static void zMainMemLvlChkCB()
{
    zSceneMemLvlChkCB();
}

static int percentageDone = 0;

void zMainShowProgressBar()
{
    if (zMenuIsFirstBoot())
    {
        char loading[11];
        char whiteText[64];
        char yellowText[64];
        char buf[64];

        if (percentageDone > 100)
        {
            percentageDone = 100;
        }

        int yellowEnd = percentageDone / 10;

        strcpy(loading, "Loading...");

        memset(buf, 0, sizeof(buf));
        memset(whiteText, 0, sizeof(whiteText));

        strcpy(yellowText, loading);
        yellowText[yellowEnd] = '\0';

        memcpy(whiteText, loading + yellowEnd, strlen(loading) - yellowEnd);

        sprintf(buf, "{font=0}{h*2}{w*2}%s{color=FFFFFFFF}%s{~:c}",
                yellowText, whiteText);

        zMainMemCardRenderText(buf, true);

        percentageDone += 10;
    }
}

static void zMainLoop()
{
    iTime t;

    zMainShowProgressBar();

    xMemPushBase();

    t = iTimeGet();
    xUtil_idtag2string('BOOT', 0);
    iTimeDiffSec(t);

    int cltver = xSTPreLoadScene('BOOT', NULL, XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('BOOT', 0);
    iTimeDiffSec(t);

    xSTQueueSceneAssets('BOOT', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('BOOT', 0);
    iTimeDiffSec(t);

    while (xSTLoadStep('BOOT') < 1.0f) { }

    xSTDisconnect('BOOT', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('BOOT', 0);
    iTimeDiffSec(t);

    zMainShowProgressBar();

    xSTPreLoadScene('PLAT', NULL, XST_SCENE_HIP);
    xSTQueueSceneAssets('PLAT', XST_SCENE_HIP);

    while (xSTLoadStep('PLAT') < 1.0f) { }

    xSTDisconnect('PLAT', XST_SCENE_HIP);

    zMainShowProgressBar();

    iTimeGet();
    xShadowSimple_Init();

    globals.pickupTable = (zAssetPickupTable *)xSTFindAssetByType('PICK', 0, NULL);
    zPickupTableInit();

    xMemPushBase();

    t = iTimeGet();
    xUtil_idtag2string('MNU4', 0);
    iTimeDiffSec(t);

    xSTPreLoadScene('MNU4', NULL, XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('MNU4', 0);
    iTimeDiffSec(t);

    xSTQueueSceneAssets('MNU4', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('MNU4', 0);
    iTimeDiffSec(t);

    while (xSTLoadStep('MNU4') < 1.0f) { }

    xSTDisconnect('MNU4', XST_SCENE_HIP);

    zMainShowProgressBar();

    t = iTimeGet();
    xUtil_idtag2string('MNU4', 0);
    iTimeDiffSec(t);

    xMemPushBase();
    
    t = iTimeGet();
    xUtil_idtag2string('MNU5', 0);
    iTimeDiffSec(t);

    xSTPreLoadScene('MNU5', NULL, XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('MNU5', 0);
    iTimeDiffSec(t);

    xSTQueueSceneAssets('MNU5', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('MNU5', 0);
    iTimeDiffSec(t);

    while (xSTLoadStep('MNU5') < 1.0f) { }

    xSTDisconnect('MNU5', XST_SCENE_HIP);

    zMainShowProgressBar();

    t = iTimeGet();
    xUtil_idtag2string('MNU5', 0);
    iTimeDiffSec(t);

    xModelInit();
    xModelPoolInit(32, 64);
    xModelPoolInit(40, 8);
    xModelPoolInit(56, 1);

    static unsigned int preinit_bubble_matfx[] =
    {
        0xDBD033BC, /* bubble_shoeL.dff */
        0x452279A2, /* bubble_shoeR.dff */
        0xC17F4BCC, /* bubble_bowlingball */
        0x0CF9267A, /* bubble_helmet.dff */
        0x5C009D14, /* cruise_bubble_bind.dff */
        0
    };

    unsigned int *preinit = preinit_bubble_matfx;

    while (*preinit)
    {
        RpAtomic *modl = (RpAtomic *)xSTFindAsset(*preinit, NULL);

        if (modl)
        {
            xFXPreAllocMatFX(RpAtomicGetClump(modl));
        }

        preinit++;
    }

    static unsigned int menuModeID = 'MNU3';

    static unsigned int gameSceneID =
        (globals.sceneStart[0] << 24) |
        (globals.sceneStart[1] << 16) |
        (globals.sceneStart[2] << 8) |
        globals.sceneStart[3];

    unsigned int newGameSceneID = gameSceneID;

    xUtil_idtag2string(gameSceneID, 0);
    xMemPushBase();

    zMainShowProgressBar();

    zMainMemCardSpaceQuery();

    while (true)
    {
        zMainShowProgressBar();

        xSerialWipeMainBuffer();
        zCutSceneNamesTable_clearAll();

        zMainShowProgressBar();

        if (sShowMenuOnBoot)
        {
            zMenuInit(menuModeID);

            zMainShowProgressBar();
            zMainShowProgressBar();

            zMenuSetup();

            xFX_SceneEnter(globals.sceneCur->env->geom->world);

            newGameSceneID = zMenuLoop();

            zMenuExit();
        }
        else
        {
            sShowMenuOnBoot = 1;
            globals.firstStartPressed = true;

            zGameModeSwitch(eGameMode_Game);
            zGameStateSwitch(eGameState_FirstTime);
        }

        if (!newGameSceneID)
        {
            zVarNewGame();
            iTimeSetGame(0.0f);

            gameSceneID =
                (globals.sceneStart[0] << 24) |
                (globals.sceneStart[1] << 16) |
                (globals.sceneStart[2] << 8) |
                globals.sceneStart[3];
        }
        else
        {
            gameSceneID = newGameSceneID;
            
            iTimeSetGame(0.0f);
        }

        zGameInit(gameSceneID);
        zGameSetup();

        iProfileClear(gameSceneID);

        zGameLoop();
        zGameExit();
    }
}

static void zMainReadINI()
{
    const char *str;
    void *buf;
    unsigned int size;
    xIniFile *ini;

    buf = iFileLoad("SB.INI", NULL, &size);

    if (buf)
    {
        ini = xIniParse((char *)buf, size);

        str = xIniGetString(ini, "PATH", NULL);

        if (str)
        {
            iFileSetPath(str);
        }

        str = xIniGetString(ini, "BOOT", NULL);

        if (str)
        {
            strcpy(globals.sceneStart, "BOOT");

            if (strlen(str) == 4)
            {
                strcpy(globals.sceneStart, str);

                if (xStricmp(str, "soak") == 0)
                {
                    gSoak = 1;
                    strcpy(globals.sceneStart, "mnu3");
                }
            }
        }

        globals.profile = xIniGetInt(ini, "Profile", 0);

        strncpy(globals.profFunc[0], xIniGetString(ini, "ProfFuncTriangle", ""), 128);
        globals.profFunc[0][127] = '\0';

        strncpy(globals.profFunc[1], xIniGetString(ini, "ProfFuncSquare", ""), 128);
        globals.profFunc[1][127] = '\0';

        strncpy(globals.profFunc[2], xIniGetString(ini, "ProfFuncLeft", ""), 128);
        globals.profFunc[2][127] = '\0';

        strncpy(globals.profFunc[3], xIniGetString(ini, "ProfFuncRight", ""), 128);
        globals.profFunc[3][127] = '\0';

        strncpy(globals.profFunc[4], xIniGetString(ini, "ProfFuncUp", ""), 128);
        globals.profFunc[4][127] = '\0';

        strncpy(globals.profFunc[5], xIniGetString(ini, "ProfFuncDown", ""), 128);
        globals.profFunc[5][127] = '\0';

        globals.useHIPHOP = xIniGetInt(ini, "EnableHipHopLoading", 0);
        globals.NoMusic = (xIniGetInt(ini, "NoMusic", 0) != 0);

        zMainParseINIGlobals(ini);
        zUI_ParseINI(ini);

        xIniDestroy(ini);
    }

    iTimeDiffSec(iTimeGet());
    iTimeGet();

    RwFree(buf);
}

void zMainFirstScreen(int mode)
{
    RwCamera *cam = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    RwRGBA bg = { 0, 0, 0, 0 };

    for (int i = 0; i < 2; i++)
    {
        RwCameraClear(cam, &bg, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

        RwCameraBeginUpdate(cam);

        if (mode != 0)
        {
            xtextbox tb = xtextbox::create(
                    xfont::create(1, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f,
                    { 255, 230, 0, 200 }, screen_bounds),
                screen_bounds, 0x2, 0.0f, 0.0f, 0.0f, 0.0f);

            /*
            tb.set_text(
                "Game and Software © 2003 THQ Inc. © 2003 Viacom International Inc. "
                "All rights reserved.\n\n"
                "Nickelodeon, SpongeBob SquarePants and all related titles, logos, and "
                "characters are trademarks of Viacom International Inc. Created by "
                "Stephen Hillenburg.\n\n"
                "Exclusively published by THQ Inc. Developed by Heavy Iron. Portions of "
                "this software are Copyright 1998 - 2003 Criterion Software Ltd. and "
                "its Licensors. THQ, Heavy Iron and the THQ logo are trademarks and/or "
                "registered trademarks of THQ Inc. All rights reserved.\n\n"
                "All other trademarks, logos and copyrights are property of their "
                "respective owners.");
                */
            
            tb.set_text(
                "This is an unofficial PC port released in 2020. Developed by the "
                "BFBB Modding community.\n\n"
                "Original Game and Software © 2003 THQ Inc. © 2003 Viacom International Inc. "
                "All rights reserved.\n\n"
                "Nickelodeon, SpongeBob SquarePants and all related titles, logos, and "
                "characters are trademarks of Viacom International Inc. Created by "
                "Stephen Hillenburg.\n\n"
                "Exclusively published by THQ Inc. Developed by Heavy Iron. Portions of "
                "this software are Copyright 1998 - 2003 Criterion Software Ltd. and "
                "its Licensors. THQ, Heavy Iron and the THQ logo are trademarks and/or "
                "registered trademarks of THQ Inc. All rights reserved.\n\n"
                "All other trademarks, logos and copyrights are property of their "
                "respective owners.");

            tb.bounds = screen_bounds;
            tb.bounds.contract(0.1f);
            tb.bounds.h = tb.yextent(true);
            tb.bounds.y = -(0.5f * tb.bounds.h - 0.5f);

            tb.render(true);
        }

        RwCameraEndUpdate(cam);

        RwCameraShowRaster(cam, NULL, FULLSCREEN ? rwRASTERFLIPWAITVSYNC : 0);
    }

    // 3 seconds
    for (int vbl = 180; vbl != 0; vbl--)
    {
        iTRCDisk::CheckDVDAndResetState();
        iWindowUpdate();
        iVSync();
    }

    //Sleep(3000);

    iCameraDestroy(cam);
}

void zMainMemCardSpaceQuery()
{
    BFBBSTUB("zMainMemCardSpaceQuery");
}

void zMainMemCardRenderText(const char *text, bool enabled)
{
    RwRGBA bg = { 0, 0, 0, 0 };
    RwCamera *cam = iCameraCreate(SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    RwCameraClear(cam, &bg, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

    RwCameraBeginUpdate(cam);

    RenderText(text, enabled);

    RwCameraEndUpdate(cam);

    RwCameraShowRaster(cam, NULL, FULLSCREEN ? rwRASTERFLIPWAITVSYNC : 0);

    iCameraDestroy(cam);
}

static void zMainLoadFontHIP()
{
    iTime t;

    xMemPushBase();

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    xSTPreLoadScene('FONT', NULL, XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    xSTQueueSceneAssets('FONT', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);

    while (xSTLoadStep('FONT') < 1.0f) { }

    xSTDisconnect('FONT', XST_SCENE_HIP);

    t = iTimeGet();
    xUtil_idtag2string('FONT', 0);
    iTimeDiffSec(t);
}