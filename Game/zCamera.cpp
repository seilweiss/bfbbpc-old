#include "zCamera.h"

#include "xMath.h"
#include "zCameraTweak.h"
#include "zMain.h"
#include "zCutsceneMgr.h"
#include "xScrFx.h"
#include "xMathInlines.h"
#include "iSystem.h"
#include "iMath.h"
#include "zCameraFly.h"
#include "zPad.h"

#include "print.h"

#define SKIP_BUTTONS (XPAD_BUTTON_X | XPAD_BUTTON_TRIANGLE)

float zcam_pad_pyaw_scale = 0.18124573f;
float zcam_pad_pitch_scale = 0.01923077f;
float zcam_near_d = 3.0f;
float zcam_near_h = 1.8f;
float zcam_near_pitch = DEG2RAD(10.0f);
float zcam_far_d = 5.0f;
float zcam_far_h = 3.0f;
float zcam_far_pitch = DEG2RAD(15.0f);
float zcam_wall_d = 7.5f;
float zcam_wall_h = 2.0f;
float zcam_wall_pitch = DEG2RAD(18.0f);
float zcam_above_d = 0.2f;
float zcam_above_h = 2.2f;
float zcam_above_pitch = DEG2RAD(70.0f);
float zcam_below_d = 0.6f;
float zcam_below_h = 0.2f;
float zcam_below_pitch = DEG2RAD(-70.0f);
float zcam_highbounce_d = 0.2f;
float zcam_highbounce_h = 5.0f;
float zcam_highbounce_pitch = DEG2RAD(89.0f);
float zcam_overrot_min = DEG2RAD(25.0f);
float zcam_overrot_mid = DEG2RAD(90.0f);
float zcam_overrot_max = DEG2RAD(170.0f);
float zcam_overrot_rate = 0.1f;
float zcam_overrot_tstart = 1.5f;
float zcam_overrot_tend = 2.5f;
float zcam_overrot_velmin = 3.0f;
float zcam_overrot_velmax = 5.0f;
float zcam_overrot_tmanual = 1.5f;
float zcam_overrot_tmr;
xCamera zcam_backupcam;
xCamera zcam_backupconvers;
int zcam_near;
int zcam_mode;
int zcam_bbounce;
int zcam_lbbounce;
int zcam_convers;
int zcam_lconvers;
int zcam_longbounce;
int zcam_highbounce;
int zcam_cutscene;
int zcam_reward;
xVec3 *zcam_playervel;
float zcam_mintgtheight = -1e+38f;
int zcam_fly;
int zcam_flypaused;
void *zcam_flydata;
unsigned int zcam_flysize;
float zcam_flytime;
unsigned int zcam_flyasset_current;
xCamAsset *zcam_dest;
xQuat zcam_quat;
float zcam_tmr;
float zcam_ttm;
float zcam_fovcurr;
float zcam_fovdest;

// temp
int zcam_debug = 0;

namespace
{
enum WallJumpViewState
{
    WJVS_DISABLED,
    WJVS_DISABLING,
    WJVS_ENABLED,
    WJVS_ENABLING
};

unsigned int stop_track;
bool input_enabled;
bool lassocam_enabled;
float lassocam_factor;
WallJumpViewState wall_jump_enabled;
xVec3 wall_jump_view;
float dMultiplier;
float dOffset;
float hMultiplier;
float hOffset;

float GetCurrentPitch()
{
    if (zcam_highbounce)
    {
        return zcam_highbounce_pitch;
    }
    
    return zCameraTweakGlobal_GetPitch();
}

float _GetCurrentH()
{
    if (zcam_highbounce)
    {
        return zcam_highbounce_h;
    }

    if (wall_jump_enabled == WJVS_ENABLED)
    {
        return zcam_wall_h;
    }

    return zCameraTweakGlobal_GetH();
}

float GetCurrentH()
{
    return dMultiplier * _GetCurrentH() + dOffset;
}

float _GetCurrentD()
{
    if (zcam_highbounce)
    {
        return zcam_highbounce_d;
    }

    if (wall_jump_enabled == WJVS_ENABLED)
    {
        return zcam_wall_d;
    }

    return zCameraTweakGlobal_GetD();
}

float GetCurrentD()
{
    return dMultiplier * _GetCurrentD() + dOffset;
}
}

