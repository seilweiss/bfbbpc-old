#ifndef XSCRFX_H
#define XSCRFX_H

#include "iColor.h"

void xScrFxInit();
void xScrFxFade(iColor_tag *base, iColor_tag *dest, float seconds, void(*callback)(),
                int hold);
void xScrFxLetterboxReset();
void xScrFxLetterBoxSetSize(float size);
void xScrFxLetterBoxSetAlpha(unsigned char alpha);

#endif