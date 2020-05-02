#ifndef XSURFACE_H
#define XSURFACE_H

#include "xBase.h"

struct xEnt;

struct xSurface : xBase
{
    unsigned int idx;
    unsigned int type;
    union
    {
        unsigned int mat_idx;
        xEnt *ent;
        void *obj;
    };
    float friction;
    unsigned char state;
    unsigned char pad[3];
    void *moprops;
};

#endif