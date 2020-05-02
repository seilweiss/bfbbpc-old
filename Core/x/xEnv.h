#ifndef XENV_H
#define XENV_H

#include "iEnv.h"
#include "xLightKit.h"

struct xEnv
{
    iEnv *geom;
    iEnv ienv;
    xLightKit *lightKit;
};

#endif