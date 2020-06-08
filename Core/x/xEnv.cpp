#include "xEnv.h"

#include "print.h"

xEnv *gCurXEnv;

void xEnvLoadBsp(xEnv *env, const void *data, unsigned int datasize, int dataType)
{
    if (dataType == 0)
    {
        env->geom = &env->ienv;
    }

    iEnvLoad(env->geom, data, datasize, dataType);

    gCurXEnv = env;
}

void xEnvSetup(xEnv *env)
{
    iEnvDefaultLighting(env->geom);

    env->lightKit = NULL;
    gCurXEnv = env;
}

void xEnvRender(xEnv *env)
{
    if (env->geom)
    {
        iEnvRender(env->geom);
    }
}