void zCameraReset(xCamera *cam)
{
    zcam_mode = 0;
    zcam_bbounce = 0;
    zcam_lbbounce = 0;
    zcam_lconvers = 0;
    zcam_longbounce = 0;
    zcam_highbounce = 0;
    zcam_convers = 0;
    zcam_fly = 0;
    zcam_flypaused = 0;
    zcam_cutscene = 0;
    zcam_reward = 0;
    zcam_fovcurr = CAMERA_FOV;
    zcam_overrot_tmr = 0.0f;

    wall_jump_enabled = WJVS_DISABLED;
    lassocam_enabled = false;
    stop_track = 0;

    zcam_mintgtheight = -1e+38f;

    xCameraSetFOV(cam, CAMERA_FOV);

    zCameraTweakGlobal_Update(0.0f);

    xCameraReset(cam, GetCurrentD(), GetCurrentH(), GetCurrentPitch());

    input_enabled = true;

    dMultiplier = 1.0f;
    dOffset = 0.0f;
    hMultiplier = 1.0f;
    hOffset = 0.0f;
}

static float EaseInOut(float x)
{
    return x * (x * -(1.5f * x - 2.0f) + 0.5f);
}

static void zCameraConversUpdate(xCamera *cam, float dt)
{
    if (zcam_dest)
    {
        if (zcam_tmr <= 0.0f)
        {
            zcam_tmr = 0.0f;
        }
        else
        {
            if ((dt / zcam_tmr) > 1.0f)
            {
                cam->mat.right = zcam_dest->right;
                cam->mat.up = zcam_dest->up;
                cam->mat.at = zcam_dest->at;
                cam->mat.pos = zcam_dest->pos;

                zcam_fovcurr = zcam_fovdest;
            }
            else
            {
                float s = EaseInOut(1.0f - zcam_tmr / zcam_ttm);
                s = (EaseInOut(1.0f - (zcam_tmr - dt) / zcam_ttm) - s) / (1.0f - s);

                xQuat a, c;

                xQuatFromMat(&a, &cam->mat);
                xQuatSlerp(&c, &a, &zcam_quat, s);
                xQuatToMat(&c, &cam->mat);

                xVec3Lerp(&cam->mat.pos, &cam->mat.pos, &zcam_dest->pos, s);

                zcam_fovcurr = zcam_fovcurr * (1.0f - s) + zcam_fovdest * s;
            }

            zcam_tmr -= dt;
        }
    }
}

static float TranSpeed(zFlyKey *keys)
{
    return 2 * FRAMES_PER_SEC *
        xVec3Dist((xVec3 *)&keys[0].matrix[9], (xVec3 *)&keys[1].matrix[9]);
}

static float MatrixSpeed(zFlyKey *keys)
{
    float dot;

    dot = iabs(xVec3Dot((xVec3 *)&keys[0].matrix[0], (xVec3 *)&keys[1].matrix[0]));

    if (iabs(xVec3Dot((xVec3 *)&keys[0].matrix[3], (xVec3 *)&keys[1].matrix[3])) > dot)
    {
        dot = iabs(xVec3Dot((xVec3 *)&keys[0].matrix[3], (xVec3 *)&keys[1].matrix[3]));
    }

    if (iabs(xVec3Dot((xVec3 *)&keys[0].matrix[6], (xVec3 *)&keys[1].matrix[6])) > dot)
    {
        dot = iabs(xVec3Dot((xVec3 *)&keys[0].matrix[6], (xVec3 *)&keys[1].matrix[6]));
    }

    return 2 * FRAMES_PER_SEC * RAD2DEG(2) * xacos(xmin(1.0f, dot));
}

