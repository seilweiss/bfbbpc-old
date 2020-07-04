#include "zGameState.h"

#include "xEvent.h"
#include "zGame.h"
#include "zEnt.h"
#include "zBase.h"
#include "xSnd.h"
#include "zMain.h"

#include "print.h"

int gGameState = eGameState_Dead;
_GameOstrich gGameOstrich;
eGameMode gGameMode;

static unsigned int sGameMode_DispatchTable[] =
{
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown,
    eEventUnknown
};

static unsigned int sGameState_DispatchTable[] =
{
    eEventDispatcher_GameState_FirstTime,
    eEventDispatcher_GameState_Play,
    eEventDispatcher_GameState_LoseChance,
    eEventDispatcher_GameState_GameOver,
    eEventDispatcher_GameState_SceneSwitch,
    eEventDispatcher_GameState_Dead,
    eEventUnknown,
    eEventUnknown
};

static unsigned int sPauseState_DispatchTable[] =
{
    eEventDispatcher_PauseState_Pause,
    eEventDispatcher_PauseState_Options
};

static unsigned int sSaveState_DispatchTable[] =
{
    eEventDispatcher_SaveState_SelectMemCard,
    eEventDispatcher_SaveState_SelectSlot,
    eEventDispatcher_SaveState_Saving
};

static unsigned int sOptionsState_DispatchTable[] =
{
    eEventDispatcher_OptionsState_Options
};

static unsigned int sLoadState_DispatchTable[] =
{
    eEventDispatcher_LoadState_SelectMemCard,
    eEventDispatcher_LoadState_SelectSlot,
    eEventDispatcher_LoadState_Loading
};

static unsigned int sTitleState_DispatchTable[] =
{
    eEventDispatcher_TitleState_Start,
    eEventDispatcher_TitleState_Attract
};

static unsigned int sIntroState_DispatchTable[] =
{
    eEventDispatcher_IntroState_Sony,
    eEventDispatcher_IntroState_Publisher,
    eEventDispatcher_IntroState_Developer,
    eEventDispatcher_IntroState_License
};

static unsigned int sGameState_DoDispatchTable[] =
{
    eEventDispatcher_SetGameState_FirstTime,
    eEventDispatcher_SetGameState_Play,
    eEventDispatcher_SetGameState_LoseChance,
    eEventDispatcher_SetGameState_GameOver,
    eEventDispatcher_SetGameState_GameStats,
    eEventDispatcher_SetGameState_SceneSwitch,
    eEventDispatcher_SetGameState_Dead,
    eEventDispatcher_SetGameState_Exit
};

static unsigned int sPauseState_DoDispatchTable[] =
{
    eEventDispatcher_SetPauseState_Pause,
    eEventDispatcher_SetPauseState_Options
};

static unsigned int sSaveState_DoDispatchTable[] =
{
    eEventDispatcher_SetSaveState_SelectMemCard,
    eEventDispatcher_SetSaveState_SelectSlot,
    eEventDispatcher_SetSaveState_Saving
};

static unsigned int sOptionsState_DoDispatchTable[] =
{
    eEventDispatcher_SetOptionsState_Options
};

static unsigned int sLoadState_DoDispatchTable[] =
{
    eEventDispatcher_SetLoadState_SelectMemCard,
    eEventDispatcher_SetLoadState_SelectSlot,
    eEventDispatcher_SetLoadState_Loading
};

static unsigned int sTitleState_DoDispatchTable[] =
{
    eEventDispatcher_SetTitleState_Start,
    eEventDispatcher_SetTitleState_Attract
};

static unsigned int sIntroState_DoDispatchTable[] =
{
    eEventDispatcher_SetIntroState_Sony,
    eEventDispatcher_SetIntroState_Publisher,
    eEventDispatcher_SetIntroState_Developer,
    eEventDispatcher_SetIntroState_License
};

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
    int stateOld = gGameState;
    unsigned int theEvent = eEventUnknown;

    gGameState = theNewState;

    if (theNewState == eGameState_Play && stateOld == eGameState_FirstTime)
    {
        startPressed = 1;
    }

    switch (gGameMode)
    {
    case eGameMode_Intro:
    {
        theEvent = sIntroState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Title:
    {
        theEvent = sTitleState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Load:
    {
        theEvent = sLoadState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Options:
    {
        theEvent = sOptionsState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Save:
    {
        theEvent = sSaveState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Pause:
    {
        theEvent = sPauseState_DispatchTable[theNewState];
        break;
    }
    case eGameMode_Game:
    {
        theEvent = sGameState_DispatchTable[theNewState];
        break;
    }
    }

    if (theEvent != eEventUnknown)
    {
        zEntEventAllOfType(theEvent, eBaseTypeDispatcher);
    }
}

void zGameModeSwitch(eGameMode modeNew)
{
    eGameMode modeOld = gGameMode;

    bool pausing =
        (modeNew == eGameMode_Save ||
         modeNew == eGameMode_Pause ||
         modeNew == eGameMode_Stall);

    bool unpausing =
        (modeOld == eGameMode_Save ||
         modeOld == eGameMode_Pause ||
         modeOld == eGameMode_Stall);

    if (modeOld == eGameMode_Game && pausing)
    {
        xSndPauseAll(1, 1);
    }
    else if (modeNew == eGameMode_Game && unpausing)
    {
        xSndPauseAll(0, 0);

        globals.pad0->pressed = 0;
        globals.pad0->released = 0;
        globals.pad0->analog1.x = 0;
        globals.pad0->analog1.y = 0;
        globals.pad0->analog2.x = 0;
        globals.pad0->analog2.y = 0;
    }

    gGameMode = modeNew;

    zEntEventAllOfType(sGameMode_DispatchTable[modeNew], eBaseTypeDispatcher);
}