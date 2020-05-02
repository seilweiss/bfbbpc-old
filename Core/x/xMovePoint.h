#ifndef XMOVEPOINT_H
#define XMOVEPOINT_H

#include "xBase.h"
#include "xMath3.h"
#include "xSpline.h"

struct xMovePointAsset : xBaseAsset
{
    xVec3 pos;
    unsigned short wt;
    unsigned char on;
    unsigned char bezIndex;
    unsigned char flg_props;
    unsigned char pad;
    unsigned short numPoints;
    float delay;
    float zoneRadius;
    float arenaRadius;
};

struct xMovePoint : xBase
{
    xMovePointAsset *asset;
    xVec3 *pos;
    xMovePoint **nodes;
    xMovePoint *prev;
    unsigned int node_wt_sum;
    unsigned char on;
    unsigned char pad[2];
    float delay;
    xSpline3 *spl;
};

#endif