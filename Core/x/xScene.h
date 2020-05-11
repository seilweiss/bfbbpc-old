#ifndef XSCENE_H
#define XSCENE_H

#include "xEnt.h"
#include "xEnv.h"
#include "xMemMgr.h"

typedef xBase *(*xSceneResolvIDCallBack)(unsigned int gameID);
typedef const char *(*xSceneBase2NameCallBack)(xBase *b);
typedef const char *(*xSceneID2NameCallBack)(unsigned int gameID);

struct xScene
{
    unsigned int sceneID;
    unsigned short flags;
    unsigned short num_ents;
    unsigned short num_trigs;
    unsigned short num_stats;
    unsigned short num_dyns;
    unsigned short num_npcs;
    unsigned short num_act_ents;
    unsigned short num_nact_ents;
    float gravity;
    float drag;
    float friction;
    unsigned short num_ents_allocd;
    unsigned short num_trigs_allocd;
    unsigned short num_stats_allocd;
    unsigned short num_dyns_allocd;
    unsigned short num_npcs_allocd;
    xEnt **trigs;
    xEnt **stats;
    xEnt **dyns;
    xEnt **npcs;
    xEnt **act_ents;
    xEnt **nact_ents;
    xEnv *env;
    xMemPool mempool;
    xSceneResolvIDCallBack resolvID;
    xSceneBase2NameCallBack base2Name;
    xSceneID2NameCallBack id2Name;
};

extern xScene *g_xSceneCur;

void xSceneInit(xScene *sc, unsigned short num_trigs, unsigned short num_stats,
                unsigned short num_dyns, unsigned short num_npcs);
void xSceneAddEnt(xScene *sc, xEnt *ent);

#endif