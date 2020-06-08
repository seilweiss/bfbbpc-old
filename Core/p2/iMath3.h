#ifndef IMATH3_H
#define IMATH3_H

#include "xMath3.h"
#include <rwcore.h>

union xiMat4x3Union
{
	xMat4x3 xm;
	RwMatrix im;
};

void iMath3Init();

#endif