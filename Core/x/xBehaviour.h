#ifndef XBEHAVIOUR_H
#define XBEHAVIOUR_H

#include "xListItem.h"
#include "xFactory.h"

enum en_GOALSTATE
{
    GOAL_STAT_UNKNOWN,
    GOAL_STAT_PROCESS,
    GOAL_STAT_ENTER,
    GOAL_STAT_EXIT,
    GOAL_STAT_SUSPEND,
    GOAL_STAT_RESUME,
    GOAL_STAT_PAUSED,
    GOAL_STAT_DONE,
    GOAL_STAT_NOMORE,
    GOAL_STAT_FORCE = 0x7fffffff
};

enum en_trantype
{
    GOAL_TRAN_NONE,
    GOAL_TRAN_SET,
    GOAL_TRAN_PUSH,
    GOAL_TRAN_POP,
    GOAL_TRAN_POPTO,
    GOAL_TRAN_POPALL,
    GOAL_TRAN_POPBASE,
    GOAL_TRAN_POPSAFE,
    GOAL_TRAN_SWAP,
    GOAL_TRAN_NOMORE,
    GOAL_TRAN_FORCE = 0x7fffffff
};

struct xPsyche;

struct xGoal : xListItem<xGoal>, xFactoryInst
{
    xPsyche *psyche;
    int goalID;
    en_GOALSTATE stat;
    int flg_able;
    int(*fun_process)(xGoal *, void *, en_trantype *, float, void *);
    int(*fun_precalc)(xGoal *, void *, float, void *);
    int(*fun_chkRule)(xGoal *, void *, en_trantype *, float, void *);
    void *cbdata;
};

#endif