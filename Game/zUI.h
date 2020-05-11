#ifndef ZUI_H
#define ZUI_H

#include "zEnt.h"
#include "zScene.h"

struct zUIAsset : xEntAsset
{
	unsigned int uiFlags;
	unsigned short dim[2];
	unsigned int textureID;
	float uva[2];
	float uvb[2];
	float uvc[2];
	float uvd[2];
};

struct _zUI : zEnt
{
	zUIAsset *sasset;
	unsigned int uiFlags;
	unsigned int uiButton;
	unsigned short preUpdateIndex;
	unsigned short updateIndex;
};

struct xIniFile;

void zUI_Init();
void zUI_Init(void *ent, void *asset);
void zUI_ParseINI(xIniFile *ini);
void zUI_ScenePortalInit(zScene *zsc);

#endif