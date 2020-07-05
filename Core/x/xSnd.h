#ifndef XSND_H
#define XSND_H

enum sound_listener_game_mode
{
	SND_LISTENER_MODE_PLAYER,
	SND_LISTENER_MODE_CAMERA
};

void xSndSceneInit();
void xSndInit();
void xSndPauseAll(unsigned int pause_effects, unsigned int pause_streams);
void xSndUpdate();
void xSndStopAll(unsigned int mask);
void xSndSelectListenerMode(sound_listener_game_mode listenerMode);
void xSndExit();

#endif