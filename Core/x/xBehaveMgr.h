#ifndef XBEHAVEMGR_H
#define XBEHAVEMGR_H

#include "xRMemData.h"
#include "xBase.h"
#include "xBehaviour.h"

struct xPSYNote
{
};

enum en_pendtype
{
	PEND_TRAN_NONE,
	PEND_TRAN_SET,
	PEND_TRAN_PUSH,
	PEND_TRAN_POP,
	PEND_TRAN_POPTO,
	PEND_TRAN_POPALL,
	PEND_TRAN_SWAP,
	PEND_TRAN_INPROG,
	PEND_TRAN_NOMORE
};

enum PSY_BRAIN_STATUS
{
	PSY_STAT_BLANK,
	PSY_STAT_GROW,
	PSY_STAT_EXTEND,
	PSY_STAT_THINK,
	PSY_STAT_NOMORE,
	PSY_STAT_FORCE = 0x7fffffff
};

struct xPsyche : RyzMemData
{
	xBase *clt_owner;
	xPSYNote *cb_notice;
	int flg_psyche;
	xGoal *goallist;
	xGoal *goalstak[5];
	float tmr_stack[5][1];
	int staktop;
	xGoal *pendgoal;
	en_pendtype pendtype;
	int gid_safegoal;
	void(*fun_remap)(int *, en_trantype *);
	void *userContext;
	int cnt_transLastTimestep;
	PSY_BRAIN_STATUS psystat;
	xBase fakebase;
};

void xBehaveMgr_Startup();
void xBehaveMgr_Shutdown();

#endif