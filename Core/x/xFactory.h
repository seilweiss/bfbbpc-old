#ifndef XFACTORY_H
#define XFACTORY_H

#include "xRMemData.h"

struct xFactoryInst : RyzMemData
{
	int itemType;
	xFactoryInst *nextprod;
	xFactoryInst *prevprod;
};

#endif