#ifndef XSCRFX_H
#define XSCRFX_H

#include "iColor.h"
#include "xCamera.h"

#include <rwcore.h>

void xScrFxInit();
void xScrFxUpdate(RwCamera *cam, float dt);
void xScrFxRender(RwCamera *cam);
void xScrFxDrawScreenSizeRectangle();
void xScrFxFade(iColor_tag *base, iColor_tag *dest, float seconds, void(*callback)(),
                int hold);
void xScrFxLetterboxReset();
void xScrFxLetterBoxSetSize(float size);
void xScrFxLetterBoxSetAlpha(unsigned char alpha);
void xScrFxLetterbox(int enable);
void xScrFXFullScreenGlareRender();
void xScrFXGlareRender(xCamera *cam);

#endif