#ifndef ZCUTSCENEMGR_H
#define ZCUTSCENEMGR_H

#include "xBase.h"
#include "xCutscene.h"

struct xCutsceneMgrAsset : xBaseAsset
{
	unsigned int cutsceneAssetID;
	unsigned int flags;
	float interpSpeed;
	float startTime[15];
	float endTime[15];
	unsigned int emitID[15];
};

struct xCutsceneZbuffer
{
	float start;
	float end;
	float nearPlane;
	float farPlane;
};

struct xCutsceneZbufferHack
{
	char *name;
	xCutsceneZbuffer times[4];
};

struct xCutsceneMgr : xBase
{
	xCutsceneMgrAsset *tasset;
	xCutscene *csn;
	unsigned int stop;
	xCutsceneZbufferHack *zhack;
	float oldfov;
};

struct zCutsceneMgr : xCutsceneMgr
{
};

#endif