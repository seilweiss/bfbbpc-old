#ifndef ZLIGHTNING_H
#define ZLIGHTNING_H

#include "xMath3.h"
#include "iColor.h"

struct _tagLightningLine
{
    float unused;
};

struct _tagLightningRot
{
    float deg[16];
    float degrees;
    float height;
};

struct _tagLightningZeus
{
    float normal_offset;
    float back_offset;
    float side_offset;
};

struct zLightning
{
    unsigned int type;
    unsigned int flags;
    union
    {
        struct
        {
            xVec3 base_point[16];
            xVec3 point[16];
            short total_points;
            short end_points;
            float arc_height;
            xVec3 arc_normal;
            float thickness[16];
            union
            {
                _tagLightningLine line;
                _tagLightningRot rot;
                _tagLightningZeus zeus;
            };
            float rand_radius;
        } legacy;
        struct
        {
            xVec3 endPoint[2];
            xVec3 direction;
            float length;
            float scale;
            float width;
            float endParam[2];
            float endVel[2];
            float paramSpan[2];
            float arc_height;
            xVec3 arc_normal;
        } func;
    };
    iColor_tag color;
    float time_left;
    float time_total;
};

void zLightningInit();

#endif