static int zCameraFlyUpdate(xCamera *cam, float dt)
{
    int i;
    int flyIdx;
    int numKeys;
    float flyLerp;
    float flyFrame;
    zFlyKey keys[4];
    float matdiff1;
    float matdiff2;
    float matdiff3;
    xMat3x3 tmpMat;
    xQuat quats[2];
    xQuat qresult;

    if ((globals.pad0->pressed & SKIP_BUTTONS) &&
        (zcam_flytime > gSkipTimeFlythrough))
    {
        zcam_flytime = 2 * ONE_FRAME * zcam_flysize;
    }

    flyFrame = FRAMES_PER_SEC / 2 * zcam_flytime;
    flyIdx = (int)xfloor(flyFrame);
    flyLerp = flyFrame - xfloor(flyFrame);

    numKeys = zcam_flysize / sizeof(zFlyKey);

    if (flyIdx >= numKeys - 1)
    {
        return 0;
    }

    keys[0] = ((zFlyKey *)zcam_flydata)[((flyIdx - 1) >= 0) ? (flyIdx - 1) : flyIdx];
    keys[1] = ((zFlyKey *)zcam_flydata)[flyIdx];
    keys[2] = ((zFlyKey *)zcam_flydata)[flyIdx + 1];
    keys[3] = ((zFlyKey *)zcam_flydata)[((flyIdx + 2) < numKeys) ? (flyIdx + 2) : (flyIdx + 1)];

    // TODO: swap endian if on gamecube

    if (flyIdx > 0)
    {
        matdiff1 = TranSpeed(&keys[0]);
        matdiff2 = TranSpeed(&keys[1]);
        matdiff3 = TranSpeed(&keys[2]);

        if (matdiff2 > 10.0f &&
            matdiff2 > (5.0f * matdiff1) &&
            matdiff2 > (5.0f * matdiff3))
        {
            flyLerp = 0.0f;
        }
        else
        {
            matdiff1 = MatrixSpeed(&keys[0]);
            matdiff2 = MatrixSpeed(&keys[1]);
            matdiff2 = MatrixSpeed(&keys[2]);

            if (matdiff2 > 45.0f &&
                matdiff2 > (5.0f * matdiff1) &&
                matdiff2 > (5.0f * matdiff3))
            {
                flyLerp = 0.0f;
            }
        }
    }

    for (i = 0; i < 2; i++)
    {
        tmpMat.right.x = -keys[i + 1].matrix[0];
        tmpMat.right.y = -keys[i + 1].matrix[1];
        tmpMat.right.z = -keys[i + 1].matrix[2];

        tmpMat.up.x = keys[i + 1].matrix[3];
        tmpMat.up.y = keys[i + 1].matrix[4];
        tmpMat.up.z = keys[i + 1].matrix[5];

        tmpMat.at.x = -keys[i + 1].matrix[6];
        tmpMat.at.y = -keys[i + 1].matrix[7];
        tmpMat.at.z = -keys[i + 1].matrix[8];

        xQuatFromMat(&quats[i], &tmpMat);
    }

    xQuatSlerp(&qresult, &quats[0], &quats[1], flyLerp);
    xQuatToMat(&qresult, &cam->mat);

    xVec3Lerp(&cam->mat.pos, (xVec3 *)&keys[1].matrix[9], (xVec3 *)&keys[2].matrix[9],
              flyLerp);

    zcam_flytime += dt;

    return 1;
}

static void zCameraFlyRestoreBackup(xCamera *backup)
{
    globals.camera.mat = backup->mat;
    globals.camera.omat = backup->omat;
    globals.camera.mbasis = backup->mbasis;
    globals.camera.bound = backup->bound;
    globals.camera.focus = backup->focus;
    globals.camera.flags = backup->flags;
    globals.camera.tmr = backup->tmr;
    globals.camera.tm_acc = backup->tm_acc;
    globals.camera.tm_dec = backup->tm_dec;
    globals.camera.ltmr = backup->ltmr;
    globals.camera.ltm_acc = backup->ltm_acc;
    globals.camera.ltm_dec = backup->ltm_dec;
    globals.camera.dmin = backup->dmin;
    globals.camera.dmax = backup->dmax;
    globals.camera.dcur = backup->dcur;
    globals.camera.dgoal = backup->dgoal;
    globals.camera.hmin = backup->hmin;
    globals.camera.hmax = backup->hmax;
    globals.camera.hcur = backup->hcur;
    globals.camera.hgoal = backup->hgoal;
    globals.camera.pmin = backup->pmin;
    globals.camera.pmax = backup->pmax;
    globals.camera.pcur = backup->pcur;
    globals.camera.pgoal = backup->pgoal;
    globals.camera.depv = backup->depv;
    globals.camera.hepv = backup->hepv;
    globals.camera.pepv = backup->pepv;
    globals.camera.orn_epv = backup->orn_epv;
    globals.camera.yaw_epv = backup->yaw_epv;
    globals.camera.pitch_epv = backup->pitch_epv;
    globals.camera.roll_epv = backup->roll_epv;
    globals.camera.orn_cur = backup->orn_cur;
    globals.camera.orn_goal = backup->orn_goal;
    globals.camera.yaw_cur = backup->yaw_cur;
    globals.camera.yaw_goal = backup->yaw_goal;
    globals.camera.pitch_cur = backup->pitch_cur;
    globals.camera.pitch_goal = backup->pitch_goal;
    globals.camera.roll_cur = backup->roll_cur;
    globals.camera.roll_goal = backup->roll_goal;
    globals.camera.dct = backup->dct;
    globals.camera.dcd = backup->dcd;
    globals.camera.dccv = backup->dccv;
    globals.camera.dcsv = backup->dcsv;
    globals.camera.hct = backup->hct;
    globals.camera.hcd = backup->hcd;
    globals.camera.hccv = backup->hccv;
    globals.camera.hcsv = backup->hcsv;
    globals.camera.pct = backup->pct;
    globals.camera.pcd = backup->pcd;
    globals.camera.pccv = backup->pccv;
    globals.camera.pcsv = backup->pcsv;
    globals.camera.orn_ct = backup->orn_ct;
    globals.camera.orn_cd = backup->orn_cd;
    globals.camera.orn_ccv = backup->orn_ccv;
    globals.camera.orn_csv = backup->orn_csv;
    globals.camera.yaw_ct = backup->yaw_ct;
    globals.camera.yaw_cd = backup->yaw_cd;
    globals.camera.yaw_ccv = backup->yaw_ccv;
    globals.camera.yaw_csv = backup->yaw_csv;
    globals.camera.pitch_ct = backup->pitch_ct;
    globals.camera.pitch_cd = backup->pitch_cd;
    globals.camera.pitch_ccv = backup->pitch_ccv;
    globals.camera.pitch_csv = backup->pitch_csv;
    globals.camera.roll_ct = backup->roll_ct;
    globals.camera.roll_cd = backup->roll_cd;
    globals.camera.roll_ccv = backup->roll_ccv;
    globals.camera.roll_csv = backup->roll_csv;
}

