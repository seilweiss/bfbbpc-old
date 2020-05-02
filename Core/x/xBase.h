#ifndef XBASE_H
#define XBASE_H

#include "xLinkAsset.h"

struct xBaseAsset
{
    unsigned int id;
    unsigned char baseType;
    unsigned char linkCount;
    unsigned short baseFlags;
};

struct xBase;

typedef int(*xBaseEventCallBack)(xBase *, xBase *, unsigned int, float *, xBase *);

struct xBase
{
    unsigned int id;
    unsigned char baseType;
    unsigned char linkCount;
    unsigned short baseFlags;
    xLinkAsset *link;
    xBaseEventCallBack eventFunc;
};

#endif