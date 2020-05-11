#ifndef ILIGHT_H
#define ILIGHT_H

#include "xMath3.h"

#include <rwcore.h>
#include <rpworld.h>

struct _xFColor
{
    float r;
    float g;
    float b;
    float a;
};

struct iLight
{
    unsigned int type;
    RpLight *hw;
    xSphere sph;
    float radius_sq;
    _xFColor color;
    xVec3 dir;
    float coneangle;
};

#endif