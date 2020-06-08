#ifndef ZGAMESTATE_H
#define ZGAMESTATE_H

enum eGameMode
{
    eGameMode_Boot,
    eGameMode_Intro,
    eGameMode_Title,
    eGameMode_Start,
    eGameMode_Load,
    eGameMode_Options,
    eGameMode_Save,
    eGameMode_Pause,
    eGameMode_Stall,
    eGameMode_WorldMap,
    eGameMode_MonsterGallery,
    eGameMode_ConceptArtGallery,
    eGameMode_Game,
    eGameMode_Count
};

enum eGameState
{
    eGameState_FirstTime,
    eGameState_Play,
    eGameState_LoseChance,
    eGameState_GameOver,
    eGameState_Unk04,
    eGameState_SceneSwitch,
    eGameState_Dead,
    eGameState_Exit,
    eGameState_Count
};

enum ePauseState
{
    ePauseState_Pause,
    ePauseState_Options,
    ePauseState_Count
};

enum eSaveState
{
    eSaveState_SelectMemCard,
    eSaveState_SelectSlot,
    eSaveState_Saving,
    eSaveState_Count
};

enum eOptionsState
{
    eOptionsState_Options,
    eOptionsState_Count
};

enum eLoadState
{
    eLoadState_SelectMemCard,
    eLoadState_SelectSlot,
    eLoadState_Loading,
    eLoadState_Count
};

enum eTitleState
{
    eTitleState_Start,
    eTitleState_Attract,
    eTitleState_Count
};

enum eIntroState
{
    eIntroState_Sony,
    eIntroState_Publisher,
    eIntroState_Developer,
    eIntroState_License,
    eIntroState_Count
};

enum _GameOstrich
{
    eGameOstrich_Loading,
    eGameOstrich_PlayingMovie,
    eGameOstrich_InScene,
    eGameOstrich_Total
};

extern _GameOstrich gGameOstrich;
extern eGameMode gGameMode;

int zGameStateGet();
int zGameModeGet();
void zGameSetOstrich(_GameOstrich o);
void zGameModeSwitch(eGameMode modeNew);
void zGameStateSwitch(int theNewState);

#endif