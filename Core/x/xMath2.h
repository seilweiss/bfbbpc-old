#ifndef XMATH2_H
#define XMATH2_H

struct xVec2
{
    float x;
    float y;
};

template <class T>
struct basic_rect
{
    T x;
    T y;
    T w;
    T h;

    void scale(T x, T y, T w, T h)
    {
        this->x *= x;
        this->y *= y;
        this->w *= w;
        this->h *= h;
    }

    void scale(T x, T y)
    {
        scale(x, y, x, y);
    }

    void assign(T x, T y, T w, T h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

#endif