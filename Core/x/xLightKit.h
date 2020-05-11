#ifndef XLIGHTKIT_H
#define XLIGHTKIT_H

#include <rwcore.h>
#include <rpworld.h>

struct xLightKitLight
{
    unsigned int type;
    RwRGBAReal color;
    float matrix[16];
    float radius;
    float angle;
    RpLight *platLight;
};

struct xLightKit
{
    unsigned int tagID;
    unsigned int groupID;
    unsigned int lightCount;
    xLightKitLight *lightList;
};

xLightKit *xLightKit_Prepare(void *data);

#endif