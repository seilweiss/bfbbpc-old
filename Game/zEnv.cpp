#include "zEnv.h"

#include "zMain.h"
#include "xstransvc.h"
#include "xEnv.h"
#include "zSurface.h"
#include "zLight.h"

#include "print.h"

void zEnvInit(void *env, void *easset)
{
    zEnvInit((_zEnv *)env, (xEnvAsset *)easset);
}

void zEnvInit(_zEnv *env, xEnvAsset *easset)
{
    unsigned int bufsize;
    void *buf;

    xBaseInit(env, easset);

    env->easset = easset;
    env->eventFunc = zEnvEventCB;

    if (env->linkCount)
    {
        env->link = (xLinkAsset *)(easset + 1);
    }

    globals.sceneCur->zen = env;
    globals.sceneCur->zen = env; // why does this happen twice?

    xSTAssetCountByType('BSP ');

    buf = xSTFindAsset(easset->bspAssetID, &bufsize);

    if (buf)
    {
        xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, 0);
    }

    zSurfaceRegisterMapper(easset->bspMapperID);

    if (easset->bspCollisionAssetID)
    {
        buf = xSTFindAsset(easset->bspCollisionAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, 1);
        }
    }

    if (easset->bspFXAssetID)
    {
        buf = xSTFindAsset(easset->bspFXAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, 2);
        }
    }

    if (easset->bspCameraAssetID)
    {
        buf = xSTFindAsset(easset->bspCameraAssetID, &bufsize);

        if (buf)
        {
            xEnvLoadBsp(globals.sceneCur->env, buf, bufsize, 3);
        }
    }

    if (globals.sceneCur->env && globals.sceneCur->env->geom)
    {
        zLightResetAll(globals.sceneCur->env);
        iLightInit(globals.sceneCur->env->geom->world);
    }
}

void zEnvSetup(_zEnv *env)
{
    BFBBSTUB("zEnvSetup");
}

void zEnvStartingCamera(_zEnv *env)
{
    BFBBSTUB("zEnvStartingCamera");
}

void zEnvRender(xEnv *env)
{
    BFBBSTUB("zEnvRender");

    /*
    RpWorld *world = env->geom->world;
    int num = RpWorldGetNumMaterials(world);

    for (int i = 0; i < num; i++)
    {
        xSurface *sp = zSurfaceGetSurface(i);
        zSurfaceProps *pp = (zSurfaceProps *)sp->moprops;

        if (pp && pp->asset && (pp->texanim_flags & 0x1))
        {
            RpMaterial *mp = RpWorldGetMaterial(world, i);

            if (mp)
            {
                xGroup *g = (xGroup *)pp->texanim[0].group_ptr;

                if (g)
                {
                    unsigned int texid = xGroupGetItem(g, pp->texanim[0].group_idx);
                    RwTexture *texptr = (RwTexture *)xSTFindAsset(texid, NULL);

                    if (texptr)
                    {
                        RpMaterialSetTexture(mp, texptr);
                    }
                }
            }
        }
    }
    */
    
    xEnvRender(env);
}

int zEnvEventCB(xBase *from, xBase *to, unsigned int toEvent, const float *toParam,
                xBase *toParamWidget)
{
    BFBBSTUB("zEnvEventCB");
    return 1;
}