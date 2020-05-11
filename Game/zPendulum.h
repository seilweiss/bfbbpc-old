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

void zPendulum_Init(void *pend, void *asset);

#endif