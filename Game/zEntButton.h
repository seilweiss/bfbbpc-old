#ifndef ZENTBUTTON_H
#define ZENTBUTTON_H

#include "zEnt.h"
#include "xEntMotion.h"

struct zEntButtonAsset
{
    unsigned int modelPressedInfoID;
    unsigned int actMethod;
    int initButtonState;
    int isReset;
    float resetDelay;
    unsigned int buttonActFlags;
};

struct _zEntButton : zEnt
{
    zEntButtonAsset *basset;
    xEntMotion motion;
    unsigned int state;
    float speed;
    unsigned int oldState;
    int oldMotState;
    float counter;
    xModelInstance *modelPressed;
    float holdTimer;
    unsigned int hold;
    float topHeight;
};

struct xScene;

void zEntButton_Init(void *ent, void *asset);
void zEntButton_Setup(_zEntButton *ent, xScene *sc);
void zEntButton_Reset(_zEntButton *ent, xScene *sc);

#endif