float rewardMove = 1.0f;
float rewardMoveSpeed = 1.1f;
float rewardZoomSpeed = 6.0f;
float rewardZoomAmount = 100.0f;
float rewardTiltTime = 1.5f;
float rewardTiltAmount = -0.22f;

static int zCameraRewardUpdate(xCamera *cam, float dt)
{
    xCameraUpdate(cam, dt);

    xVec3 v = { 0.0f, 0.0f, 0.0f };

    xVec3Copy(&v, (xVec3 *)&globals.player.ent.model->Mat->pos);

    v.y -= 0.7f;

    if (zcam_near)
    {
        if (globals.player.s->pcType == ePlayer_SB)
        {
            rewardMove = 1.3f;
            rewardMoveSpeed = 0.68f;
            rewardZoomSpeed = 7.1f;
            rewardZoomAmount = 108.0f;
            rewardTiltTime = 1.5f;
            rewardTiltAmount = -0.22f;
        }
        else if (globals.player.s->pcType == ePlayer_Patrick)
        {
            rewardMove = 1.6f;
            rewardMoveSpeed = 0.68f;
            rewardZoomSpeed = 7.1f;
            rewardZoomAmount = 108.0f;
            rewardTiltTime = 1.0f;
            rewardTiltAmount = -0.25f;
        }
        else if (globals.player.s->pcType == ePlayer_Sandy)
        {
            rewardMove = 1.3f;
            rewardMoveSpeed = 0.68f;
            rewardZoomSpeed = 7.1f;
            rewardZoomAmount = 108.0f;
            rewardTiltTime = 1.5f;
            rewardTiltAmount = -0.22f;
        }
    }
    else
    {
        if (globals.player.s->pcType == ePlayer_SB)
        {
            rewardMove = 1.5f;
            rewardMoveSpeed = 1.1f;
            rewardZoomSpeed = 5.9f;
            rewardZoomAmount = 100.0f;
            rewardTiltTime = 1.5f;
            rewardTiltAmount = -0.2f;
        }
        else if (globals.player.s->pcType == ePlayer_Patrick)
        {
            rewardMove = 1.5f;
            rewardMoveSpeed = 1.1f;
            rewardZoomSpeed = 5.9f;
            rewardZoomAmount = 100.0f;
            rewardTiltTime = 1.5f;
            rewardTiltAmount = -0.2f;
        }
        else if (globals.player.s->pcType == ePlayer_Sandy)
        {
            rewardMove = 1.5f;
            rewardMoveSpeed = 1.1f;
            rewardZoomSpeed = 5.9f;
            rewardZoomAmount = 100.0f;
            rewardTiltTime = 1.5f;
            rewardTiltAmount = -0.2f;
        }
    }

    if (xVec3Dist2(
            (xVec3 *)&globals.player.ent.model->Mat->pos,
            &globals.camera.mat.pos)
        > xsqr(rewardMove))
    {
        xCameraMove(cam, v, rewardMoveSpeed * dt);
        xCameraFOV(cam, rewardZoomAmount, rewardZoomSpeed, dt);
    }

    xCameraLookYPR(cam, 0,
                   globals.camera.yaw_cur, rewardTiltAmount, globals.camera.roll_cur,
                   rewardTiltTime, 0.1f, 0.1f);

    return 1;
}

