#ifndef XCOLOR_H
#define XCOLOR_H

#include "iColor.h"

inline iColor_tag xColorFromRGBA(unsigned char r, unsigned char g, unsigned char b,
                                 unsigned char a)
{
    iColor_tag c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;

    return c;
}

#endif