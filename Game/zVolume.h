#ifndef ZVOLUME_H
#define ZVOLUME_H

#include "xVolume.h"

struct zVolume : xVolume
{
};

extern int gOccludeCount;

void zVolumeInit();
zVolume *zVolumeGetVolume(unsigned short n);

#endif