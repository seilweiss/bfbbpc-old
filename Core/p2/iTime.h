#ifndef ITIME_H
#define ITIME_H

#include <time.h>

typedef clock_t iTime;

void iTimeInit();
void iTimeExit();
iTime iTimeGet();
float iTimeDiffSec(iTime time);
float iTimeDiffSec(iTime t0, iTime t1);
void iTimeGameAdvance(float elapsed);
void iTimeSetGame(float time);
void iFuncProfileParse(const char *, int);

#endif