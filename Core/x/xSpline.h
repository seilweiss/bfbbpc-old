#ifndef XSPLINE_H
#define XSPLINE_H

#include "xMath3.h"

struct xCoef
{
	float a[4];
};

struct xCoef3
{
	xCoef x;
	xCoef y;
	xCoef z;
};

struct xSpline3
{
	unsigned short type;
	unsigned short flags;
	unsigned int N;
	unsigned int allocN;
	xVec3 *points;
	float *time;
	xVec3 *p12;
	xVec3 *bctrl;
	float *knot;
	xCoef3 *coef;
	unsigned int arcSample;
	float *arcLength;
};

#endif