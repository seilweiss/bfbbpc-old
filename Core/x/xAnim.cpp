#include "xAnim.h"

#include "print.h"

static bool _xSingleCompare(char c1, char c2)
{
    switch (c2)
    {
    case '?':
    case '*':
    case '+':
    {
        return true;
    }
    case '#':
    {
        return (c1 >= '0' && c1 <= '9');
    }
    }

    return (c1 == c2);
}

static bool _xSingleCompare(char c, const char *s)
{
    while (*s)
    {
        if (_xSingleCompare(c, *s))
        {
            return true;
        }

        s++;
    }

    return false;
}

static bool _xCharIn(char c, const char *s)
{
    while (*s)
    {
        if (c == *s)
        {
            return true;
        }
        s++;
    }

    return false;
}

void xAnimInit()
{
    BFBBSTUB("xAnimInit");
}

void xAnimTempTransitionInit(unsigned int count)
{
    BFBBSTUB("xAnimTempTransitionInit");
}

xAnimState *xAnimTableGetState(xAnimTable *table, const char *name)
{
    BFBBSTUB("xAnimTableGetState");
    return NULL;
}

void xAnimPlaySetState(xAnimSingle *single, xAnimState *state, float startTime)
{
    BFBBSTUB("xAnimPlaySetState");
}

void xAnimPoolInit(xMemPool *pool, unsigned int count, unsigned int singles,
                   unsigned int blendFlags, unsigned int effectMax)
{
    BFBBSTUB("xAnimPoolInit");
}

xAnimPlay *xAnimPoolAlloc(xMemPool *pool, void *object, xAnimTable *table,
                          xModelInstance *modelInst)
{
    BFBBSTUB("xAnimPoolAlloc");
    return NULL;
}