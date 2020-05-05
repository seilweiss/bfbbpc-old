#ifndef XMATH2_H
#define XMATH2_H

#include "print.h"

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

    static basic_rect<T> m_Null;
    static basic_rect<T> m_Unit;

    void expand(T x, T y, T w, T h)
    {
        this->x -= x;
        this->w += x + w;
        this->y -= y;
        this->h += y + h;
    }

    void expand(T n)
    {
        expand(n, n, n, n);
    }

    void contract(T n)
    {
        expand(-n);
    }

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

    bool empty() const
    {
        return (w <= 0 || h <= 0);
    }

    void clip(basic_rect<T> &r1, basic_rect<T> &r2) const
    {
        float f5 = r2.w / r1.w;
        float f0 = r2.h / r1.h;

        if (r1.x < x)
        {
            float f2 = x - r1.x;
            float f3 = f5 * f2;

            r1.x = x;
            r1.w -= f2;
            r2.x += f3;
            r2.y -= f3;
        }

        if (r1.y < y)
        {
            float f2 = x - r1.y;
            float f3 = f0 * f2;

            r1.y = y;
            r1.h -= f2;
            r2.y += f3;
            r2.h -= f3;
        }

        float f3 = r1.x + r1.w;
        float f4 = x + w;

        if (f3 > f4)
        {
            r2.w -= f5 * (f3 - f4);
            r1.w = f4 - r1.x;
        }

        f3 = r1.y + r1.h;
        f4 = y + h;

        if (f3 > f4)
        {
            r2.h -= f0 * (f3 - f4);
            r1.h = f4 - r1.y;
        }
    }

    void set_bounds(T x1, T y1, T x2, T y2)
    {
        x = x1;
        w = x2 - x1;
        y = y1;
        h = y2 - y1;
    }

    void get_bounds(T &x1, T &y1, T &x2, T &y2) const
    {
        x1 = x;
        x2 = x + w;
        y1 = y;
        y2 = y + h;
    }

    void move(T dx, T dy)
    {
        x += dx;
        y += dy;
    }

    void center(T cx, T cy)
    {
        x = -(0.5 * w - cx);
        y = -(0.5 * h - cy);
    }

    void set_size(float sw, float sh)
    {
        w = sw;
        h = sh;
    }

    void set_size(float size)
    {
        w = size;
        h = size;
    }

    basic_rect<T> &operator|=(const basic_rect<T> &r)
    {
        T ax1, ay1, ax2, ay2;
        T bx1, by1, bx2, by2;

        get_bounds(ax1, ay1, ax2, ay2);
        r.get_bounds(bx1, by1, bx2, by2);

        if (ax1 > bx1)
        {
            ax1 = bx1;
        }

        if (ay1 > by1)
        {
            ay1 = by1;
        }

        if (ax2 < bx2)
        {
            ax2 = bx2;
        }

        if (ay2 < by2)
        {
            ay2 = by2;
        }

        set_bounds(ax1, ay1, ax2, ay2);

        return *this;
    }
};

template<class T>
basic_rect<T> basic_rect<T>::m_Null = { 0, 0, 0, 0 };

template<class T>
basic_rect<T> basic_rect<T>::m_Unit = { 0, 0, 1, 1 };

#endif