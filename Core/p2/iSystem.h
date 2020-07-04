#ifndef ISYSTEM_H
#define ISYSTEM_H

//#ifdef _WIN32
#if 0
#include "iWindow.h"

#define SCREEN_WIDTH gWindowWidth
#define SCREEN_HEIGHT gWindowHeight
#else
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#endif

#define FULLSCREEN 0

#define FRAMES_PER_SEC 60.0f
#define ONE_FRAME      (1.0f / FRAMES_PER_SEC)

#define CAMERA_FOV 75.0f

inline float NSCREENX(float x)
{
    return (1.0f / SCREEN_WIDTH) * x;
}

inline float NSCREENY(float y)
{
    return (1.0f / SCREEN_HEIGHT) * y;
}

struct RwMemoryFunctions;

RwMemoryFunctions *psGetMemoryFunctions();

void iSystemInit(unsigned int options);
void iSystemExit();
void iVSync();

#endif