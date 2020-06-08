#ifndef XVEC3_H
#define XVEC3_H

struct xVec3
{
    float x;
    float y;
    float z;

    static xVec3 m_Null;
};

float xVec3Normalize(xVec3 *o, const xVec3 *v);
float xVec3Dot(const xVec3 *a, const xVec3 *b);

#endif