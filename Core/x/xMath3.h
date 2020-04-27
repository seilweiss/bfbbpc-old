#ifndef XMATH3_H
#define XMATH3_H

#include "xVec3.h"

struct xVec4
{
	float x;
	float y;
	float z;
	float w;
};

struct xMat3x3
{
	xVec3 right;
	int flags;
	xVec3 up;
	unsigned int pad1;
	xVec3 at;
	unsigned int pad2;
};

struct xMat4x3 : xMat3x3
{
	xVec3 pos;
	unsigned int pad3;
};

struct xQuat
{
	xVec3 v;
	float s;
};

struct xRot
{
	xVec3 axis;
	float angle;
};

struct xSphere
{
	xVec3 center;
	float r;
};

struct xBox
{
	xVec3 upper;
	xVec3 lower;
};

struct xBBox
{
	xVec3 center;
	xBox box;
};

struct xCylinder
{
	xVec3 center;
	float r;
	float h;
};

extern xVec3 g_O3;
extern xVec3 g_X3;
extern xVec3 g_Y3;
extern xVec3 g_Z3;
extern xVec3 g_NX3;
extern xVec3 g_NY3;
extern xVec3 g_NZ3;
extern xVec3 g_Onez;
extern xMat4x3 g_I3;
extern xQuat g_IQ;

void xMath3Init();

#endif