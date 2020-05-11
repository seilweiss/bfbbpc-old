#ifndef ZUIFONT_H
#define ZUIFONT_H

#include "zUI.h"

struct zUIFontAsset : zUIAsset
{
    unsigned short uiFontFlags;
    unsigned char mode;
    unsigned char fontID;
    unsigned int textAssetID;
    unsigned char bcolor[4];
    unsigned char color[4];
    unsigned short inset[4];
    unsigned short space[2];
    unsigned short cdim[2];
    unsigned int max_height;
};

struct zUIFont : _zUI
{
    zUIFontAsset *fasset;
    unsigned short uiFontFlags;
    unsigned short uiFontHackFlags;
    unsigned int text_id;
    float yscroll;
    unsigned int text_index;
    char *text;
};

void zUIFontInit();
void zUIFont_Init(void *ent, void *asset);

#endif