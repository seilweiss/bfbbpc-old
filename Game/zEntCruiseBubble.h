#ifndef ZENTCRUISEBUBBLE_H
#define ZENTCRUISEBUBBLE_H

#include "xAnim.h"

namespace cruise_bubble
{
void init();
void reset();
void render_screen();
void insert_player_animations(xAnimTable &table);
xAnimTable *anim_table();
}

#endif