#ifndef XGROUP_H
#define XGROUP_H

#include "xBase.h"

struct xGroupAsset : xBaseAsset
{
    unsigned short itemCount;
    unsigned short groupFlags;
};

struct xGroup : xBase
{
    xGroupAsset *asset;
    xBase **item;
    unsigned int last_index;
    int flg_group;
};

void xGroupInit(void *b, void *asset);
unsigned int xGroupGetCount(xGroup *g);
void xGroupSetup(xGroup *g);
xBase *xGroupGetItemPtr(xGroup *g, unsigned int index);
unsigned int xGroupGetItem(xGroup *g, unsigned int index);

#endif