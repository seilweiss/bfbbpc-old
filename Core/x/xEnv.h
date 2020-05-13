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

extern xEnv *gCurXEnv;

void xEnvLoadBsp(xEnv *env, const void *data, unsigned int datasize, int dataType);
void xEnvSetup(xEnv *env);

#endif