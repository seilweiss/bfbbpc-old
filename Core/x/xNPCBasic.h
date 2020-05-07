#ifndef XNPCBASIC_H
#define XNPCBASIC_H

#include "xEnt.h"
#include "xFactory.h"

struct xNPCBasic : xEnt, xFactoryInst
{
	void(*f_setup)(xEnt *);
	void(*f_reset)(xEnt *);
	struct
	{
		int flg_basenpc : 16;
		int inUpdate    : 8;
		int flg_upward  : 8;
	};
	int colFreq;
	int colFreqReset;
	struct
	{
		unsigned int flg_colCheck : 8;
		unsigned int flg_penCheck : 8;
		unsigned int flg_unused   : 16;
	};
	int myNPCType;
	xEntShadow entShadow_embedded;
	xShadowSimpleCache simpShadow_embedded;
};

#endif