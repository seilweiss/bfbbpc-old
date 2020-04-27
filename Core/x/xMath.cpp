#include "xMath.h"

static int xmath_inited;
static unsigned int rndseed;

void xMathInit()
{
    if (!xmath_inited)
    {
        xmath_inited = 1;
        rndseed = 0;
    }
}