#ifndef XCURVEASSET_H
#define XCURVEASSET_H

enum xCurveType
{
    xCVT_BAKED
};

enum xCurveClamp
{
    xCC_CONSTANT,
    xCC_CYCLE,
    xCC_OSCILLATE
};

struct xCurveAsset
{
    xCurveType type;
    xCurveClamp clamp;
    float delta;
    int numPoints;
    float *points;
};

#endif