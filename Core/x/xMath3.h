#ifndef XMATH3_H
#define XMATH3_H

#include "xVec3.h"
#include "xVec3Inlines.h"

#include <string.h>

struct xVec4
{
    float x;
    float y;
    float z;
    float w;
};

struct xMat3x3
{
    xVec3 right;
    int flags;
    xVec3 up;
    unsigned int pad1;
    xVec3 at;
    unsigned int pad2;
};

struct xMat4x3 : xMat3x3
{
    xVec3 pos;
    unsigned int pad3;
};

struct xQuat
{
    xVec3 v;
    float s;
};

struct xRot
{
    xVec3 axis;
    float angle;
};

struct xSphere
{
    xVec3 center;
    float r;
};

struct xBox
{
    xVec3 upper;
    xVec3 lower;
};

struct xBBox
{
    xVec3 center;
    xBox box;
};

struct xCylinder
{
    xVec3 center;
    float r;
    float h;
};

extern xVec3 g_O3;
extern xVec3 g_X3;
extern xVec3 g_Y3;
extern xVec3 g_Z3;
extern xVec3 g_NX3;
extern xVec3 g_NY3;
extern xVec3 g_NZ3;
extern xVec3 g_Onez;
extern xMat4x3 g_I3;
extern xQuat g_IQ;

void xMath3Init();
void xMat3x3GetEuler(const xMat3x3 *m, xVec3 *a);
void xMat4x3MoveLocalRight(xMat4x3 *m, float mag);
void xMat4x3MoveLocalUp(xMat4x3 *m, float mag);
void xMat4x3MoveLocalAt(xMat4x3 *m, float mag);
float xMat3x3LookVec(xMat3x3 *m, const xVec3 *at);
void xMat3x3Euler(xMat3x3 *m, const xVec3 *ypr);
void xMat3x3Euler(xMat3x3 *m, float yaw, float pitch, float roll);
void xQuatFromMat(xQuat *q, const xMat3x3 *m);
void xQuatToMat(const xQuat *q, xMat3x3 *m);
void xQuatToAxisAngle(const xQuat *q, xVec3 *a, float *t);
void xQuatSMul(xQuat *o, const xQuat *q, float s);
float xQuatNormalize(xQuat *o, const xQuat *q);
void xQuatSlerp(xQuat *o, const xQuat *a, const xQuat *b, float t);
void xQuatMul(xQuat *o, const xQuat *a, const xQuat *b);
void xQuatDiff(xQuat *o, const xQuat *a, const xQuat *b);

inline void xMat3x3LookAt(xMat3x3 *o, const xVec3 *from, const xVec3 *to)
{
    xVec3 at;

    xVec3Sub(&at, to, from);
    xMat3x3LookVec(o, &at);
}

inline void xMat3x3RMulVec(xVec3 *o, const xMat3x3 *m, const xVec3 *v)
{
    //float y;
    //float z;

    o->x = m->right.x * v->x + m->up.x * v->y + m->at.x * v->z;
    o->y = m->right.y * v->x + m->up.y * v->y + m->at.y * v->z;
    o->z = m->right.z * v->x + m->up.z * v->y + m->at.z * v->z;
}

inline void xMat4x3Copy(xMat4x3 *o, const xMat4x3 *m)
{
    memcpy(o, m, sizeof(xMat4x3));
}

inline void xMat4x3Identity(xMat4x3 *m)
{
    xMat4x3Copy(m, &g_I3);
}

inline void xQuatCopy(xQuat *o, const xQuat *q)
{
    o->s = q->s;
    o->v.x = q->v.x;
    o->v.y = q->v.y;
    o->v.z = q->v.z;
}

inline void xQuatSMul(xQuat *o, const xQuat *q, float s)
{
    o->s = q->s * s;
    xVec3SMul(&o->v, &q->v, s);
}

inline void xQuatAdd(xQuat *o, const xQuat *a, const xQuat *b)
{
    o->s = a->s + b->s;
    xVec3Add(&o->v, &a->v, &b->v);
}

inline float xQuatDot(const xQuat *a, const xQuat *b)
{
    return a->s * b->s + xVec3Dot(&a->v, &b->v);
}

inline float xQuatLength2(const xQuat *q)
{
    return xQuatDot(q, q);
}

inline void xQuatFlip(xQuat *o, const xQuat *q)
{
    o->s = -q->s;
    xVec3Inv(&o->v, &q->v);
}

inline void xQuatConj(xQuat *o, const xQuat *q)
{
    o->s = q->s;
    xVec3Inv(&o->v, &q->v);
}

inline float xQuatGetAngle(const xQuat *q)
{
    if (q->s > 0.99999f)
    {
        return 0.0f;
    }
    
    if (q->s < -0.99999f)
    {
        return 2 * PI;
    }

    return 2.0f * xacos(q->s);
}

inline void xRotCopy(xRot *o, const xRot *r)
{
    o->axis.x = r->axis.x;
    o->axis.y = r->axis.y;
    o->axis.z = r->axis.z;
    o->angle = r->angle;
}

#endif