#include "zGameState.h"

#include "print.h"

int gGameState = eGameState_Dead;
_GameOstrich gGameOstrich;
eGameMode gGameMode;

int zGameStateGet()
{
    return gGameState;
}

int zGameModeGet()
{
    return gGameMode;
}

void zGameSetOstrich(_GameOstrich o)
{
    gGameOstrich = o;
}

void zGameStateSwitch(int theNewState)
{
    BFBBSTUB("zGameStateSwitch");
    gGameState = theNewState;
}

void zGameModeSwitch(eGameMode modeNew)
{
    BFBBSTUB("zGameModeSwitch");
    gGameMode = modeNew;
}