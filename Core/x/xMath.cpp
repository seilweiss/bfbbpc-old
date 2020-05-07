#include "xMath.h"

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