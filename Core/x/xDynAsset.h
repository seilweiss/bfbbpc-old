#ifndef XDYNASSET_H
#define XDYNASSET_H

#include "xBase.h"

struct xDynAsset : xBaseAsset
{
    unsigned int type;
    unsigned short version;
    unsigned short handle;
};

#endif