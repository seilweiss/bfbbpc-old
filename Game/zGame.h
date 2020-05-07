#ifndef ZGAME_H
#define ZGAME_H

extern unsigned int gGameSfxReport;
extern unsigned int gSoak;

void zGameInit(unsigned int theSceneID);
void zGameExit();
void zGameSetup();
void zGameLoop();

#endif