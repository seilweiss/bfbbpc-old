#ifndef IENV_H
#define IENV_H

#include "xJSP.h"

#include <rwcore.h>
#include <rpworld.h>

struct iEnv
{
	RpWorld *world;
	RpWorld *collision;
	RpWorld *fx;
	RpWorld *camera;
	xJSPHeader *jsp;
	RpLight *light[2];
	RwFrame *light_frame[2];
	int memlvl;
};

#endif