#ifndef XVEC3INLINES_H
#define XVEC3INLINES_H

#include "xVec3.h"
#include "xMath.h"
#include "xMathInlines.h"

inline void xVec3Init(xVec3 *v, float _x, float _y, float _z)
{
    v->x = _x;
    v->y = _y;
    v->z = _z;
}

inline void xVec3Copy(xVec3 *o, const xVec3 *v)
{
    o->x = v->x;
    o->y = v->y;
    o->z = v->z;
}

inline void xVec3Add(xVec3 *o, const xVec3 *a, const xVec3 *b)
{
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
}

inline void xVec3AddTo(xVec3 *o, const xVec3 *v)
{
    o->x += v->x;
    o->y += v->y;
    o->z += v->z;
}

inline void xVec3Cross(xVec3 *o, const xVec3 *a, const xVec3 *b)
{
    o->x = a->y * b->z - b->y * a->z;
    o->y = a->z * b->x - b->z * a->x;
    o->z = a->x * b->y - b->x * a->y;
}

inline void xVec3SMul(xVec3 *o, const xVec3 *v, float s)
{
    o->x = v->x * s;
    o->y = v->y * s;
    o->z = v->z * s;
}

inline void xVec3Sub(xVec3 *o, const xVec3 *a, const xVec3 *b)
{
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}

inline void xVec3Inv(xVec3 *o, const xVec3 *v)
{
    o->x = -v->x;
    o->y = -v->y;
    o->z = -v->z;
}

inline void xVec3Lerp(xVec3 *o, const xVec3 *a, const xVec3 *b, float t)
{
    o->x = t * (b->x - a->x) + a->x;
    o->y = t * (b->y - a->y) + a->y;
    o->z = t * (b->z - a->z) + a->z;
}

inline float xVec3Dist(const xVec3 *a, const xVec3 *b)
{
    return xsqrt(xsqr(a->x - b->x) + xsqr(a->y - b->y) + xsqr(a->z - b->z));
}

inline float xVec3Dist2(const xVec3 *a, const xVec3 *b)
{
    return xsqr(a->x - b->x) + xsqr(a->y - b->y) + xsqr(a->z - b->z);
}

inline float xVec3Length(const xVec3 *v)
{
    return xsqrt(xsqr(v->x) + xsqr(v->y) + xsqr(v->z));
}

#endif