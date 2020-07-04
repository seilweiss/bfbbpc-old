#ifndef XBOUND_H
#define XBOUND_H

#include "xMath3.h"
#include "xQuickCull.h"

#define XBOUND_TYPE_UNK0 0
#define XBOUND_TYPE_UNK1 1
#define XBOUND_TYPE_UNK4 4

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