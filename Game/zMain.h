#ifndef ZMAIN_H
#define ZMAIN_H

#include "zGlobals.h"

extern zGlobals globals;
extern xGlobals *xglobals;

extern float gSkipTimeCutscene;
extern float gSkipTimeFlythrough;

void zMainFirstScreen(int mode);
void zMainShowProgressBar();
void zMainMemCardRenderText(const char *text, bool enabled);
void zMainMemCardSpaceQuery();

#endif