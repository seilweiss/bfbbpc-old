#ifndef ZMENU_H
#define ZMENU_H

extern bool menu_fmv_played;

int zMenuRunning();
void zMenuInit(unsigned int theSceneID);
void zMenuExit();
void zMenuSetup();
void zMenuFirstBootSet(int firstBoot);
void zMenuFMVPlay(char *filename, unsigned int buttons, float time, bool skippable,
                  bool lockController);
unsigned int zMenuLoop();
int zMenuIsFirstBoot();

#endif