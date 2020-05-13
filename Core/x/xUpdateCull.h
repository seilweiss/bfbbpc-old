#ifndef XUPDATECULL_H
#define XUPDATECULL_H

#include "xGroup.h"

typedef unsigned int(*xUpdateCullEntCallBack)(void *ent, void *cbdata);

struct xUpdateCullEnt
{
    unsigned short index;
    short groupIndex;
    xUpdateCullEntCallBack cb;
    void *cbdata;
    xUpdateCullEnt *nextInGroup;
};

struct xUpdateCullGroup
{
    unsigned int active;
    unsigned short startIndex;
    unsigned short endIndex;
    xGroup *groupObject;
};

typedef void(*xUpdateCullMgrCallBack)(void *);

struct xUpdateCullMgr
{
    unsigned int entCount;
    unsigned int entActive;
    void **ent;
    xUpdateCullEnt **mgr;
    unsigned int mgrCount;
    unsigned int mgrCurr;
    xUpdateCullEnt *mgrList;
    unsigned int grpCount;
    xUpdateCullGroup *grpList;
    xUpdateCullMgrCallBack activateCB;
    xUpdateCullMgrCallBack deactivateCB;
};

unsigned int xUpdateCull_DistanceSquaredCB(void *ent, void *cbdata);

xUpdateCullMgr *xUpdateCull_Init(void **ent, unsigned int entCount, xGroup **group,
                                 unsigned int groupCount);
void xUpdateCull_Update(xUpdateCullMgr *m, unsigned int percent_update);
void xUpdateCull_SetCB(xUpdateCullMgr *m, void *entity, xUpdateCullEntCallBack cb,
                       void *cbdata);

#endif