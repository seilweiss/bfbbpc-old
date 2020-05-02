#ifndef XCLUMPCOLL_H
#define XCLUMPCOLL_H

#include <rwcore.h>

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

#endif