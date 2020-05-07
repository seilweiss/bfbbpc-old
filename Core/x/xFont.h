#ifndef XFONT_H
#define XFONT_H

#include "xString.h"
#include "xMath2.h"
#include "iColor.h"

#include <rwcore.h>

struct xfont
{
    unsigned int id;
    float width;
    float height;
    float space;
    iColor_tag color;
    basic_rect<float> clip;

    static void set_render_state(RwRaster *raster);
    static void restore_render_state();
    static void init();
    basic_rect<float> bounds(char c) const;
    void start_render() const;
    void stop_render() const;
    void irender(const char *text, unsigned int text_size, float x, float y) const;
    void irender(const char *text, float x, float y) const;
    static xfont create(unsigned int id, float width, float height, float space,
                        iColor_tag color, const basic_rect<float> &clip);
};

struct xtextbox
{
    struct jot;

    typedef void(*render_callback)(const jot &j, const xtextbox &tb, float x, float y);
    typedef void(*update_callback)(const jot &j, xtextbox &tb, const xtextbox &ctb);

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

        union
        {
            struct
            {
                bool invisible : 1;
                bool ethereal : 1;
                bool merge : 1;
                bool word_break : 1;
                bool word_end : 1;
                bool line_break : 1;
                bool stop : 1;
                bool tab : 1;
                bool insert : 1;
                bool dynamic : 1;
                bool page_break : 1;
                bool stateful : 1;
            };
            unsigned short dummy;
        } flag;

        unsigned short context_size;
        void *context;
        basic_rect<float> bounds;
        basic_rect<float> render_bounds;
        callback *cb;
        tag_type *tag;

        void intersect_flags(const jot &a);
        void reset_flags();
    };

    struct jot_line
    {
        basic_rect<float> bounds;
        float baseline;
        unsigned int first;
        unsigned int last;
        bool page_break;
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

    struct layout;

    xfont font;
    basic_rect<float> bounds;
    unsigned int flags;
    float line_space;
    float tab_stop;
    float left_indent;
    float right_indent;
    callback *cb;
    void *context;
    const char **texts;
    const unsigned int *text_sizes;
    unsigned int texts_size;
    substr text;
    unsigned int text_hash;

    static callback text_cb;

    static tag_type *find_format_tag(const substr &s);
    static tag_type *find_format_tag(const substr &s, int &index);
    static void text_render(const jot &j, const xtextbox &tb, float x, float y);
    void set_text(const char *text);
    void set_text(const char *text, unsigned int size);
    void set_text(const char **texts, unsigned int size);
    void set_text(const char **texts, const unsigned int *text_sizes, unsigned int size);
    layout &temp_layout(bool cache) const;
    void render(bool cache) const;
    void render(layout &l, int begin_jot, int end_jot) const;
    float yextent(bool cache) const;
    float yextent(const layout &l, int begin_jot, int end_jot) const;
    float yextent(float max, int &size, const layout &l, int begin_jot, int end_jot) const;
    static xtextbox create(const xfont &font, const basic_rect<float> &bounds,
                           unsigned int flags, float line_space, float tab_stop,
                           float left_indent, float right_indent);
    static void register_tags(const tag_type *t, unsigned int size);
};

struct xtextbox::layout
{
    xtextbox tb;
    jot _jots[512];
    unsigned int _jots_size;
    jot_line _lines[128];
    unsigned int _lines_size;
    unsigned char context_buffer[1024];
    unsigned int context_buffer_size;
    unsigned short dynamics[64];
    unsigned int dynamics_size;

    bool changed(const xtextbox &ctb);
    void refresh(const xtextbox &tb, bool force);
    void render(const xtextbox &ctb, int begin_jot, int end_jot);
    float yextent(float max, int &size, int begin_jot, int end_jot) const;
    void calc(const xtextbox &ctb, unsigned int start_text);
    void erase_jots(unsigned int begin_jot, unsigned int end_jot);
    void trim_line(jot_line &line);
    void merge_line(jot_line &line);
    void bound_line(jot_line &line);
    bool fit_line();
    void next_line();
};

static basic_rect<float> screen_bounds = { 0.0f, 0.0f, 1.0f, 1.0f };

void render_fill_rect(const basic_rect<float> &bounds, iColor_tag color);

#endif