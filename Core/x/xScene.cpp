#include "xScene.h"

#include "xMemMgr.h"
#include "xAnim.h"
#include "xCollide.h"
#include "xCollideFast.h"

#include "print.h"

xScene *g_xSceneCur;

void xSceneInit(xScene *sc, unsigned short num_trigs, unsigned short num_stats,
                unsigned short num_dyns, unsigned short num_npcs)
{
    sc->flags = 0;

    sc->num_trigs_allocd = num_trigs;
    sc->trigs = (xEnt **)xMemAlloc(num_trigs * sizeof(xEnt *), 0);

    sc->num_stats_allocd = num_stats;
    sc->stats = (xEnt **)xMemAlloc(num_stats * sizeof(xEnt *), 0);

    sc->num_dyns_allocd = num_dyns;
    sc->dyns = (xEnt **)xMemAlloc(num_dyns * sizeof(xEnt *), 0);

    sc->num_npcs_allocd = num_npcs;
    sc->npcs = (xEnt **)xMemAlloc(num_npcs * sizeof(xEnt *), 0);

    sc->num_ents_allocd = num_trigs + num_stats + num_dyns + num_npcs;

    sc->num_act_ents = 0;
    sc->act_ents = (xEnt **)xMemAlloc(sc->num_ents_allocd * sizeof(xEnt *), 0);

    sc->num_nact_ents = 0;
    sc->nact_ents = (xEnt **)xMemAlloc(sc->num_ents_allocd * sizeof(xEnt *), 0);

    sc->num_ents = 0;
    sc->num_trigs = 0;
    sc->num_stats = 0;
    sc->num_dyns = 0;
    sc->num_npcs = 0;
    sc->resolvID = NULL;

    xAnimInit();
    xModelInit();

    xAnimPoolInit(&sc->mempool, 50, 1, 1, 4);

    xModelPoolInit(49, 64);
    xModelPoolInit(74, 8);
    xModelPoolInit(164, 1);

    xAnimTempTransitionInit(16);

    xCollideInit(sc);
    xCollideFastInit(sc);
}

void xSceneExit(xScene *sc)
{
    return;
}

void xSceneSetup(xScene *sc)
{
    xEnvSetup(sc->env);
}

void xSceneAddEnt(xScene *sc, xEnt *ent)
{
    BFBBSTUB("xSceneAddEnt");
}

void xRayHitsGrid(xGrid *grid, xScene *sc, xRay3 *r, xRayHitsEntCallBack rentcb,
                  xQCData *qcr, void *data)
{
    BFBBSTUB("xRayHitsGrid");
}