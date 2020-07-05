#ifndef XVEC3_H
#define XVEC3_H

struct xVec3
{
    float x;
    float y;
    float z;

    static xVec3 m_Null;

    inline xVec3 &assign(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;

        return *this;
    }
};

float xVec3Normalize(xVec3 *o, const xVec3 *v);
float xVec3Dot(const xVec3 *a, const xVec3 *b);

#endif