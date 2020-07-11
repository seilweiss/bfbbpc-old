#ifndef XMORPH_H
#define XMORPH_H

#include "xMath3.h"

#include <rwcore.h>
#include <rpworld.h>

struct xMorphSeqFile
{
	unsigned int Magic;
	unsigned int Flags;
	unsigned int TimeCount;
	unsigned int ModelCount;
};

struct xMorphTargetFile
{
	unsigned int Magic;
	unsigned short NumTargets;
	unsigned short NumVerts;
	unsigned int Flags;
	float Scale;
	xVec3 Center;
	float Radius;
};

struct xMorphFrame
{
	RpAtomic *Model;
	float RecipTime;
	float Scale;
	unsigned short Flags;
	unsigned short NumVerts;
	short *Targets[4];
	short WeightStart[4];
	short WeightEnd[4];
};

float xMorphSeqDuration(xMorphSeqFile *seq);

#endif