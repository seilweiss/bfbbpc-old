#ifndef ZTEXTBOX_H
#define ZTEXTBOX_H

#include "xBase.h"
#include "xDynAsset.h"
#include "xMath2.h"
#include "xFont.h"

#include <rwcore.h>

struct ztextbox : xBase
{
    struct asset_type : xDynAsset
    {
        struct color_type
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };

        unsigned int text;
        basic_rect<float> bounds;
        unsigned int font;
        struct
        {
            float width;
            float height;
        } size;
        struct
        {
            float x;
            float y;
        } space;
        color_type color;
        struct
        {
            float left;
            float top;
            float right;
            float bottom;
        } inset;
        enum
        {
            XJ_LEFT,
            XJ_CENTER,
            XJ_RIGHT
        } xjustify;
        enum
        {
            EX_UP,
            EX_CENTER,
            EX_DOWN,
            MAX_EX
        } expand;
        float max_height;
        struct
        {
            unsigned int type;
            color_type color;
            unsigned int texture;
        } backdrop;
    };

    struct
    {
        bool active          : 1;
        bool dirty           : 1;
        bool show_backdrop   : 1;
        bool visible         : 1;
        bool hack_invisible  : 1;
    } flag;
    asset_type *asset;
    xtextbox tb;
    char *segments[16];
    unsigned int segments_size;
    ztextbox *next;
    ztextbox *prev;
    RwRaster *bgtex;

    static void init();
    static void render_all();
};

#endif