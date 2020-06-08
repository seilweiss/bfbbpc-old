#include "xVec3.h"

#include "xMathInlines.h"
#include "iMath.h"

xVec3 xVec3::m_Null = { 0.0f, 0.0f, 0.0f };

float xVec3Normalize(xVec3 *o, const xVec3 *v)
{
    float len;

    float len2 = xsqr(v->x) + xsqr(v->y) + xsqr(v->z);

    if (CLOSEENOUGH(len2, 1.0f))
    {
        o->x = v->x;
        o->y = v->y;
        o->z = v->z;

        len = 1.0f;
    }
    else if (CLOSEENOUGH(len2, 0.0f))
    {
        o->x = 0.0f;
        o->y = 1.0f;
        o->z = 0.0f;

        len = 0.0f;
    }
    else
    {
        len = xsqrt(len2);

        float len_inv = 1.0f / len;

        o->x = v->x * len_inv;
        o->y = v->y * len_inv;
        o->z = v->z * len_inv;
    }

    return len;
}

float xVec3Dot(const xVec3 *a, const xVec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}