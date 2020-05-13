#include "xCamera.h"

#include "xstransvc.h"
#include "xMath.h"
#include "xMathInlines.h"
#include "iMath.h"

#include "print.h"

#include <rwcore.h>
#include <rpworld.h>

int xcam_collis_owner_disable;
int xcam_do_collis = 1;
float xcam_collis_radius = 0.4f;
float xcam_collis_stiffness = 0.3f;

static RpAtomic *sInvisWallHack;

static void xCameraFXInit();
static void xCam_cyltoworld(xVec3 *pos, const xMat4x3 *mat, float d, float h,
                            float pitch, unsigned int flags);

void xCameraInit(xCamera *cam, unsigned int width, unsigned int height)
{
    xCameraFXInit();
    
    cam->lo_cam = iCameraCreate(width, height, 1);

    xCameraSetFOV(cam, 75.0f);

    cam->bound.sph.center.x = 0.0f;
    cam->bound.sph.center.y = 0.0f;
    cam->bound.sph.center.z = 0.0f;
    cam->bound.sph.r = 0.5f;

    cam->tgt_mat = NULL;
    cam->tgt_omat = NULL;
    cam->tgt_bound = NULL;

    cam->sc = NULL;

    cam->tran_accum.x = 0.0f;
    cam->tran_accum.y = 0.0f;
    cam->tran_accum.z = 0.0f;

    add_camera_tweaks();
}

void add_camera_tweaks()
{
    return;
}

void xCameraReset(xCamera *cam, float d, float h, float pitch)
{
    float goal_p;

    sInvisWallHack = (RpAtomic *)xSTFindAsset(0xB8895D14, NULL); // invisible_wall

    xMat4x3Identity(&cam->mat);

    cam->omat = cam->mat;

    cam->focus.x = 0.0f;
    cam->focus.y = 0.0f;
    cam->focus.z = 10.0f;

    cam->tran_accum.x = 0.0f;
    cam->tran_accum.y = 0.0f;
    cam->tran_accum.z = 0.0f;

    cam->flags = 0;

    goal_p = PI;

    if (cam->tgt_mat)
    {
        goal_p += xatan2(cam->tgt_mat->at.x, cam->tgt_mat->at.z);
    }

    xCameraMove(cam, 0x2E, d, h, goal_p, 0.0f, 2.0f / 3.0f, 2.0f / 3.0f);

    cam->pitch_goal = pitch;
    cam->pitch_cur = pitch;
    cam->roll_cur = 0.0f;

    xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);

    cam->omat = cam->mat;

    cam->yaw_ct = 1.0f;
    cam->yaw_cd = 1.0f;
    cam->yaw_ccv = 0.65f;
    cam->yaw_csv = 1.0f;

    cam->pitch_ct = 1.0f;
    cam->pitch_cd = 1.0f;
    cam->pitch_ccv = 0.7f;
    cam->pitch_csv = 1.0f;

    cam->roll_ct = 1.0f;
    cam->roll_cd = 1.0f;
    cam->roll_ccv = 0.7f;
    cam->roll_csv = 1.0f;

    cam->flags |= 0x80;

    xcam_do_collis = 1;
    xcam_collis_owner_disable = 0;
}

static void xCam_cyltoworld(xVec3 *pos, const xMat4x3 *mat, float d, float h,
                            float pitch, unsigned int flags)
{
    if (flags & 0x10)
    {
        pos->y = h;
    }
    else
    {
        pos->y = h + mat->pos.y;
    }

    if (flags & 0x20)
    {
        pos->x = d * isin(pitch) + mat->pos.x;
        pos->z = d * icos(pitch) + mat->pos.z;
    }
    else
    {
        pitch += xatan2(mat->at.x, mat->at.z);

        pos->x = d * isin(pitch) + mat->pos.x;
        pos->z = d * icos(pitch) + mat->pos.z;
    }
}

void xCameraUpdate(xCamera *cam, float dt)
{
    BFBBSTUB("xCameraUpdate");
}

static void xCameraFXInit()
{
    BFBBSTUB("xCameraFXInit");
}

void xCameraSetScene(xCamera *cam, xScene *sc)
{
    cam->sc = sc;

    iCameraAssignEnv(cam->lo_cam, sc->env->geom);
}

void xCameraMove(xCamera *cam, unsigned int flags, float dgoal, float hgoal, float pgoal,
                 float tm, float tm_acc, float tm_dec)
{
    float s;

    cam->flags = (cam->flags & ~0x3E) | (flags & 0x3E);
    cam->dgoal = dgoal;
    cam->hgoal = hgoal;
    cam->pgoal = pgoal;

    if (tm > 0.0f)
    {
        if (cam->tgt_mat)
        {
            cam->dcur = dgoal;
            cam->hcur = hgoal;
            cam->pcur = pgoal;

            xCam_cyltoworld(&cam->mat.pos, cam->tgt_mat, cam->dcur, cam->hcur, cam->pcur,
                            cam->flags);

            cam->omat.pos = cam->mat.pos;

            cam->yaw_goal = cam->pcur + ((cam->pcur >= PI) ? -PI : PI);
            cam->yaw_cur = cam->yaw_goal;
        }
    }
    else
    {
        cam->flags |= 0x1;

        cam->tm_acc = tm - tm_acc;
        cam->tm_dec = tm_dec;
        cam->tmr = tm;

        s = 1.0f / -(0.5f * (tm_acc - tm_dec) - tm);

        cam->depv = s * (cam->dgoal - cam->dcur);
        cam->hepv = s * (cam->hgoal - cam->hcur);

        cam->pepv =
            (cam->dgoal + cam->dcur) *
            (0.5f * s * xDangleClamp(cam->pgoal - cam->pcur));
    }
}

void xCameraSetFOV(xCamera *cam, float fov)
{
    cam->fov = fov;
    iCameraSetFOV(cam->lo_cam, fov);
}