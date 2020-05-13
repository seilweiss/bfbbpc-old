#ifndef XCLUMPCOLL_H
#define XCLUMPCOLL_H

#include <rwcore.h>
#include <rpworld.h>

struct xClumpCollBSPBranchNode
{
    unsigned int leftInfo;
    unsigned int rightInfo;
    float leftValue;
    float rightValue;
};

struct xClumpCollBSPVertInfo
{
    unsigned short atomIndex;
    unsigned short meshVertIndex;
};

struct xClumpCollBSPTriangle
{
    union
    {
        xClumpCollBSPVertInfo i;
        RwV3d *p;
    } v;
    unsigned char flags;
    unsigned char platData;
    unsigned short matIndex;
};

struct xClumpCollBSPTree
{
    unsigned int numBranchNodes;
    xClumpCollBSPBranchNode *branchNodes;
    unsigned int numTriangles;
    xClumpCollBSPTriangle *triangles;
};

xClumpCollBSPTree *xClumpColl_StaticBufferInit(void *data);
void xClumpColl_InstancePointers(xClumpCollBSPTree *tree, RpClump *clump);

#endif