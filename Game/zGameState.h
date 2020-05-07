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

#define eGameState_Unk00 0

void zGameModeSwitch(eGameMode modeNew);
void zGameStateSwitch(int theNewState);

#endif