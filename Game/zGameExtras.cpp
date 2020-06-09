#include "zGameExtras.h"

#include "xEnt.h"

#include "print.h"

static xEnt *sGalleryTitle;

void zGameExtras_NewGameReset()
{
    BFBBSTUB("zGameExtras_NewGameReset");
}

void zGame_HackGalleryInit()
{
    BFBBSTUB("zGame_HackGalleryInit");
}

unsigned int zGame_HackIsGallery()
{
    if (sGalleryTitle && xEntIsVisible(sGalleryTitle))
    {
        return 1;
    }

    return 0;
}