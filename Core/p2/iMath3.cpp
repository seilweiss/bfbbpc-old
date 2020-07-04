#include "iMath3.h"

void iMath3Init()
{
    return;
}

void iBoxInitBoundVec(xBox *b, const xVec3 *v)
{
    xVec3Copy(&b->lower, v);
    xVec3Copy(&b->upper, v);
}

void iBoxBoundVec(xBox *o, const xBox *b, const xVec3 *v)
{
    xVec3Init(&o->lower,
              xmin(v->x, b->lower.x),
              xmin(v->y, b->lower.y),
              xmin(v->z, b->lower.z));

    xVec3Init(&o->upper,
              xmax(v->x, b->upper.x),
              xmax(v->y, b->upper.y),
              xmax(v->z, b->upper.z));
}