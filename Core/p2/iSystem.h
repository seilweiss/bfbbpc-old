#ifndef ISYSTEM_H
#define ISYSTEM_H

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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
void iVSync();

#endif