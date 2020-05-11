#include "zCamera.h"

#include "xMath.h"
#include "zCameraTweak.h"

#include "print.h"

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
unsigned char input_enabled;
unsigned char lassocam_enabled;
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
	zcam_fovcurr = 75.0f;
	zcam_overrot_tmr = 0.0f;

	wall_jump_enabled = WJVS_DISABLED;
	lassocam_enabled = 0;
	stop_track = 0;

	zcam_mintgtheight = -1e+38f;

	xCameraSetFOV(cam, 75.0f);

	zCameraTweakGlobal_Update(0.0f);

	xCameraReset(cam, GetCurrentD(), GetCurrentH(), GetCurrentPitch());

	input_enabled = 1;

	dMultiplier = 1.0f;
	dOffset = 0.0f;
	hMultiplier = 1.0f;
	hOffset = 0.0f;
}