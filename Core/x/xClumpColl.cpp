#include "xClumpColl.h"

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