#include "zCameraTweak.h"

#include "xMath.h"
#include "iMath.h"
#include "zCamera.h"
#include "xMathInlines.h"

#include "print.h"

struct zCamTweak
{
	unsigned int owner;
	float priority;
	float time;
	float pitch;
	float distMult;
};

struct zCamTweakLook
{
	float h;
	float dist;
	float pitch;
};

static int sCamTweakCount;
static zCamTweak sCamTweakList[8];
static float sCamTweakLerp;
static float sCamTweakTime;
static float sCamTweakPitch[2];
static float sCamTweakDistMult[2];
static float sCamTweakPitchCur;
static float sCamTweakDistMultCur;
static float sCamD;
static float sCamH;
static float sCamPitch;
static zCamTweakLook zcam_neartweak;
static zCamTweakLook zcam_fartweak;

static void zCameraTweak_LookPreCalc(zCamTweakLook *look, float d, float h, float pitch)
{
	float f1 = d * itan(pitch);

	look->h = h - f1;
	look->dist = xsqrt(f1 * f1 + d * d);
	look->pitch = pitch;
}

void zCameraTweakGlobal_Init()
{
	zCameraTweak_LookPreCalc(&zcam_neartweak, zcam_near_d, zcam_near_h, zcam_near_pitch);
	zCameraTweak_LookPreCalc(&zcam_fartweak, zcam_far_d, zcam_far_h, zcam_far_pitch);

	zCameraTweakGlobal_Reset();
}

void zCameraTweakGlobal_Add(unsigned int owner, float priority, float time, float pitch,
							float distMult)
{
	int i, j;

	for (i = 0; i < sCamTweakCount; i++)
	{
		if (owner == sCamTweakList[i].owner)
		{
			return;
		}
	}

	for (i = 0; i < sCamTweakCount; i++)
	{
		if (priority >= sCamTweakList[i].priority)
		{
			for (j = sCamTweakCount; j >= (i + 1); j--)
			{
				sCamTweakList[j] = sCamTweakList[j - 1];
			}

			break;
		}
	}

	sCamTweakList[i].owner = owner;
	sCamTweakList[i].priority = priority;
	sCamTweakList[i].time = (time > 0.001f) ? time : 0.001f;
	sCamTweakList[i].pitch = DEG2RAD(pitch);
	sCamTweakList[i].distMult = (distMult > 0.001f) ? distMult : 0.001f;

	sCamTweakCount++;

	if (i == 0)
	{
		sCamTweakPitch[1] =
			sCamTweakPitch[1] * sCamTweakLerp +
			sCamTweakPitch[0] * (1.0f - sCamTweakLerp);

		sCamTweakDistMult[1] =
			sCamTweakDistMult[1] * sCamTweakLerp +
			sCamTweakDistMult[0] * (1.0f - sCamTweakLerp);

		sCamTweakTime = sCamTweakList[0].time;
		sCamTweakPitch[0] = sCamTweakList[0].pitch;
		sCamTweakDistMult[0] = sCamTweakList[0].distMult;
	}
}

void zCameraTweakGlobal_Remove(unsigned int owner)
{
	int i, j;

	for (i = 0; i < sCamTweakCount; i++)
	{
		if (owner == sCamTweakList[i].owner)
		{
			if (i == 0)
			{
				sCamTweakPitch[1] =
					sCamTweakPitch[1] * sCamTweakLerp +
					sCamTweakPitch[0] * (1.0f - sCamTweakLerp);

				sCamTweakDistMult[1] =
					sCamTweakDistMult[1] * sCamTweakLerp +
					sCamTweakDistMult[0] * (1.0f - sCamTweakLerp);

				sCamTweakLerp = 1.0f;
				sCamTweakTime = sCamTweakList[0].time;

				if (sCamTweakCount > 1)
				{
					sCamTweakPitch[0] = sCamTweakList[1].pitch;
					sCamTweakDistMult[0] = sCamTweakList[1].distMult;
				}
				else
				{
					sCamTweakPitch[0] = 0.0f;
					sCamTweakDistMult[0] = 1.0f;
				}
			}

			for (j = i; j < (sCamTweakCount - 1); j++)
			{
				sCamTweakList[j] = sCamTweakList[j + 1];
			}

			sCamTweakCount--;
			break;
		}
	}
}

void zCameraTweakGlobal_Reset()
{
	sCamTweakCount = 0;
	sCamTweakPitch[0] = 0.0f;
	sCamTweakPitch[1] = 0.0f;
	sCamTweakDistMult[0] = 1.0f;
	sCamTweakDistMult[1] = 1.0f;
	sCamTweakTime = 0.1f;
	sCamTweakLerp = 0.0f;
	sCamTweakPitchCur = 0.0f;
	sCamTweakDistMultCur = 1.0f;
}

void zCameraTweakGlobal_Update(float dt)
{
	zCamTweakLook *tlook;

	sCamTweakLerp -= dt / sCamTweakTime;

	if (sCamTweakLerp < 0.0f)
	{
		sCamTweakLerp = 0.0f;
	}

	tlook = &zcam_fartweak;

	sCamTweakPitchCur =
		sCamTweakPitch[1] * sCamTweakLerp +
		sCamTweakPitch[0] * (1.0f - sCamTweakLerp);

	sCamTweakDistMultCur =
		sCamTweakDistMult[1] * sCamTweakLerp +
		sCamTweakDistMult[0] * (1.0f - sCamTweakLerp);

	if (zcam_near)
	{
		tlook = &zcam_neartweak;
	}

	sCamD =
		sCamTweakDistMultCur * tlook->dist *
		icos(tlook->pitch + sCamTweakPitchCur);

	sCamH =
		sCamTweakDistMultCur * tlook->dist *
		isin(tlook->pitch + sCamTweakPitchCur) +
		tlook->h;

	sCamPitch = tlook->pitch + sCamTweakPitchCur;
}

float zCameraTweakGlobal_GetD()
{
	return sCamD;
}

float zCameraTweakGlobal_GetH()
{
	return sCamH;
}

float zCameraTweakGlobal_GetPitch()
{
	return sCamPitch;
}