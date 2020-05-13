#ifndef ZPENDULUM_H
#define ZPENDULUM_H

#include "zEnt.h"
#include "xEntMotion.h"

struct _zPendulum : zEnt
{
    xEntMotion motion;
    float lt;
    float q1t;
    float q3t;
};

struct xScene;

void zPendulum_Init(void *pend, void *asset);
void zPendulum_Setup(_zPendulum *pend, xScene *sc);
void zPendulum_Reset(_zPendulum *pend, xScene *sc);

#endif