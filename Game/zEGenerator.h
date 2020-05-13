#ifndef ZEGENERATOR_H
#define ZEGENERATOR_H

#include "zEnt.h"
#include "zLightning.h"

struct zEGenAsset : xEntAsset
{
    xVec3 src_dpos;
    unsigned char damage_type;
    unsigned char flags;
    float ontime;
    unsigned int onAnimID;
};

struct zEGenerator : zEnt
{
    zEGenAsset *zasset;
    unsigned short flags;
    unsigned short num_dsts;
    float tmr;
    xAnimFile *afile;
    xVec3 src_pos;
    xVec3 dst_pos;
    xVec3 dst_off;
    xBase *dst;
    zLightning *lfx[2];
};

struct xScene;

void zEGenerator_Init(void *egen, void *asset);
void zEGenerator_Setup(zEGenerator *egen, xScene *sc);
void zEGenerator_Reset(zEGenerator *egen, xScene *sc);

#endif