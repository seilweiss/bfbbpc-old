#ifndef XPARSYS_H
#define XPARSYS_H

#include "xBase.h"
#include "xParCmd.h"
#include "xParGroup.h"

#include <rwcore.h>

struct xParSysAsset : xBaseAsset
{
    unsigned int type;
    unsigned int parentParSysID;
    unsigned int textureID;
    unsigned char parFlags;
    unsigned char priority;
    unsigned short maxPar;
    unsigned char renderFunc;
    unsigned char renderSrcBlendMode;
    unsigned char renderDstBlendMode;
    unsigned char cmdCount;
    unsigned int cmdSize;
};

struct xParSys : xBase
{
    xParSysAsset *tasset;
    unsigned int cmdCount;
    xParCmd *cmd;
    xParSys *parent;
    xParGroup *group;
    unsigned char visible;
    RwTexture *txtr_particle;
};

void xParSysInit(void *b, void *tasset);
void xParSysSetup(xParSys *t);
void xParSysExit(xParSys *t);

#endif