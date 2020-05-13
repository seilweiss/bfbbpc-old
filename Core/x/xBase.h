#ifndef XBASE_H
#define XBASE_H

#include "xLinkAsset.h"
#include "xserializer.h"

struct xBaseAsset
{
    unsigned int id;
    unsigned char baseType;
    unsigned char linkCount;
    unsigned short baseFlags;
};

struct xBase;

typedef int(*xBaseEventCallBack)(xBase *from, xBase *to, unsigned int toEvent,
                                 const float *toParam, xBase *toParamWidget);

struct xBase
{
    unsigned int id;
    unsigned char baseType;
    unsigned char linkCount;
    unsigned short baseFlags;
    xLinkAsset *link;
    xBaseEventCallBack eventFunc;
};

#define XBASE_ENABLED 0x1
#define XBASE_VALID   0x4
#define XBASE_UNK10   0x10

void xBaseInit(xBase *xb, xBaseAsset *asset);
void xBaseSetup(xBase *xb);
void xBaseSave(xBase *ent, xSerial *s);
void xBaseLoad(xBase *ent, xSerial *s);
void xBaseReset(xBase *xb, xBaseAsset *asset);

inline bool xBaseIsEnabled(const xBase *xb)
{
    return (xb->baseFlags & XBASE_ENABLED);
}

inline void xBaseEnable(xBase *xb)
{
    xb->baseFlags |= XBASE_ENABLED;
}

inline void xBaseDisable(xBase *xb)
{
    xb->baseFlags &= ~XBASE_ENABLED;
}

inline int xBaseIsValid(xBase *xb)
{
    return (xb->baseFlags & XBASE_VALID);
}

inline void xBaseValidate(xBase *xb)
{
    xb->baseFlags |= XBASE_VALID;
}

#endif