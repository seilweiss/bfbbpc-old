#include "xCamera.h"

#include "xstransvc.h"
#include "xMath.h"
#include "xMathInlines.h"
#include "iMath.h"
#include "iSystem.h"
#include "zMain.h"
#include "zGrid.h"
#include "xScrFx.h"

#include "print.h"

#include <rwcore.h>
#include <rpworld.h>

static int sCamCollis;

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

    xCameraSetFOV(cam, CAMERA_FOV);

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

void xCameraExit(xCamera *cam)
{
    if (cam->lo_cam)
    {
        iCameraDestroy(cam->lo_cam);
        cam->lo_cam = NULL;
    }
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

static void xCam_buildbasis(xCamera *cam)
{
    if (cam->tgt_mat)
    {
        float d2d;

        float dx__ = cam->mat.pos.x - cam->tgt_mat->pos.x;
        float dz__ = cam->mat.pos.z - cam->tgt_mat->pos.z;

        float dist2 = xsqr(dx__) + xsqr(dz__);

        if (CLOSEENOUGH(dist2, 1.0f))
        {
            cam->mbasis.at.x = dx__;
            cam->mbasis.at.z = dz__;

            d2d = 1.0f;
        }
        else if (CLOSEENOUGH(dist2, 0.0f))
        {
            cam->mbasis.at.x = 0.0f;
            cam->mbasis.at.z = 0.0f;

            d2d = 0.0f;
        }
        else
        {
            d2d = xsqrt(dist2);

            float dist_inv = 1.0f / d2d;

            cam->mbasis.at.x = dx__ * dist_inv;
            cam->mbasis.at.z = dz__ * dist_inv;
        }

        if (d2d < 0.00001f)
        {
            cam->mbasis.at.x = cam->mat.at.x;
            cam->mbasis.at.z = cam->mat.at.z;

            float f1 = xsqrt(xsqr(cam->mbasis.at.x) + xsqr(cam->mbasis.at.z));

            if (f1 > 0.001f)
            {
                cam->mbasis.at.x *= 1.0f / f1;
                cam->mbasis.at.z *= 1.0f / f1;
            }
            else
            {
                cam->mbasis.at.x = isin(cam->pcur);
                cam->mbasis.at.z = icos(cam->pcur);
            }
        }

        cam->mbasis.at.y = 0.0f;

        cam->mbasis.up.x = 0.0f;
        cam->mbasis.up.y = 1.0f;
        cam->mbasis.up.z = 0.0f;

        cam->mbasis.right.x = cam->mbasis.at.z;
        cam->mbasis.right.y = 0.0f;
        cam->mbasis.right.z = -cam->mbasis.at.x;
    }
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

static void xCam_worldtocyl(float &d, float &h, float &p, const xMat4x3 *tgt_mat,
                            const xVec3 *v, unsigned int flags)
{
    float lx;
    float lz;

    float dx__ = v->x - tgt_mat->pos.x;
    float dz__ = v->z - tgt_mat->pos.z;

    float dist2 = xsqr(dx__) + xsqr(dz__);

    if (CLOSEENOUGH(dist2, 1.0f))
    {
        d = 1.0f;

        lx = dx__;
        lz = dz__;
    }
    else if (CLOSEENOUGH(dist2, 0.0f))
    {
        d = 0.0f;

        lx = 0.0f;
        lz = 0.0f;
    }
    else
    {
        d = xsqrt(dist2);

        float dist_inv = 1.0f / d;

        lx = dx__ * dist_inv;
        lz = dz__ * dist_inv;
    }

    if (flags & 0x10)
    {
        h = v->y;
    }
    else
    {
        h = v->y - tgt_mat->pos.y;
    }

    p = xatan2(lx, lz);

    if (!(flags & 0x20))
    {
        float tgt_p = xatan2(tgt_mat->at.x, tgt_mat->at.z);

        p = xDangleClamp(p - tgt_p);
    }
}

static void xCam_CorrectD(xCamera *cam, float dtg, float dcv, float dt)
{
    float f2 = ((10.0f / 7.0f) * (2.0f * dtg - dcv * dt) - dcv) * dt;

    cam->mat.pos.x += cam->mbasis.at.x * f2;
    cam->mat.pos.z += cam->mbasis.at.z * f2;
}

static void xCam_CorrectH(xCamera *cam, float htg, float hcv, float dt)
{
    float f2 = 0.15f * hcv;
    float f1 = ((10.0f / 7.0f) * -(f2 * dt - htg) - f2) * dt;
    
    cam->mat.pos.y += f1;
}

static void xCam_CorrectP(xCamera *cam, float ptg, float pcv, float dt)
{
    float f8 = 0.15f * pcv;
    float f1 = (2.5f * (2.0f * ptg - f8 * dt) - f8) * dt;

    cam->mat.pos.x += cam->mbasis.right.x * f1;
    cam->mat.pos.z += cam->mbasis.right.z * f1;
}

static void xCam_DampP(xCamera *cam, float pcv, float dt)
{
    float f2 = -6.0f * pcv * dt * dt;

    cam->mat.pos.x += cam->mbasis.right.x * f2;
    cam->mat.pos.z += cam->mbasis.right.z * f2;
}

static void xCam_CorrectYaw(xCamera *cam, float ytg, float ycv, float dt)
{
    float f1 = ((1.0f / cam->yaw_ct) *
        (2.0f * cam->yaw_cd * ytg - ycv * dt) - ycv) * (cam->yaw_csv * dt);

    cam->yaw_cur += f1;
}

static void xCam_CorrectPitch(xCamera *cam, float ptg, float pcv, float dt)
{
    float f1 = ((1.0f / cam->pitch_ct) *
        (2.0f * cam->pitch_cd * ptg - pcv * dt) - pcv) * (cam->pitch_csv * dt);

    cam->pitch_cur += f1;
}

static void xCam_CorrectRoll(xCamera *cam, float rtg, float rcv, float dt)
{
    float f1 = ((1.0f / cam->roll_ct) *
        (2.0f * cam->roll_cd * rtg - rcv * dt) - rcv) * (cam->roll_csv * dt);

    cam->roll_cur += f1;
}

void SweptSphereHitsCameraEnt(xScene *sc, xRay3 *ray, xQCData *qcd, xEnt *ent,
                              void *data)
{
    BFBBSTUB("SweptSphereHitsCameraEnt");
}

static void _xCameraUpdate(xCamera *cam, float dt)
{
    if (cam->tgt_mat)
    {
        static float last_dt = ONE_FRAME;

        xCam_worldtocyl(cam->dcur, cam->hcur, cam->pcur,
                        cam->tgt_mat, &cam->mat.pos, cam->flags);

        float wcvx = cam->mat.pos.x - cam->omat.pos.x; // f26
        float wcvy = cam->mat.pos.y - cam->omat.pos.y; // f27
        float wcvz = cam->mat.pos.z - cam->omat.pos.z; // f28

        float m = 1.0f / last_dt; // f29

        wcvx *= m;
        wcvy *= m;
        wcvz *= m;

        cam->omat.pos = cam->mat.pos;

        xCam_buildbasis(cam);

        float dcv = wcvx * cam->mbasis.at.x + wcvz * cam->mbasis.at.z; // f30
        float hcv = wcvy; // f25
        float pcv = wcvx * cam->mbasis.right.x + wcvz * cam->mbasis.right.z; // f24

        wcvx *= dt;
        wcvy *= dt;
        wcvz *= dt;

        cam->mat.pos.x += wcvx;
        cam->mat.pos.y += wcvy;
        cam->mat.pos.z += wcvz;

        if (cam->flags & 0x1)
        {
            float tnext = cam->tmr - dt;

            if (tnext <= 0.0f)
            {
                cam->flags &= ~0x1;
                cam->tmr = 0.0f;
                cam->omat.pos = cam->mat.pos;
            }
            else
            {
                float dtg = cam->dgoal - cam->dcur;
                float htg = cam->hgoal - cam->hcur;
                float ptg = cam->pgoal - cam->pcur;

                ptg = 0.5f * xDangleClamp(ptg) * (cam->dgoal + cam->dcur);

                float dsv;
                float hsv;
                float psv;

                if (tnext <= cam->tm_dec)
                {
                    float T_inv = 1.0f / cam->tmr;

                    dsv = T_inv * (2.0f * dtg - dcv * dt);
                    hsv = T_inv * (2.0f * htg - hcv * dt);
                    psv = T_inv * (2.0f * ptg - pcv * dt);
                }
                else if (tnext <= cam->tm_acc)
                {
                    float T_inv = 1.0f / (2.0f * cam->tmr - dt - cam->tm_dec);

                    dsv = T_inv * (2.0f * dtg - dcv * dt);
                    hsv = T_inv * (2.0f * htg - hcv * dt);
                    psv = T_inv * (2.0f * ptg - pcv * dt);
                }
                else
                {
                    float it = 2.0f / (cam->tmr + cam->tm_acc - cam->tm_dec);
                    float ot = cam->tm_acc + (cam->tmr - dt) - cam->tm_dec;

                    float T_inv = 1.0f / (cam->tmr - cam->tm_acc);

                    dsv = T_inv * -(dcv * dt -
                        (2.0f * dtg - ((dtg * it + cam->depv) * 0.5f * ot)));
                    hsv = T_inv * -(hcv * dt -
                        (2.0f * htg - ((htg * it + cam->hepv) * 0.5f * ot)));
                    psv = T_inv * -(pcv * dt -
                        (2.0f * ptg - ((ptg * it + cam->pepv) * 0.5f * ot)));
                }

                float dpv = dsv - dcv;
                float hpv = hsv - hcv;
                float ppv = psv - pcv;

                float vax = (cam->mbasis.right.x * ppv + cam->mbasis.at.x * dpv) * dt;
                float vay = (cam->mbasis.right.y * ppv + hpv) * dt;
                float vaz = (cam->mbasis.right.z * ppv + cam->mbasis.at.z * dpv) * dt;

                cam->mat.pos.x += vax;
                cam->mat.pos.y += vay;
                cam->mat.pos.z += vaz;

                cam->tmr = tnext;
            }
        }
        else
        {
            if (cam->flags & 0x2)
            {
                if (!CLOSEENOUGH(cam->dcur / cam->dgoal, 1.0f))
                {
                    float dtg = cam->dgoal - cam->dcur;
                    xCam_CorrectD(cam, dtg, dcv, dt);
                }
            }
            else if (cam->dmax > cam->dmin)
            {
                if (cam->dcur < cam->dmin)
                {
                    float dtg = cam->dmin - cam->dcur;
                    xCam_CorrectD(cam, dtg, dcv, dt);
                }
                else if (cam->dcur > cam->dmax)
                {
                    float dtg = cam->dmax - cam->dcur;
                    xCam_CorrectD(cam, dtg, dcv, dt);
                }
            }

            if (cam->flags & 0x4)
            {
                if (!CLOSEENOUGH(cam->hcur / cam->hgoal, 1.0f))
                {
                    float htg = cam->hgoal - cam->hcur;
                    xCam_CorrectH(cam, htg, hcv, dt);
                }
            }
            else if (cam->hmax > cam->hmin)
            {
                if (cam->hcur < cam->hmin)
                {
                    float htg = cam->hmin - cam->hcur;
                    xCam_CorrectH(cam, htg, hcv, dt);
                }
                else if (cam->hcur > cam->hmax)
                {
                    float htg = cam->hmax - cam->hcur;
                    xCam_CorrectH(cam, htg, hcv, dt);
                }
            }

            if (cam->flags & 0x8)
            {
                if (!CLOSEENOUGH(cam->pcur / cam->pgoal, 1.0f))
                {
                    float ptg = cam->dcur * xDangleClamp(cam->pgoal - cam->pcur);
                    xCam_CorrectP(cam, ptg, pcv, dt);
                }
            }
            else if (cam->pmax > cam->pmin)
            {
                float dphi = xDangleClamp(cam->pmax - cam->pcur);
                float dplo = xDangleClamp(cam->pmin - cam->pcur);

                if (dplo > 0.0f && (dphi > 0.0f || iabs(dplo) <= iabs(dphi)))
                {
                    float ptg = (dplo + EPSILON) * cam->dcur;
                    xCam_CorrectP(cam, ptg, pcv, dt);
                }
                else if (dphi < 0.0f)
                {
                    float ptg = (dphi - EPSILON) * cam->dcur;
                    xCam_CorrectP(cam, ptg, pcv, dt);
                }
                else
                {
                    xCam_DampP(cam, pcv, dt);
                }
            }
            else
            {
                xCam_DampP(cam, pcv, dt);
            }
        }

        if (cam->flags & 0x80)
        {
            xVec3 oeu;
            xVec3 eu;

            xMat3x3GetEuler(&cam->mat, &oeu);
            xMat3x3GetEuler(&cam->omat, &eu);

            float m = 1.0f / last_dt; // f22

            float ycv = m * xDangleClamp(oeu.x - eu.x); // f24
            float pcv = m * xDangleClamp(oeu.y - eu.y); // f25
            float rcv = m * xDangleClamp(oeu.z - eu.z); // f23

            ycv *= cam->yaw_ccv;
            pcv *= cam->pitch_ccv;
            rcv *= cam->roll_ccv;

            cam->omat = cam->mat;

            //float f28 = ycv * dt;
            //float f27 = pcv * dt;
            //float f26 = rcv * dt;

            cam->yaw_cur += ycv * dt;
            cam->pitch_cur += pcv * dt;
            cam->roll_cur += rcv * dt;

            if (cam->flags & 0x40)
            {
                float tnext = cam->ltmr - dt; // f22

                if (tnext <= 0.0f)
                {
                    cam->flags &= ~0x40;
                    cam->ltmr = 0.0f;
                }
                else
                {
                    float ytg = xDangleClamp(cam->yaw_goal - cam->yaw_cur); // f29
                    float ptg = xDangleClamp(cam->pitch_goal - cam->pitch_cur); // f30
                    float rtg = xDangleClamp(cam->roll_goal - cam->roll_cur); // f1

                    float ysv;
                    float psv;
                    float rsv;

                    if (tnext <= cam->ltm_dec)
                    {
                        float T_inv = 1.0f / cam->ltmr;

                        ysv = T_inv * (2.0f * ytg - ycv * dt);
                        psv = T_inv * (2.0f * ptg - pcv * dt);
                        rsv = T_inv * (2.0f * rtg - rcv * dt);
                    }
                    else if (tnext <= cam->ltm_acc)
                    {
                        float T_inv = 1.0f / (2.0f * cam->ltmr - dt - cam->ltm_dec);

                        ysv = T_inv * (2.0f * ytg - ycv * dt);
                        psv = T_inv * (2.0f * ptg - pcv * dt);
                        rsv = T_inv * (2.0f * rtg - rcv * dt);
                    }
                    else
                    {
                        float it = 2.0f / (cam->ltmr + cam->ltm_acc - cam->ltm_dec);
                        float ot = cam->ltmr - dt + cam->ltm_acc - cam->ltm_dec;

                        float T_inv = 1.0f / (cam->ltmr - cam->ltm_acc);

                        ysv = T_inv * (2.0f * ytg -
                            ((ytg * it + cam->yaw_epv) * 0.5f * ot) - ycv * dt);
                        psv = T_inv * (2.0f * ptg -
                            ((ptg * it + cam->pitch_epv) * 0.5f * ot) - pcv * dt);
                        rsv = T_inv * (2.0f * rtg -
                            ((rtg * it + cam->roll_epv) * 0.5f * ot) - rcv * dt);
                    }

                    float ypv = ysv - ycv;
                    float ppv = psv - pcv;
                    float rpv = rsv - rcv;

                    float vax = ypv * dt;
                    float vay = ppv * dt;
                    float vaz = rpv * dt;

                    cam->yaw_cur += vax;
                    cam->pitch_cur += vay;
                    cam->roll_cur += vaz;

                    xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);

                    cam->ltmr = tnext;
                }
            }
            else
            {
                if (!CLOSEENOUGH(cam->yaw_cur, cam->yaw_goal))
                {
                    float ytg = xDangleClamp(cam->yaw_goal - cam->yaw_cur);
                    xCam_CorrectYaw(cam, ytg, ycv, dt);
                }

                if (!CLOSEENOUGH(cam->pitch_cur, cam->pitch_goal))
                {
                    float ptg = xDangleClamp(cam->pitch_goal - cam->pitch_cur);
                    xCam_CorrectPitch(cam, ptg, pcv, dt);
                }

                if (!CLOSEENOUGH(cam->roll_cur, cam->roll_goal))
                {
                    float rtg = xDangleClamp(cam->roll_goal - cam->roll_cur);
                    xCam_CorrectRoll(cam, rtg, rcv, dt);
                }

                xMat3x3Euler(&cam->mat, cam->yaw_cur, cam->pitch_cur, cam->roll_cur);
            }
        }
        else
        {
            xQuatFromMat(&cam->orn_cur, &cam->mat);

            xQuat oq;
            xQuatFromMat(&oq, &cam->omat);

            xQuat qdiff_o_c;
            xQuatDiff(&qdiff_o_c, &oq, &cam->orn_cur);

            xRot rot_cv;
            xQuatToAxisAngle(&qdiff_o_c, &rot_cv.axis, &rot_cv.angle);

            rot_cv.angle *= m; // this ages well
            rot_cv.angle = 0.0f;

            cam->omat = cam->mat;

            xVec3 f;
            xMat3x3RMulVec(&f, cam->tgt_mat, &cam->focus);
            xVec3AddTo(&f, &cam->tgt_mat->pos);

            float atx = cam->tgt_mat->pos.x - cam->mat.pos.x; // f22
            float aty;
            float atz = cam->tgt_mat->pos.z - cam->mat.pos.z; // f23

            float dist = xsqr(atx) + xsqr(atz);

            float dx__;
            float dz__;

            if (CLOSEENOUGH(dist, 1.0f))
            {
                dx__ = atx;
                dz__ = atz;
            }
            else if (CLOSEENOUGH(dist, 0.0f))
            {
                dx__ = 0.0f;
                dz__ = 0.0f;
            }
            else
            {
                float dist2 = xsqrt(dist);
                float dist_inv = 1.0f / xsqrt(dist2);

                dx__ = atx * dist_inv;
                dz__ = atz * dist_inv;
            }

            float mpx = cam->tgt_mat->at.x * dx__;
            float mpy = cam->tgt_mat->at.y * 0.0f;
            float mpz = cam->tgt_mat->at.z * dz__;

            if ((mpx + mpy + mpz) < 0.0f)
            {
                float s = -2.0f *
                    ((f.x - cam->tgt_mat->pos.x) * dx__ +
                    (f.y - cam->tgt_mat->pos.y) * 0.0f +
                    (f.z - cam->tgt_mat->pos.z) * dz__);

                f.x += dx__ * s;
                f.y += 0.0f * s;
                f.z += dz__ * s;
            }

            xMat3x3 des_mat;
            xMat3x3LookAt(&des_mat, &f, &cam->mat.pos);

            xMat3x3 latgt;
            xMat3x3LookAt(&latgt, &cam->tgt_mat->pos, &cam->mat.pos);

            float ang_dist = xacos(
                latgt.at.x * des_mat.at.x +
                latgt.at.y * des_mat.at.y +
                latgt.at.z * des_mat.at.z); // f22

            if (ang_dist > DEG2RAD(30.0f))
            {
                xQuat a;
                xQuatFromMat(&a, &latgt);

                xQuat b;
                xQuatFromMat(&b, &des_mat);

                xQuat o;

                float s = PI - ang_dist;

                if (s < (PI / 2))
                {
                    if (s > DEG2RAD(5.0f))
                    {
                        xQuatSlerp(&o, &a, &b, s / ang_dist);
                    }
                    else
                    {
                        o = a;
                    }
                }
                else
                {
                    xQuatSlerp(&o, &a, &b, DEG2RAD(30.0f) / ang_dist);
                }

                xQuatToMat(&o, &des_mat);
            }

            xQuat desq;
            xQuatFromMat(&desq, &des_mat);

            xCameraLook(cam, 0, &desq, 0.25f, 0.0f, 0.0f);

            xQuat difq;
            xQuatConj(&difq, &cam->orn_cur);
            xQuatMul(&difq, &difq, &desq);

            xQuat newq;
            xQuatSlerp(&newq, &cam->orn_cur, &desq, 25.5f * dt);
            xQuatToMat(&newq, &cam->mat);
        }

        if (xcam_do_collis && sCamCollis)
        {
            xSweptSphere sws;

            xVec3 tgtpos;
            tgtpos.x = cam->tgt_mat->pos.x;
            tgtpos.y = 0.7f + cam->tgt_mat->pos.y;
            tgtpos.z = cam->tgt_mat->pos.z;

            xSweptSpherePrepare(&sws, &tgtpos, &cam->mat.pos, 0.07f);
            xSweptSphereToEnv(&sws, globals.sceneCur->env);

            xRay3 ray;
            xVec3Copy(&ray.origin, &sws.start);
            xVec3Sub(&ray.dir, &sws.end, &sws.start);

            ray.max_t = xVec3Length(&ray.dir);

            float one_len = 1.0f / xmax(ray.max_t, EPSILON);

            xVec3SMul(&ray.dir, &ray.dir, one_len);

            ray.flags = 0x800;

            if (!(ray.flags & 0x400))
            {
                ray.flags |= 0x400;
                ray.min_t = 0.0f;
            }

            xRayHitsGrid(&colls_grid, globals.sceneCur, &ray,
                         SweptSphereHitsCameraEnt, &sws.qcd, &sws);
            xRayHitsGrid(&colls_oso_grid, globals.sceneCur, &ray,
                         SweptSphereHitsCameraEnt, &sws.qcd, &sws);

            if (sws.curdist != sws.dist)
            {
                float stopdist = xmax(sws.curdist, 0.6f);

                cam->mat.pos.x = stopdist * ray.dir.x + ray.origin.x;
                cam->mat.pos.y = stopdist * ray.dir.y + ray.origin.y;
                cam->mat.pos.z = stopdist * ray.dir.z + ray.origin.z;
            }
        }

        last_dt = dt;

        iCameraUpdatePos(cam->lo_cam, &cam->mat);
    }
}

void xCameraUpdate(xCamera *cam, float dt)
{
    BFBBSTUB("xCameraUpdate");

    int i;
    int num_updates = (int)xceil(144.0f * dt);
    float sdt = dt / num_updates;

    for (i = 0; i < num_updates; i++)
    {
        //sCamCollis = (i == num_updates - 1);

        _xCameraUpdate(cam, sdt);
    }
}

void xCameraBegin(xCamera *cam, int clear)
{
    iCameraBegin(cam->lo_cam, clear);
    iCameraFrustumPlanes(cam->lo_cam, cam->frustplane);
    iCameraUpdateFog(cam->lo_cam, 0);
}

static void xCameraFXInit()
{
    BFBBSTUB("xCameraFXInit");
}

void xCameraEnd(xCamera *cam, float seconds, int update_scrn_fx)
{
    if (update_scrn_fx)
    {
        xScrFxUpdate(cam->lo_cam, seconds);
    }

    iCameraEnd(cam->lo_cam);
}

void xCameraShowRaster(xCamera *cam)
{
    iCameraShowRaster(cam->lo_cam);
}

void xCameraSetScene(xCamera *cam, xScene *sc)
{
    cam->sc = sc;

    iCameraAssignEnv(cam->lo_cam, sc->env->geom);
}

void xCameraSetTargetMatrix(xCamera *cam, xMat4x3 *mat)
{
    cam->tgt_mat = mat;
}

void xCameraSetTargetOMatrix(xCamera *cam, xMat4x3 *mat)
{
    cam->tgt_omat = mat;
}

void xCameraMove(xCamera *cam, unsigned int flags, float dgoal, float hgoal, float pgoal,
                 float tm, float tm_acc, float tm_dec)
{
    float s;

    cam->flags = (cam->flags & ~0x3E) | (flags & 0x3E);
    cam->dgoal = dgoal;
    cam->hgoal = hgoal;
    cam->pgoal = pgoal;

    if (tm <= 0.0f)
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

void xCameraMove(xCamera *cam, const xVec3 &loc, float maxSpeed)
{
    xVec3 v;
    float speed;

    xVec3Sub(&v, &loc, &cam->mat.pos);

    speed = xVec3Length(&v);

    if (speed > maxSpeed)
    {
        xVec3SMul(&v, &v, maxSpeed / speed);
        xVec3Add(&cam->mat.pos, &cam->mat.pos, &v);
    }
    else
    {
        cam->mat.pos = loc;
    }

    cam->omat.pos = cam->mat.pos;
    cam->flags &= ~0x3E;
    cam->tmr = 0.0f;
    cam->tm_dec = 0.0f;
    cam->tm_acc = 0.0f;
}

void xCameraFOV(xCamera *cam, float fov, float maxSpeed, float dt)
{
    float speed = maxSpeed * dt;
    float currentFOV = xCameraGetFOV(cam);

    if (currentFOV != fov)
    {
        if (speed != 0.0f)
        {
            xCameraSetFOV(cam, fov);
        }
        else
        {
            float len = fov - currentFOV;

            if (iabs(len) > speed)
            {
                xCameraSetFOV(cam, currentFOV + len * (speed / len));
            }
            else
            {
                xCameraSetFOV(cam, fov);
            }
        }
    }
}

void xCameraLook(xCamera *cam, unsigned int flags, const xQuat *orn_goal,
                 float tm, float tm_acc, float tm_dec)
{
    cam->flags = (cam->flags & ~0xF80) | (flags & 0xF80);

    cam->orn_goal = *orn_goal;

    if (tm <= 0.0f)
    {
        if (cam->tgt_mat)
        {
            xQuatToMat(orn_goal, &cam->mat);

            cam->omat = cam->mat;
        }
    }
    else
    {
        cam->flags |= 0x40;

        cam->ltm_acc = tm - tm_acc;
        cam->ltm_dec = tm_dec;
        cam->ltmr = tm;

        xQuatDiff(&cam->orn_diff, &cam->orn_cur, orn_goal);

        float s = 1.0f / -(0.5f * (tm_acc - tm_dec) - tm);

        cam->orn_epv = s * xQuatGetAngle(&cam->orn_diff);
    }
}

void xCameraLookYPR(xCamera *cam, unsigned int flags, float yaw, float pitch, float roll,
                    float tm, float tm_acc, float tm_dec)
{
    cam->flags = (cam->flags & ~0xF80) | (flags & 0xF80) | 0x80;

    cam->yaw_goal = yaw;
    cam->pitch_goal = pitch;
    cam->roll_goal = roll;

    if (tm <= 0.0f)
    {
        if (cam->tgt_mat)
        {
            cam->yaw_cur = yaw;
            cam->pitch_cur = pitch;
            cam->roll_cur = roll;

            xMat3x3Euler(&cam->mat, yaw, pitch, roll);

            cam->omat = cam->mat;
        }
    }
    else
    {
        cam->flags |= 0x40;

        cam->ltm_acc = tm - tm_acc;
        cam->ltm_dec = tm_dec;
        cam->ltmr = tm;

        float s = 1.0f / -(0.5f * (tm_acc - tm_dec) - tm);

        cam->yaw_epv = s * xDangleClamp(yaw - cam->yaw_cur);
        cam->pitch_epv = s * xDangleClamp(pitch - cam->pitch_cur);
        cam->roll_epv = s * xDangleClamp(roll - cam->roll_cur);
    }
}

void xCameraRotate(xCamera *cam, const xVec3 &v, float roll, float time, float accel,
                   float decl)
{
    cam->yaw_goal = xatan2(v.x, v.z);
    cam->pitch_goal = -xasin(v.y);
    cam->roll_goal = roll;

    if (0.0f == time)
    {
        cam->yaw_cur = cam->yaw_goal;
        cam->pitch_cur = cam->pitch_goal;
        cam->roll_cur = cam->roll_goal;
    }

    cam->flags = (cam->flags & ~0xF80) | 0x80;

    xMat3x3Euler(&cam->mat, cam->yaw_goal, cam->pitch_goal, cam->roll_goal);

    if (0.0f == time)
    {
        cam->omat = cam->mat;
    }

    if (0.0f == time)
    {
        cam->ltmr = 0.0f;
        cam->ltm_dec = 0.0f;
        cam->ltm_acc = 0.0f;
    }
    else
    {
        cam->ltm_acc = accel;
        cam->ltm_dec = decl;
        cam->ltmr = time;
    }

    cam->roll_epv = 0.0f;
    cam->pitch_epv = 0.0f;
    cam->yaw_epv = 0.0f;
}

void xCameraSetFOV(xCamera *cam, float fov)
{
    cam->fov = fov;
    iCameraSetFOV(cam->lo_cam, fov);
}

float xCameraGetFOV(const xCamera *cam)
{
    return cam->fov;
}