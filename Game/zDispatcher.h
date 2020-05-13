#ifndef ZDISPATCHER_H
#define ZDISPATCHER_H

#include "xBase.h"

struct st_ZDISPATCH_DATA : xBase
{
    xBaseAsset *rawass;
    int placeholder;
};

struct zScene;

void zDispatcher_Startup();
void zDispatcher_Shutdown();
void zDispatcher_scenePrepare();
st_ZDISPATCH_DATA *zDispatcher_memPool(int cnt);
st_ZDISPATCH_DATA *zDispatcher_getInst(st_ZDISPATCH_DATA *pool, int idx);
void zDispatcher_Init(st_ZDISPATCH_DATA *dspdata, xBaseAsset *bass);
void zDispatcher_InitDep(st_ZDISPATCH_DATA *dspdata, zScene *sc);

#endif