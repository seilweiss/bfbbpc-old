#include "xUpdateCull.h"

#include "print.h"

#include <stdlib.h>

unsigned int xUpdateCull_DistanceSquaredCB(void *ent, void *cbdata)
{
    BFBBSTUB("xUpdateCull_DistanceSquaredCB");
    return 1;
}

xUpdateCullMgr *xUpdateCull_Init(void **ent, unsigned int entCount, xGroup **group,
                                 unsigned int groupCount)
{
    BFBBSTUB("xUpdateCull_Init");
    return NULL;
}

void xUpdateCull_Update(xUpdateCullMgr *m, unsigned int percent_update)
{
    BFBBSTUB("xUpdateCull_Update");
}

void xUpdateCull_SetCB(xUpdateCullMgr *m, void *entity, xUpdateCullEntCallBack cb,
                       void *cbdata)
{
    BFBBSTUB("xUpdateCull_SetCB");
}