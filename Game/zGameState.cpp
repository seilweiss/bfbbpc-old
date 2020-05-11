#include "zGameState.h"

#include "print.h"

_GameOstrich gGameOstrich;

void zGameSetOstrich(_GameOstrich o)
{
    gGameOstrich = o;
}

void zGameStateSwitch(int theNewState)
{
    BFBBSTUB("zGameStateSwitch");
}

void zGameModeSwitch(eGameMode modeNew)
{
    BFBBSTUB("zGameModeSwitch");
}