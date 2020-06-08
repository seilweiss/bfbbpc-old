#ifndef XDEBUG_H
#define XDEBUG_H

extern unsigned int gFrameCount;

void xprintf(const char *format, ...);
void xDebugInit();
void xDebugUpdate();
void xDebugExit();
void xDebugTimestampScreen();

#endif