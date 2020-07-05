#include "xAnim.h"

#include "print.h"

void xAnimInit()
{
    BFBBSTUB("xAnimInit");
}

void xAnimTempTransitionInit(unsigned int count)
{
    BFBBSTUB("xAnimTempTransitionInit");
}

xAnimState *xAnimTableGetState(xAnimTable *table, const char *name)
{
    BFBBSTUB("xAnimTableGetState");
    return NULL;
}

void xAnimPoolInit(xMemPool *pool, unsigned int count, unsigned int singles,
                   unsigned int blendFlags, unsigned int effectMax)
{
    BFBBSTUB("xAnimPoolInit");
}

xAnimPlay *xAnimPoolAlloc(xMemPool *pool, void *object, xAnimTable *table,
                          xModelInstance *modelInst)
{
    BFBBSTUB("xAnimPoolAlloc");
    return NULL;
}