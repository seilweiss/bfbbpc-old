#ifndef XJSP_H
#define XJSP_H

#include "xClumpColl.h"

#include <rwcore.h>
#include <rpworld.h>

struct xJSPNodeInfo
{
    int originalMatIndex;
    int nodeFlags;
};

struct xJSPHeader
{
    char idtag[4];
    unsigned int version;
    unsigned int jspNodeCount;
    RpClump *clump;
    xClumpCollBSPTree *colltree;
    xJSPNodeInfo *jspNodeList;
};

#endif