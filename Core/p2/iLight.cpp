#include "iLight.h"

RpWorld *gLightWorld;

void iLightInit(RpWorld *world)
{
    gLightWorld = world;
}