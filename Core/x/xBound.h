#ifndef XBOUND_H
#define XBOUND_H

#include "xMath3.h"
#include "xQuickCull.h"

struct xBound
{
	xQCData qcd;
	unsigned char type;
	unsigned char pad[3];
	union
	{
		xSphere sph;
		xBBox box;
		xCylinder cyl;
	};
	xMat4x3 *mat;
};

#endif