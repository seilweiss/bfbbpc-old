#ifndef ZENTPLAYEROOBSTATE_H
#define ZENTPLAYEROOBSTATE_H

#include "xIni.h"

namespace oob_state
{
void load_settings(xIniFile &ini);
void init();
bool render();
void fx_render();
}

#endif