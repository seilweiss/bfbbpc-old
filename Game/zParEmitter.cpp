#include "zParEmitter.h"

#include "xString.h"
#include "zMain.h"
#include "zBase.h"

zParEmitter *zParEmitterFind(unsigned int asset_id)
{
    zScene *s = globals.sceneCur;

    if (!s)
    {
        return NULL;
    }

    zParEmitter *pe = (zParEmitter *)s->baseList[eBaseTypeParticleEmitter];

    for (int i = 0; i < s->baseCount[eBaseTypeParticleEmitter]; i++)
    {
        if (pe[i].tasset->id == asset_id)
        {
            return &pe[i];
        }
    }

    return NULL;
}

zParEmitter *zParEmitterFind(const char *asset_name)
{
    unsigned int asset_id = xStrHash(asset_name);

    return zParEmitterFind(asset_id);
}