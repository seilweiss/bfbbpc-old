#ifndef ZCAMERATWEAK_H
#define ZCAMERATWEAK_H

#include "xDynAsset.h"

struct CameraTweak_asset : xDynAsset
{
	int priority;
	float time;
	float pitch_adjust;
	float dist_adjust;
};

struct zCameraTweak : xBase
{
	CameraTweak_asset *casset;
};

void zCameraTweakGlobal_Reset();
void zCameraTweakGlobal_Init();
void zCameraTweakGlobal_Add(unsigned int owner, float priority, float time, float pitch,
							float distMult);
void zCameraTweakGlobal_Remove(unsigned int owner);
void zCameraTweakGlobal_Update(float dt);
float zCameraTweakGlobal_GetD();
float zCameraTweakGlobal_GetH();
float zCameraTweakGlobal_GetPitch();

#endif