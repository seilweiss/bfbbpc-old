#ifndef XSND_H
#define XSND_H

void xSndSceneInit();
void xSndInit();
void xSndPauseAll(unsigned int pause_effects, unsigned int pause_streams);
void xSndUpdate();
void xSndStopAll(unsigned int mask);
void xSndExit();

#endif