static void zCameraFreeLookSetGoals(xCamera *cam, float pitch_s, float &dgoal,
                                    float &hgoal, float &pitch_goal, float &lktm,
                                    float dt)
{
    if (zcam_bbounce)
    {
        if (zcam_highbounce)
        {
            dgoal = GetCurrentD();
            hgoal = GetCurrentH();
            pitch_goal = GetCurrentPitch();
        }
        else
        {
            dgoal = zcam_near ? 3.5f : GetCurrentD();
            hgoal = zcam_near ? 2.4f : GetCurrentH();

            if (zcam_longbounce)
            {
                float s = xsqrt(
                    xsqr(zcam_playervel->x) +
                    xsqr(zcam_playervel->y) +
                    xsqr(zcam_playervel->z));

                if (zcam_playervel && s != 0.0f)
                {
                    s = -xmin(0.0f,
                             (cam->mat.at.x * zcam_playervel->x +
                              cam->mat.at.y * zcam_playervel->y +
                              cam->mat.at.z * zcam_playervel->z) / s);
                }
                else
                {
                    s = 0.0f;
                }

                pitch_goal = zcam_near ? DEG2RAD(20.0f * s + 20.0f) : DEG2RAD(30.0f);
            }
            else
            {
                pitch_goal = zcam_near ? DEG2RAD(40.0f) : DEG2RAD(30.0f);
            }
        }
    }
    else
    {
        float dcur = GetCurrentD();
        float hcur = GetCurrentH();
        float pcur = GetCurrentPitch();

        if (lassocam_enabled && stop_track == 0)
        {
            dgoal = lassocam_factor * (dcur - zcam_near_d) + zcam_near_d;
            hgoal = lassocam_factor * (hcur - zcam_near_h) + zcam_near_h;
            pitch_goal = lassocam_factor * (pcur - zcam_near_pitch) + zcam_near_pitch;
        }
        else
        {
            if (pitch_s > 0.0f)
            {
                dgoal = pitch_s * (zcam_below_d - dcur) + dcur;
                hgoal = pitch_s * (zcam_below_h - hcur) + hcur;
                pitch_goal = pitch_s * (zcam_below_pitch - pcur) * xsqr(pitch_s) + pcur;
            }
            else
            {
                dgoal = -pitch_s * (zcam_above_d - dcur) + dcur;
                hgoal = -pitch_s * (zcam_above_h - hcur) + hcur;
                pitch_goal = -pitch_s * (zcam_above_pitch - pcur) + pcur;
            }

            if (lktm > 0.1f)
            {
                lktm -= dt;

                if (lktm < 0.1f)
                {
                    lktm = 0.1f;
                }
            }
            else
            {
                lktm = 0.1f;
            }
        }
    }
}

static void zCameraDebugUpdate(xCamera *cam, float dt)
{
    float movespd = 10.0f * dt;
    float rotspd = 2.0f * dt;

    if (globals.pad0->on & XPAD_BUTTON_DBG_CAM_SPRINT)
    {
        movespd *= 2.0f;
        rotspd *= 2.0f;
    }

    xVec3 rot;
    xMat3x3GetEuler(&cam->mat, &rot);

    xVec3 drot;
    drot.x = ((float)globals.pad0->analog2.x / 127) * rotspd;
    drot.y = -((float)globals.pad0->analog2.y / 127) * rotspd;
    drot.z = 0.0f;

    xVec3AddTo(&rot, &drot);
    xMat3x3Euler(&cam->mat, &rot);
    
    xVec3 dpos;
    dpos.x = -((float)globals.pad0->analog1.x / 127) * movespd;
    dpos.z = -((float)globals.pad0->analog1.y / 127) * movespd;
    
    dpos.y = 0.0f;

    if (globals.pad0->on & XPAD_BUTTON_DBG_CAM_UP)
    {
        dpos.y += movespd;
    }
    
    if (globals.pad0->on & XPAD_BUTTON_DBG_CAM_DOWN)
    {
        dpos.y -= movespd;
    }

    xMat4x3MoveLocalRight(&cam->mat, dpos.x);
    xMat4x3MoveLocalUp(&cam->mat, dpos.y);
    xMat4x3MoveLocalAt(&cam->mat, dpos.z);
}

