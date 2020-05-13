#ifndef ZENT_H
#define ZENT_H

#include "xEnt.h"
#include "xAnim.h"

struct zEnt : xEnt
{
    xAnimTable *atbl;
};

void zEntEventAll(xBase *from, unsigned int fromEvent, unsigned int toEvent,
                  float *toParam);

#endif