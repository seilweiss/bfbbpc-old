#ifndef XPARGROUP_H
#define XPARGROUP_H

#include "xPar.h"
#include "xParCmd.h"

struct xParGroup
{
	xPar *m_root;
	xPar *m_dead;
	int m_num_of_particles;
	unsigned char m_alive;
	unsigned char m_killWhenDead;
	unsigned char m_active;
	unsigned char m_visible;
	unsigned char m_culled;
	unsigned char m_priority;
	unsigned char m_flags;
	unsigned char m_regidx;
	xParGroup *m_next;
	xParGroup *m_prev;
	void(*draw)(void *, xParGroup *);
	xParCmdTex *m_cmdTex;
};

#endif