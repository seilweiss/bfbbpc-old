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

#endif