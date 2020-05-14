#include "xBase.h"

#include <stdlib.h>

void xBaseInit(xBase *xb, xBaseAsset *asset)
{
    xb->id = asset->id;
    xb->baseType = asset->baseType;
    xb->baseFlags = asset->baseFlags;
    xb->linkCount = asset->linkCount;
    xb->link = NULL;

    xBaseValidate(xb);
}

void xBaseSetup(xBase *xb)
{
    return;
}

void xBaseSave(xBase *ent, xSerial *s)
{
    if (xBaseIsEnabled(ent))
    {
        s->Write_b1(1);
    }
    else
    {
        s->Write_b1(0);
    }
}

void xBaseLoad(xBase *ent, xSerial *s)
{
    int b = 0;

    s->Read_b1(&b);

    if (b)
    {
        xBaseEnable(ent);
    }
    else
    {
        xBaseDisable(ent);
    }
}

void xBaseReset(xBase *xb, xBaseAsset *asset)
{
    xb->baseFlags =
        (xb->baseFlags & ~XBASE_RECSHADOW) |
        (asset->baseFlags & XBASE_RECSHADOW);
}