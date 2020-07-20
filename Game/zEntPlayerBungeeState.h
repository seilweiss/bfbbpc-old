#ifndef ZENTPLAYERBUNGEESTATE_H
#define ZENTPLAYERBUNGEESTATE_H

#include "xIni.h"
#include "xAnim.h"

namespace bungee_state
{
void load_settings(xIniFile &ini);
void init();
void reset();
void insert_animations(xAnimTable &table);
}

#endif