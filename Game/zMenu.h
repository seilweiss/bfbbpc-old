#ifndef ZMENU_H
#define ZMENU_H

void zMenuInit(unsigned int theSceneID);
void zMenuExit();
void zMenuSetup();
unsigned int zMenuLoop();
int zMenuIsFirstBoot();

#endif