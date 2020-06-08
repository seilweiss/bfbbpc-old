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

struct xMgr
{
};

struct zUIMgr : xMgr
{
    int m_preUpdateStart;
    int m_preUpdateEnd;
    unsigned int m_preUpdateMax;
    _zUI **m_preUpdate;
    int m_updateStart;
    int m_updateEnd;
    unsigned int m_updateMax;
    _zUI **m_update;

    void Setup(zScene *s);
};

extern zUIMgr gUIMgr;

struct xIniFile;

void zUI_Init();
void zUI_Init(void *ent, void *asset);
void zUIRenderAll();
void zUI_ParseINI(xIniFile *ini);
void zUI_ScenePortalInit(zScene *zsc);

#endif