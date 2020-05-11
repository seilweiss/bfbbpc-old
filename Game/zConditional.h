#ifndef ZCONDITIONAL_H
#define ZCONDITIONAL_H

#include "xBase.h"

struct zCondAsset : xBaseAsset
{
    unsigned int constNum;
    unsigned int expr1;
    unsigned int op;
    unsigned int value_asset;
};

struct _zConditional : xBase
{
    zCondAsset *asset;
};

void zConditionalInit(void *b, void *asset);

#endif