void zCameraUpdate(xCamera *cam, float dt)
{
    if (globals.sceneCur->sceneID == 'HB02' ||
        globals.sceneCur->sceneID == 'HB03' ||
        globals.sceneCur->sceneID == 'HB04' ||
        globals.sceneCur->sceneID == 'HB06' ||
        globals.sceneCur->sceneID == 'HB07' ||
        globals.sceneCur->sceneID == 'HB08' ||
        globals.sceneCur->sceneID == 'HB09' ||
        globals.sceneCur->sceneID == 'HB10')
    {
        zcam_near |= ZCAM_NEAR_INDOORS;
    }
    else
    {
        zcam_near &= ZCAM_NEAR_ON;
    }

    zCameraTweakGlobal_Update(dt);

    xVec3 tran_accum = cam->tran_accum;
    
    cam->tran_accum.x = 0.0f;
    cam->tran_accum.y = 0.0f;
    cam->tran_accum.z = 0.0f;

    if (globals.cmgr && globals.cmgr->csn->Ready)
    {
        if (zcam_fly && zcam_flypaused)
        {
            zcam_fly = 0;
            zcam_flypaused = 0;

            zEntPlayerControlOn(CONTROL_OWNER_FLY_CAM);
            xScrFxLetterbox(0);
            zCameraFlyRestoreBackup(&zcam_backupcam);

            zcam_flyasset_current = 0;

            xCameraSetFOV(cam, CAMERA_FOV);
        }

        if (!zcam_cutscene)
        {
            zcam_backupcam = globals.camera;
            zcam_cutscene = 1;
        }

        xCutscene_SetCamera(globals.cmgr->csn, cam);
        iCameraUpdatePos(cam->lo_cam, &cam->mat);

        return;
    }

    if (globals.pad0->pressed & XPAD_BUTTON_DBG_CAM_TOGGLE)
    {
        zcam_debug = !zcam_debug;
    }
    
    if (zcam_debug)
    {
        zCameraDebugUpdate(cam, dt);
        iCameraUpdatePos(cam->lo_cam, &cam->mat);

        return;
    }

    if (zcam_cutscene)
    {
        zCameraFlyRestoreBackup(&zcam_backupcam);
        xCameraSetFOV(cam, CAMERA_FOV);

        zcam_cutscene = 0;
    }

    if (zcam_convers)
    {
        zCameraConversUpdate(cam, dt);
        iCameraUpdatePos(cam->lo_cam, &cam->mat);
        xCameraSetFOV(cam, zcam_fovcurr);

        zcam_lconvers = zcam_convers;

        return;
    }

    if (zcam_reward)
    {
        float minDist = zcam_near ? 12.14f : 33.9f;
        float dist = xsqr(cam->dcur) + xsqr(cam->hcur);

        if (stop_track & CO_REWARDANIM)
        {
            zCameraRewardUpdate(cam, dt);
            return;
        }
        else if (dist > minDist)
        {
            zCameraDisableTracking(CO_REWARDANIM);
        }
    }

    if (zcam_fly)
    {
        if (zCameraFlyUpdate(cam, dt))
        {
            iCameraUpdatePos(cam->lo_cam, &cam->mat);
            return;
        }
        else
        {
            if (!zcam_flypaused)
            {
                zCameraFlyProcessStopEvent();
            }

            if (globals.cmgr)
            {
                zcam_flypaused = 1;
                return;
            }

            zcam_fly = 0;

            zEntPlayerControlOn(CONTROL_OWNER_FLY_CAM);
            xScrFxLetterbox(0);
            zCameraFlyRestoreBackup(&zcam_backupcam);

            zcam_flyasset_current = 0;

            xCameraSetFOV(cam, CAMERA_FOV);
        }
    }

    if (stop_track)
    {
        if (stop_track & CO_OOB)
        {
            xCameraUpdate(cam, dt);
        }
        else
        {
            iCameraUpdatePos(cam->lo_cam, &cam->mat);
        }

        return;
    }

    float tgtHeight = cam->tgt_mat->pos.y;
    float oldTgtHeight = cam->tgt_omat->pos.y;

    if (tgtHeight < zcam_mintgtheight)
    {
        cam->tgt_omat->pos.y += zcam_mintgtheight - tgtHeight;
        cam->tgt_mat->pos.y += zcam_mintgtheight - cam->tgt_mat->pos.y;
    }

    float plerp = RAD2DEG(0.1f) * (iabs(cam->pitch_cur) - DEG2RAD(70.0f));

    if (0.0f > xmin(plerp, 1.0f))
    {
        plerp = 0.0f;
    }
    else
    {
        plerp = xmin(plerp, 1.0f);
    }

    float dlerp = (5.0f / 3.0f) * (1.2f - xsqrt(
        xsqr(cam->mat.pos.x - cam->tgt_mat->pos.x) +
        xsqr(cam->mat.pos.z - cam->tgt_mat->pos.z)));

    if (0.0f > xmin(dlerp, 1.0f))
    {
        dlerp = 0.0f;
    }
    else
    {
        dlerp = xmin(dlerp, 1.0f);
    }

    dlerp = xmax(dlerp, plerp);

    if (dlerp != 0.0f)
    {
        cam->mat.pos.x += dlerp * (cam->tgt_mat->pos.x - cam->tgt_omat->pos.x - tran_accum.x);
        cam->mat.pos.y += dlerp * (cam->tgt_mat->pos.y - cam->tgt_omat->pos.y - tran_accum.y);
        cam->mat.pos.z += dlerp * (cam->tgt_mat->pos.z - cam->tgt_omat->pos.z - tran_accum.z);

        cam->omat.pos = cam->mat.pos;
    }

    static float mvtm = 0.1f;
    static float lktm = 0.1f;
    static float pitch_s = 0.0f;

    if (wall_jump_enabled == WJVS_ENABLING ||
        wall_jump_enabled == WJVS_DISABLING)
    {
        mvtm = 0.3f;
        lktm = 0.3f;

        if (wall_jump_enabled == WJVS_ENABLING)
        {
            wall_jump_enabled = WJVS_ENABLED;
        }
    }

    if (globals.pad0->pressed & ZPAD_BUTTON_CAM_TOGGLE)
    {
        if (input_enabled && !(zcam_near & ZCAM_NEAR_INDOORS))
        {
            zcam_near ^= ZCAM_NEAR_ON;

            mvtm = 0.3f;
            lktm = 0.3f;
            pitch_s = 0.0f;
        }
    }
    else if (zcam_bbounce != zcam_lbbounce)
    {
        mvtm = 0.3f;
        lktm = 0.3f;
        pitch_s = 0.0f;
    }
    else if (mvtm > 0.1f)
    {
        mvtm -= dt;

        if (mvtm < 0.1f)
        {
            mvtm = 0.1f;
        }
    }

    xVec3 delta;
    delta.z = GetCurrentD();
    delta.y = GetCurrentH();
    delta.x = zcam_near ? zcam_near_pitch : zcam_far_pitch;

    float pgoal = cam->pcur;

    if (lassocam_enabled && stop_track == 0)
    {
        delta.z = lassocam_factor * (delta.z - zcam_near_d) + zcam_near_d;
        delta.y = lassocam_factor * (delta.y - zcam_near_h) + zcam_near_h;
    }

    if (input_enabled && wall_jump_enabled == WJVS_DISABLED)
    {
        if (globals.pad0->analog2.x > ZPAD_ANALOG_MIN)
        {
            int x = xmax(xmin(globals.pad0->analog2.x, ZPAD_ANALOG_MAX), ZPAD_ANALOG_MIN);
            float dp = ONE_FRAME * (x - ZPAD_ANALOG_MIN) * zcam_pad_pyaw_scale;

            if (lassocam_enabled && stop_track == 0)
            {
                dp *= 0.2f;
            }

            pgoal += dp;

            cam->pcur += dp;
            cam->pgoal += dp;

            if (lktm > 0.025f)
            {
                lktm -= dt;

                if (lktm < 0.025f)
                {
                    lktm = 0.025f;
                }
            }

            zcam_overrot_tmr = -zcam_overrot_tmanual;
        }
        else if (globals.pad0->analog2.x < -ZPAD_ANALOG_MIN)
        {
            int x = xmax(xmin(globals.pad0->analog2.x, -ZPAD_ANALOG_MIN), -ZPAD_ANALOG_MAX);
            float dp = ONE_FRAME * (x + ZPAD_ANALOG_MIN) * zcam_pad_pyaw_scale;

            if (lassocam_enabled && stop_track == 0)
            {
                dp *= 0.2f;
            }

            pgoal += dp;

            cam->pcur += dp;
            cam->pgoal += dp;

            if (lktm > 0.025f)
            {
                lktm -= dt;

                if (lktm < 0.025f)
                {
                    lktm = 0.025f;
                }
            }

            zcam_overrot_tmr = -zcam_overrot_tmanual;
        }
    }

    pitch_s = 0.0f;

    if (input_enabled && wall_jump_enabled == WJVS_DISABLED && !zcam_highbounce)
    {
        if (globals.pad0->analog2.y > ZPAD_ANALOG_MIN)
        {
            int y = xmax(xmin(globals.pad0->analog2.y, ZPAD_ANALOG_MAX), ZPAD_ANALOG_MIN);

            pitch_s = ONE_FRAME * (y - ZPAD_ANALOG_MIN) * zcam_pad_pitch_scale;

            if (pitch_s > 1.0f)
            {
                pitch_s = 1.0f;
            }

            zcam_overrot_tmr = -zcam_overrot_tmanual;
        }
        else if (globals.pad0->analog2.y < -ZPAD_ANALOG_MIN)
        {
            int y = xmax(xmin(globals.pad0->analog2.y, -ZPAD_ANALOG_MIN), -ZPAD_ANALOG_MAX);

            pitch_s = ONE_FRAME * (y + ZPAD_ANALOG_MIN) * zcam_pad_pitch_scale;

            if (pitch_s < -1.0f)
            {
                pitch_s = -1.0f;
            }

            zcam_overrot_tmr = -zcam_overrot_tmanual;
        }
    }

    if (dt > EPSILON && cam->tgt_mat && cam->tgt_omat && dlerp < 0.9999f)
    {
        float velx = (cam->tgt_mat->pos.x - cam->tgt_omat->pos.x - tran_accum.x) / dt;
        float velz = (cam->tgt_mat->pos.z - cam->tgt_omat->pos.z - tran_accum.z) / dt;
        float camx = cam->tgt_mat->pos.x - cam->mat.pos.x;
        float camz = cam->tgt_mat->pos.z - cam->mat.pos.z;

        float cammag = xsqrt(xsqr(camx) + xsqr(camz));
        float velmag = xsqrt(xsqr(velx) + xsqr(velz));

        if (velmag > zcam_overrot_velmin || zcam_overrot_tmr < 0.0f)
        {
            zcam_overrot_tmr += dt;
        }
        else if (zcam_overrot_tmr > 0.0f)
        {
            if (zcam_overrot_tmr > zcam_overrot_tend)
            {
                zcam_overrot_tmr = zcam_overrot_tend;
            }

            zcam_overrot_tmr -= dt;

            if (zcam_overrot_tmr < 0.0f)
            {
                zcam_overrot_tmr = 0.0f;
            }
        }

        if (zcam_overrot_tmr > zcam_overrot_tstart &&
            cammag > 1.2f &&
            velmag > zcam_overrot_velmin)
        {
            camz /= cammag;
            velz /= velmag;
            camx /= cammag;
            velx /= velmag;

            camz = camz * velx - camx * velz;

            float velcos = xacos(xclamp(camx * velx + camz * velz, -1.0f, 1.0f));

            if (velcos > zcam_overrot_min && velcos < zcam_overrot_max)
            {
                float angle_factor;

                if (velcos <= zcam_overrot_mid)
                {
                    angle_factor =
                        (velcos - zcam_overrot_min) /
                        (zcam_overrot_mid - zcam_overrot_min);
                }
                else
                {
                    angle_factor =
                        (zcam_overrot_max - velcos) /
                        (zcam_overrot_max - zcam_overrot_mid);
                }

                float vel_factor = xclamp(
                    (velmag - zcam_overrot_velmin) /
                    (zcam_overrot_velmax - zcam_overrot_velmin),
                    0.0f, 1.0f);

                float time_factor = xclamp(
                    (zcam_overrot_tmr - zcam_overrot_tstart) /
                    (zcam_overrot_tend - zcam_overrot_tstart),
                    0.0f, 1.0f);

                float dp = angle_factor * vel_factor * zcam_overrot_rate * time_factor;

                if (lassocam_enabled && stop_track == 0)
                {
                    dp *= 0.2f;
                }

                if (camz > 0.0f)
                {
                    pgoal += dp;

                    cam->pcur += dp;
                    cam->pgoal += dp;
                }
                else
                {
                    pgoal -= dp;

                    cam->pcur -= dp;
                    cam->pgoal -= dp;
                }
            }
        }
    }

    zCameraFreeLookSetGoals(cam, pitch_s, delta.z, delta.y, delta.x, lktm, dt);

    float dx__ = cam->tgt_mat->pos.x - cam->mat.pos.x;
    float dy__ = cam->tgt_mat->pos.y - cam->mat.pos.y;
    float dz__ = cam->tgt_mat->pos.z - cam->mat.pos.z;

    float dist2 = xsqr(dx__) + xsqr(dy__) + xsqr(dz__);

    float f1, f2;

    if (CLOSEENOUGH(dist2, 1.0f))
    {
        f1 = dx__;
        f2 = dz__;
    }
    else if (CLOSEENOUGH(dist2, 0.0f))
    {
        f1 = 0.0f;
        f2 = 0.0f;
    }
    else
    {
        float dist_inv = 1.0f / xsqrt(dist2);

        f1 = dx__ * dist_inv;
        f2 = dz__ * dist_inv;
    }

    float f22 = xatan2(f1, f2);

    if (lassocam_enabled && stop_track == 0)
    {
        xCameraMove(cam, 0x20, delta.z, delta.y, pgoal, mvtm, 0.0f, 0.0f);
    }
    else
    {
        xCameraMove(cam, 0x28, delta.z, delta.y, pgoal, mvtm, 0.0f, 0.0f);
    }

    xCameraLookYPR(cam, 0, f22, delta.x, 0.0f, lktm, 0.0f, 0.0f);

    if (wall_jump_enabled == WJVS_ENABLED)
    {
        xCameraRotate(cam, wall_jump_view, 0.0f, 0.5f, 0.1f, 0.1f);

        cam->dcur = cam->dgoal;

        xVec3 destPosition;

        xVec3SMul(&destPosition, &wall_jump_view, -cam->dcur);
        xVec3Add(&destPosition, &destPosition,
            (xVec3 *)&globals.player.ent.model->Mat->pos);

        destPosition.y += delta.y * 0.5f;

        xCameraMove(cam, destPosition, 25.0f * dt);

        cam->flags |= 0x20;
    }
    else if (wall_jump_enabled == WJVS_DISABLING)
    {
        cam->dcur = cam->dgoal;
        cam->pcur = cam->pgoal;
        cam->hcur = cam->hcur;

        wall_jump_enabled = WJVS_DISABLED;
    }

    xCameraUpdate(cam, dt);

    if (lassocam_enabled && stop_track == 0)
    {
        cam->pcur = pgoal;
    }

    zcam_lbbounce = zcam_bbounce;

    cam->tgt_mat->pos.y = tgtHeight;
    cam->tgt_omat->pos.y = oldTgtHeight;
}

void zCameraDisableTracking(camera_owner_enum owner)
{
    stop_track |= owner;
}