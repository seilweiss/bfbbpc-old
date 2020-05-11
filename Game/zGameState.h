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

enum _GameOstrich
{
	eGameOstrich_Loading,
	eGameOstrich_PlayingMovie,
	eGameOstrich_InScene,
	eGameOstrich_Total
};

#define eGameState_Unk00 0

extern _GameOstrich gGameOstrich;

void zGameSetOstrich(_GameOstrich o);
void zGameModeSwitch(eGameMode modeNew);
void zGameStateSwitch(int theNewState);

#endif