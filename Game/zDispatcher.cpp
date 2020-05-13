#include "zDispatcher.h"

#include "print.h"

#include <stdlib.h>

void zDispatcher_Startup()
{
    return;
}

void zDispatcher_Shutdown()
{
    return;
}

void zDispatcher_scenePrepare()
{
    BFBBSTUB("zDispatcher_scenePrepare");
}

st_ZDISPATCH_DATA *zDispatcher_memPool(int cnt)
{
    BFBBSTUB("zDispatcher_memPool");
    return NULL;
}

st_ZDISPATCH_DATA *zDispatcher_getInst(st_ZDISPATCH_DATA *pool, int idx)
{
    BFBBSTUB("zDispatcher_getInst");
    return NULL;
}

void zDispatcher_Init(st_ZDISPATCH_DATA *dspdata, xBaseAsset *bass)
{
    BFBBSTUB("zDispatcher_Init");
}

void zDispatcher_InitDep(st_ZDISPATCH_DATA *dspdata, zScene *sc)
{
    BFBBSTUB("zDispatcher_InitDep");
}