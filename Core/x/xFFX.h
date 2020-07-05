#ifndef XFFX_H
#define XFFX_H

#include "xScene.h"

typedef void(*xFFXDoEffectCallBack)(xEnt *ent, xScene *sc, float dt, void *fdata);

struct xFFX
{
    unsigned int flags;
    xFFXDoEffectCallBack doEffect;
    void *fdata;
    xFFX *next;
};

struct xFFXRotMatchState
{
	int lgrounded;
	xVec3 lfup;
	xVec3 lfat;
	xVec3 plfat;
	float tmr;
	float mrate;
	float tmatch;
	float rrate;
	float trelax;
	float max_decl;
	xFFXRotMatchState *next;
};

void xFFXPoolInit(unsigned int num_ffx);
short xFFXAddEffect(xEnt *ent, xFFXDoEffectCallBack dof, void *fd);
void xFFXShakePoolInit(unsigned int num);
void xFFXRotMatchPoolInit(unsigned int num);
xFFXRotMatchState *xFFXRotMatchAlloc();

#endif