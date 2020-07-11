#include "xMath.h"

#include "xMathInlines.h"

#include <math.h>

static int xmath_inited;
static int xmath_exited;
static unsigned int rndseed;

void xMathInit()
{
    if (!xmath_inited)
    {
        xmath_inited = 1;
        rndseed = 0;
    }
}

void xMathExit()
{
    if (!xmath_exited)
    {
        xmath_exited = 1;
    }
}

float xatof(const char *x)
{
    return atof(x);
}

void xsrand(unsigned int seed)
{
    rndseed = seed;
}

unsigned int xrand()
{
    rndseed = rndseed * 1103515245 + 12345;
    return rndseed;
}

float xurand()
{
    return 1.0f / 0xFFFFFFFF * xrand();
}

float xAngleClampFast(float a)
{
    if (a < 0.0f)
    {
        return a + PI * 2;
    }

    if (a >= (PI * 2))
    {
        return a - PI * 2;
    }

    return a;
}

float xDangleClamp(float a)
{
    float b = xfmod(a, 2 * PI);

    if (b >= PI)
    {
        return b - 2 * PI;
    }

    if (b < -PI)
    {
        return b + 2 * PI;
    }

    return b;
}