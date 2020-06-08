#ifndef XCOLLIDE_H
#define XCOLLIDE_H

#include "xModel.h"
#include "iMath3.h"
#include "xQuickCull.h"

struct xScene;

struct xCollis
{
    struct tri_data
    {
        unsigned int index;
        float r;
        float d;
    };

    unsigned int flags;
    unsigned int oid;
    void *optr;
    xModelInstance *mptr;
    float dist;
    xVec3 norm;
    xVec3 tohit;
    xVec3 depen;
    xVec3 hdng;
    union
    {
        struct
        {
            float t;
            float u;
            float v;
        } tuv;
        tri_data tri;
    };
};

struct xParabola
{
    xVec3 initPos;
    xVec3 initVel;
    float gravity;
    float minTime;
    float maxTime;
};

struct xSweptSphere
{
    xVec3 start;
    xVec3 end;
    float radius;
    float dist;
    xiMat4x3Union basis;
    xiMat4x3Union invbasis;
    xBox box;
    xQCData qcd;
    float boxsize;
    float curdist;
    xVec3 contact;
    xVec3 polynorm;
    unsigned int oid;
    void *optr;
    xModelInstance *mptr;
    int hitIt;
    xVec3 worldPos;
    xVec3 worldContact;
    xVec3 worldNormal;
    xVec3 worldTangent;
    xVec3 worldPolynorm;
};

struct xRay3
{
    xVec3 origin;
    xVec3 dir;
    float min_t;
    float max_t;
    int flags;
};

struct xEnv;

void xCollideInit(xScene *sc);
void xSweptSpherePrepare(xSweptSphere *sws, xVec3 *start, xVec3 *end, float radius);
int xSweptSphereToEnv(xSweptSphere *sws, xEnv *env);

#endif