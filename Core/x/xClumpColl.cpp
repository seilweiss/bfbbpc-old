#include "xClumpColl.h"

#include "print.h"

xClumpCollBSPTree *xClumpColl_StaticBufferInit(void *data)
{
    unsigned int *header = (unsigned int *)data;
    unsigned int numBranchNodes;
    unsigned int numTriangles;
    xClumpCollBSPTree *tree;

    numBranchNodes = header[1];
    numTriangles = header[2];

    tree = (xClumpCollBSPTree *)RwMalloc(sizeof(xClumpCollBSPTree), rwMEMHINTDUR_GLOBAL);

    if (numBranchNodes)
    {
        tree->branchNodes = (xClumpCollBSPBranchNode *)(header + 3);
        tree->triangles = (xClumpCollBSPTriangle *)(tree->branchNodes + numBranchNodes);
    }
    else
    {
        tree->branchNodes = NULL;
        tree->triangles = (xClumpCollBSPTriangle *)(header + 3);
    }

    tree->numBranchNodes = numBranchNodes;
    tree->numTriangles = numTriangles;

    return tree;
}

void xClumpColl_InstancePointers(xClumpCollBSPTree *tree, RpClump *clump)
{
    BFBBSTUB("xClumpColl_InstancePointers");
    
    // nothing on gamecube
    // TODO: there is actual code here on PS2
    return;
}