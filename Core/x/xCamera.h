#ifndef XCAMERA_H
#define XCAMERA_H

#include "xBase.h"
#include "xMath3.h"
#include "xBound.h"
#include "xScene.h"
#include "iCamera.h"

#include <rwcore.h>

struct xCamera : xBase
{
    RwCamera *lo_cam;
    xMat4x3 mat;
    xMat4x3 omat;
    xMat3x3 mbasis;
    xBound bound;
    xMat4x3 *tgt_mat;
    xMat4x3 *tgt_omat;
    xBound *tgt_bound;
    xVec3 focus;
    xScene *sc;
    xVec3 tran_accum;
    float fov;
    unsigned int flags;
    float tmr;
    float tm_acc;
    float tm_dec;
    float ltmr;
    float ltm_acc;
    float ltm_dec;
    float dmin;
    float dmax;
    float dcur;
    float dgoal;
    float hmin;
    float hmax;
    float hcur;
    float hgoal;
    float pmin;
    float pmax;
    float pcur;
    float pgoal;
    float depv;
    float hepv;
    float pepv;
    float orn_epv;
    float yaw_epv;
    float pitch_epv;
    float roll_epv;
    xQuat orn_cur;
    xQuat orn_goal;
    xQuat orn_diff;
    float yaw_cur;
    float yaw_goal;
    float pitch_cur;
    float pitch_goal;
    float roll_cur;
    float roll_goal;
    float dct;
    float dcd;
    float dccv;
    float dcsv;
    float hct;
    float hcd;
    float hccv;
    float hcsv;
    float pct;
    float pcd;
    float pccv;
    float pcsv;
    float orn_ct;
    float orn_cd;
    float orn_ccv;
    float orn_csv;
    float yaw_ct;
    float yaw_cd;
    float yaw_ccv;
    float yaw_csv;
    float pitch_ct;
    float pitch_cd;
    float pitch_ccv;
    float pitch_csv;
    float roll_ct;
    float roll_cd;
    float roll_ccv;
    float roll_csv;
    xVec4 frustplane[12];
};

extern int xcam_collis_owner_disable;
extern int xcam_do_collis;
extern float xcam_collis_radius;
extern float xcam_collis_stiffness;

void xCameraSetFOV(xCamera *cam, float fov);
void add_camera_tweaks();
void xCameraInit(xCamera *cam, unsigned int width, unsigned int height);
void xCameraMove(xCamera *cam, unsigned int flags, float dgoal, float hgoal, float pgoal,
                 float tm, float tm_acc, float tm_dec);
void xCameraReset(xCamera *cam, float d, float h, float pitch);
void xCameraSetScene(xCamera *cam, xScene *sc);

#endif