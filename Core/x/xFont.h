#ifndef XFONT_H
#define XFONT_H

#include "xString.h"
#include "xMath2.h"
#include "iColor.h"

struct xfont
{
    unsigned int id;
    float width;
    float height;
    float space;
    iColor_tag color;
    basic_rect<float> clip;

    static void init();
};

struct xtextbox
{
    struct jot;

    typedef void(*render_callback)(jot &, xtextbox &, float, float);
    typedef void(*update_callback)(jot &, xtextbox &, xtextbox &);

    struct callback
    {
        render_callback render;
        update_callback layout_update;
        update_callback render_update;
    };

    struct tag_type;

    struct jot
    {
        substr s;
        struct
        {
            bool invisible  : 1;
            bool ethereal   : 1;
            bool merge      : 1;
            bool word_break : 1;
            bool word_end   : 1;
            bool line_break : 1;
            bool stop       : 1;
            bool tab        : 1;
            bool insert     : 1;
            bool dynamic    : 1;
            bool page_break : 1;
            bool stateful   : 1;
            unsigned short dummy : 4;
        } flag;
        unsigned short context_size;
        void *context;
        basic_rect<float> bounds;
        basic_rect<float> render_bounds;
        callback *cb;
        tag_type *tag;
    };

    struct split_tag
    {
        substr tag;
        substr name;
        substr action;
        substr value;
    };

    typedef void(*tag_type_callback)(jot &a, const xtextbox &tb, const xtextbox &ctb,
                                     const split_tag &ti);

    struct tag_type
    {
        substr name;
        tag_type_callback parse_tag;
        tag_type_callback reset_tag;
        void *context;
    };

    xfont font;
    basic_rect<float> bounds;
    unsigned int flags;
    float line_space;
    float tab_stop;
    float left_indent;
    float right_indent;
    callback *cb;
    void *context;
    char **texts;
    unsigned int *text_sizes;
    unsigned int texts_size;
    substr text;
    unsigned int text_hash;

    static void register_tags(const tag_type *t, unsigned int size);
};

#endif