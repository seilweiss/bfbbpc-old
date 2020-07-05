#ifndef ZPAREMITTER_H
#define ZPAREMITTER_H

#include "xParEmitter.h"

struct zParEmitter : xParEmitter
{
};

zParEmitter *zParEmitterFind(unsigned int asset_id);
zParEmitter *zParEmitterFind(const char *asset_name);

#endif