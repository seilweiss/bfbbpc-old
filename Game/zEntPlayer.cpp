#include "zEntPlayer.h"

#include "zMain.h"

#include "print.h"

void zEntPlayerControlOn(zControlOwner owner)
{
    BFBBSTUB("zEntPlayerControlOn");
}

void zEntPlayer_Init(xEnt *ent, xEntAsset *asset)
{
    BFBBSTUB("zEntPlayer_Init");

    zEntInit((zEnt *)ent, asset, 'PLYR');
}

void zEntPlayer_ShadowModelEnable()
{
    BFBBSTUB("zEntPlayer_ShadowModelEnable");
}

void zEntPlayer_ShadowModelDisable()
{
    BFBBSTUB("zEntPlayer_ShadowModelDisable");
}

void CalcJumpImpulse(zJumpParam *param, const zPlayerSettings *settings)
{
    BFBBSTUB("CalcJumpImpulse");
}

void zEntPlayerExit(xEnt *ent)
{
    BFBBSTUB("zEntPlayerExit");
}

void zEntPlayer_StoreCheckPoint(xVec3 *pos, float rot, unsigned int initCamID)
{
    if (pos)
    {
        globals.player.cp.pos = *pos;
        globals.player.cp.rot = rot;
        globals.player.cp.initCamID = initCamID;
    }
}

void zEntPlayer_LoadSounds()
{
    BFBBSTUB("zEntPlayer_LoadSounds");
}

void zEntPlayer_UnloadSounds()
{
    BFBBSTUB("zEntPlayer_UnloadSounds");
}