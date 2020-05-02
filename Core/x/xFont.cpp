#include "xFont.h"

#include "xstransvc.h"
#include "xModel.h"
#include "xMemMgr.h"

#include "print.h"

#include <rwcore.h>
#include <string.h>

namespace
{
struct font_asset
{
    unsigned int tex_id;
    unsigned short u;
    unsigned short v;
    unsigned char du;
    unsigned char dv;
    unsigned char line_size;
    unsigned char baseline;
    struct
    {
        short x;
        short y;
    } space;
    unsigned int flags;
    unsigned char char_set[128];
    struct
    {
        unsigned char offset;
        unsigned char size;
    } char_pos[127];
};

struct font_data
{
    font_asset *asset;
    unsigned int index_max;
    unsigned char char_index[256];
    float iwidth;
    float iheight;
    basic_rect<float> tex_bounds[127];
    basic_rect<float> bounds[127];
    xVec2 dstfrac[127];
    RwTexture *texture;
    RwRaster *raster;
};

struct model_pool
{
    RwMatrix mat[8];
    xModelInstance model[8];
};

struct model_cache_entry
{
    unsigned int id;
    unsigned int order;
    xModelInstance *model;
};

const char *default_font_texture[] =
{
    "font_sb",
    "font1_sb",
    "font_numbers"
};

font_asset default_font_assets[] =
{
    1, 0, 0, 18, 22, 14, 0, { 0, 0 }, 0x1,
    {
        '~', '{', '}', '#', 'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1',
        '2', '3', '4', '5', '6', '7', '8', '9',
        '?', '!', '.', ',', '-', ':', '_', '"',
        '\'','&', '(', ')', '<', '>', '/', '%',
        'ü', 'û', 'ù', 'â', 'ä', 'à', 'ê', 'è',
        'é', 'î', 'ö', 'ô', 'ç', 'ß', '+'
    }, {},

    0, 0, 0, 18, 22, 14, 0, { 1, 0 }, 0,
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '?', '!',
        '.', ',', ';', ':', '+', '-', '=', '/',
        '&', '(', ')', '%', '"', '\'','_', '<',
        '>', '*', '[', ']', 'Ü', 'Û', 'Ù', 'Â',
        'Ä', 'À', 'Ê', 'È', 'É', 'Î', 'Ö', 'Ô',
        'Ç', 'ß', 'ü', 'û', 'ù', 'â', 'ä', 'à',
        'ê', 'è', 'é', 'î', 'ö', 'ô', 'ç', '~',
        '©', '®', '™', '@', '|'
    }, {},

    1, 148, 232, 6, 8, 18, 0, { 0, 0 }, 0x9,
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', ',', '.', '/', '*',
        '+', '-', '=', ':', ';', '%', '<', '>',
        '[', ']', '|', '(', ')', '_'
    }, {},

    2, 0, 0, 32, 32, 4, 0, { 0, 0 }, 0,
    {
        '1', '2', '3', '4', '5', '6', '7', '8',
        '9', '0', '/'
    }, {}
};

font_data active_fonts[4];
unsigned int active_fonts_size;

basic_rect<int> find_bounds(const iColor_tag *bits, const basic_rect<int> &r, int pitch)
{
    int diff = pitch - r.w;

    const iColor_tag *endp = bits + pitch * r.h;
    const iColor_tag *p = bits;

    int pmode = (p->r == p->g && p->g == p->b && p->r >= 240) ? 1 : 0;

    int minx = r.x + r.w;
    int maxx = r.x - 1;
    int miny = r.y + r.h;
    int maxy = r.y - 1;

    int y = r.y;

    while (p != endp)
    {
        const iColor_tag *endline = p + r.w;
        int x = r.x;

        while (p != endline)
        {
            if ((pmode && p->a) || (!pmode && p->r))
            {
                minx = (x < minx) ? x : minx;
                maxx = (x > maxx) ? x : maxx;
                miny = (y < miny) ? y : miny;
                maxy = (y > maxy) ? y : maxy;
            }

            p++;
            x++;
        }

        p += diff;
        y++;
    }

    basic_rect<int> b;
    b.x = minx;
    b.y = miny;
    b.w = maxx + 1 - minx;
    b.h = maxy + 1 - miny;

    return b;
}

unsigned char reset_font_spacing(font_asset &a)
{
    RwTexture *tex = (RwTexture *)xSTFindAsset(a.tex_id, NULL);

    if (!tex)
    {
        return 0;
    }

    basic_rect<int> char_bounds;
    char_bounds.w = a.du;
    char_bounds.h = a.dv;

    unsigned char baseline_count[256];

    memset(baseline_count, 0, sizeof(baseline_count));

    a.baseline = 0;

    int width = tex->raster->width;
    RwImage *image = RwImageCreate(width, tex->raster->height, 32);

    if (!image)
    {
        return 0;
    }

    RwImageAllocatePixels(image);
    RwImageSetFromRaster(image, tex->raster);

    iColor_tag *bits = (iColor_tag *)image->cpPixels;

    for (int i = 0; a.char_set[i] != 0; i++)
    {
        if (a.flags & 0x4)
        {
            char_bounds.x = a.u + char_bounds.w * (i / a.line_size);
            char_bounds.y = a.v + char_bounds.h * (i - ((i / a.line_size) * a.line_size));
        }
        else
        {
            char_bounds.x = a.u + char_bounds.w * (i - ((i / a.line_size) * a.line_size));
            char_bounds.y = a.v + char_bounds.h * (i / a.line_size);
        }

        basic_rect<int> r = find_bounds(&bits[width * char_bounds.y + char_bounds.x],
                                        char_bounds, width);

        if (a.flags & 0x8)
        {
            a.char_pos[i].offset = 0;
            a.char_pos[i].size = char_bounds.w;
        }
        else
        {
            a.char_pos[i].offset = r.x - char_bounds.x;
            a.char_pos[i].size = r.w;
        }

        int baseline = r.h + (char_bounds.y - r.y + 1);

        if (++baseline_count[baseline] > baseline_count[a.baseline])
        {
            a.baseline = baseline;
        }
    }

    RwImageDestroy(image);
    return 1;
}

basic_rect<float> get_tex_bounds(const font_data &fd, unsigned char char_index)
{
    font_asset &a = *fd.asset;
    basic_rect<float> b;

    if (a.flags & 0x4)
    {
        b.x = char_index / a.line_size;
        b.y = char_index - (char_index / a.line_size) * a.line_size;
    }
    else
    {
        b.y = char_index / a.line_size;
        b.x = char_index - (char_index / a.line_size) * a.line_size;
    }

    b.x = a.char_pos[char_index * 2].offset;
    b.y = a.dv * b.y + a.v;
    b.w = a.char_pos[char_index * 2].size - 0.5f;
    b.h = a.dv - 0.5f;

    b.scale(fd.iwidth, fd.iheight);

    return b;
}

basic_rect<float> get_bounds(const font_data &fd, unsigned char char_index)
{
    font_asset &a = *fd.asset;
    basic_rect<float> b;

    b.x = 0.0f;
    b.y = (float)-a.baseline / a.dv;
    b.w = (float)(a.char_pos[char_index * 2].size + a.space.x) / (a.du + a.space.x);
    b.h = 1.0f;

    return b;
}

unsigned char init_font_data(font_data &fd)
{
    font_asset &a = *fd.asset;

    fd.texture = (RwTexture *)xSTFindAsset(a.tex_id, NULL);

    if (!fd.texture)
    {
        return 0;
    }

    RwTextureSetFilterMode(fd.texture, rwFILTERLINEAR);

    fd.raster = RwTextureGetRaster(fd.texture);
    fd.iwidth = 1.0f / RwRasterGetWidth(fd.raster);
    fd.iheight = 1.0f / RwRasterGetHeight(fd.raster);

    memset(&fd.char_index, 0xFF, sizeof(fd.char_index));

    fd.index_max = 0;

    while (a.char_set[fd.index_max])
    {
        unsigned char i = fd.index_max;
        unsigned char c = a.char_set[i];

        fd.char_index[c] = fd.index_max;

        if ((a.flags & 0x1) && c >= 'A' && c <= 'Z')
        {
            fd.char_index[c + 32] = i;
        }
        else if ((a.flags & 0x2) && c >= 'a' && c <= 'z')
        {
            fd.char_index[c - 32] = i;
        }

        fd.tex_bounds[i] = get_tex_bounds(fd, i);
        fd.bounds[i] = get_bounds(fd, i);

        fd.dstfrac[i].x = (float)a.char_pos[i].size / (a.char_pos[i].size + a.space.x);
        fd.dstfrac[i].y = (float)a.dv / (a.dv + a.space.y);

        fd.index_max++;
    }

    unsigned int tail_index = fd.index_max;

    if (fd.char_index[' '] == 0xFF)
    {
        fd.char_index[' '] = tail_index;
        fd.tex_bounds[tail_index].assign(0.0f, 0.0f, 0.0f, 0.0f);
        fd.bounds[tail_index].assign(0.0f, (float)-a.baseline / a.dv,
                                     (a.flags & 0x8) ? 1.0f : 0.5f, 1.0f);

        tail_index++;
    }

    if (fd.char_index['\n'] == 0xFF)
    {
        fd.char_index['\n'] = tail_index;
        fd.tex_bounds[tail_index].assign(0.0f, 0.0f, 0.0f, 0.0f);
        fd.bounds[tail_index].assign(0.0f, (float)-a.baseline / a.dv,
                                     0.0f, 1.0f);
    }

    return 1;
}

model_cache_entry model_cache[8];
unsigned char model_cache_inited;

void init_model_cache()
{
    model_cache_inited = 1;

    void *data = xMemAlloc(gActiveHeap, sizeof(model_pool), 16);
    memset(data, 0, sizeof(model_pool));

    model_pool &pool = *(model_pool *)data;

    for (int i = 0; i < 8; i++)
    {
        xModelInstance &model = pool.model[i];
        model_cache_entry &e = model_cache[i];

        e.order = 0;
        e.id = 0;
        e.model = &model;
        e.model->Mat = &pool.mat[i];
        e.model->Flags = 0x1;
        e.model->BoneCount = 1;
        e.model->shadowID = 0xDEADBEEF;
    }
}
}

void xfont::init()
{
    active_fonts_size = 0;

    for (unsigned int i = 0;
         i < (sizeof(default_font_assets) / sizeof(font_asset));
         i++)
    {
        font_asset &fa = default_font_assets[i];

        if (fa.tex_id < (sizeof(default_font_texture) / sizeof(char *)))
        {
            fa.tex_id = xStrHash(default_font_texture[fa.tex_id]);
        }

        xSTFindAsset(fa.tex_id, NULL);

        if (reset_font_spacing(fa))
        {
            font_data &fd = active_fonts[active_fonts_size];

            fd.asset = &fa;

            if (init_font_data(fd))
            {
                active_fonts_size++;
            }
        }
    }

    init_model_cache();
}

#define SUBSTR(text) (text), sizeof((text)) - 1

namespace
{
void parse_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void reset_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void parse_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void reset_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void parse_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void reset_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void parse_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void reset_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void parse_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void reset_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void parse_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void reset_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void parse_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti);
void reset_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti);
void parse_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                            const xtextbox::split_tag &ti);
void reset_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                            const xtextbox::split_tag &ti);
void parse_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti);
void reset_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti);
void parse_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void reset_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void parse_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void reset_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void reset_tag_all(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void parse_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void reset_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void parse_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void reset_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void parse_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void reset_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void parse_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void reset_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti);
void parse_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti);
void reset_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti);
void parse_tag_open_curly(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti);
void parse_tag_newline(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                       const xtextbox::split_tag &ti);
void parse_tag_tab(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void parse_tag_word_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti);
void parse_tag_page_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti);
void parse_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void reset_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);
void parse_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void reset_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void parse_tag_insert(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti);
void parse_tag_insert_hash(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti);
void parse_tag_pop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti);
void parse_tag_timer(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti);

void parse_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_alpha");
}

void reset_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_alpha");
}

void parse_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_red");
}

void reset_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_red");
}

void parse_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_green");
}

void reset_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_green");
}

void parse_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_blue");
}

void reset_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_blue");
}

void parse_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_width");
}

void reset_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_width");
}

void parse_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_height");
}

void reset_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_height");
}

void parse_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_left_indent");
}

void reset_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_left_indent");
}

void parse_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                            const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_right_indent");
}

void reset_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                            const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_right_indent");
}

void parse_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_tab_stop");
}

void reset_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_tab_stop");
}

void parse_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_xspace");
}

void reset_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_xspace");
}

void parse_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_yspace");
}

void reset_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_yspace");
}

void reset_tag_all(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_all");
}

void parse_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_color");
}

void reset_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_color");
}

void parse_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_font");
}

void reset_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_font");
}

void parse_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_wrap");
}

void reset_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_wrap");
}

void parse_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_xjustify");
}

void reset_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_xjustify");
}

void parse_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_yjustify");
}

void reset_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                        const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_yjustify");
}

void parse_tag_open_curly(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_open_curly");
}

void parse_tag_newline(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                       const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_newline");
}

void parse_tag_tab(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_tab");
}

void parse_tag_word_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_word_break");
}

void parse_tag_page_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                          const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_page_break");
}

void parse_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_model");
}

void reset_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_model");
}

void parse_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_tex");
}

void reset_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("reset_tag_tex");
}

void parse_tag_insert(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_insert");
}

void parse_tag_insert_hash(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_insert_hash");
}

void parse_tag_pop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                   const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_pop");
}

void parse_tag_timer(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    BFBBSTUB("parse_tag_timer");
}

xtextbox::tag_type format_tags_buffer[2][128] =
{
    SUBSTR(""), parse_tag_open_curly, NULL, NULL,
    SUBSTR("a"), parse_tag_alpha, reset_tag_alpha, NULL,
    SUBSTR("all"), NULL, reset_tag_all, NULL,
    SUBSTR("alpha"), parse_tag_alpha, reset_tag_alpha, NULL,
    SUBSTR("b"), parse_tag_blue, reset_tag_blue, NULL,
    SUBSTR("blue"), parse_tag_blue, reset_tag_blue, NULL,
    SUBSTR("c"), parse_tag_color, reset_tag_color, NULL,
    SUBSTR("color"), parse_tag_color, reset_tag_color, NULL,
    SUBSTR("f"), parse_tag_font, reset_tag_font, NULL,
    SUBSTR("font"), parse_tag_font, reset_tag_font, NULL,
    SUBSTR("g"), parse_tag_green, reset_tag_green, NULL,
    SUBSTR("green"), parse_tag_green, reset_tag_green, NULL,
    SUBSTR("h"), parse_tag_height, reset_tag_height, NULL,
    SUBSTR("height"), parse_tag_height, reset_tag_height, NULL,
    SUBSTR("i"), parse_tag_insert, NULL, NULL,
    SUBSTR("ih"), parse_tag_insert_hash, NULL, NULL,
    SUBSTR("insert"), parse_tag_insert, NULL, NULL,
    SUBSTR("left_indent"), parse_tag_left_indent, reset_tag_left_indent, NULL,
    SUBSTR("li"), parse_tag_left_indent, reset_tag_left_indent, NULL,
    SUBSTR("model"), parse_tag_model, reset_tag_model, NULL,
    SUBSTR("n"), parse_tag_newline, NULL, NULL,
    SUBSTR("newline"), parse_tag_newline, NULL, NULL,
    SUBSTR("page_break"), parse_tag_page_break, NULL, NULL,
    SUBSTR("pb"), parse_tag_page_break, NULL, NULL,
    SUBSTR("pop"), parse_tag_pop, NULL, NULL,
    SUBSTR("r"), parse_tag_red, reset_tag_red, NULL,
    SUBSTR("red"), parse_tag_red, reset_tag_red, NULL,
    SUBSTR("ri"), parse_tag_right_indent, reset_tag_right_indent, NULL,
    SUBSTR("right_indent"), parse_tag_right_indent, reset_tag_right_indent, NULL,
    SUBSTR("t"), parse_tag_tab, NULL, NULL,
    SUBSTR("tab"), parse_tag_tab, NULL, NULL,
    SUBSTR("tab_stop"), parse_tag_tab_stop, reset_tag_tab_stop, NULL,
    SUBSTR("tex"), parse_tag_tex, reset_tag_tex, NULL,
    SUBSTR("timer"), parse_tag_timer, NULL, NULL,
    SUBSTR("ts"), parse_tag_tab_stop, reset_tag_tab_stop, NULL,
    SUBSTR("w"), parse_tag_width, reset_tag_width, NULL,
    SUBSTR("wb"), parse_tag_word_break, NULL, NULL,
    SUBSTR("width"), parse_tag_width, reset_tag_width, NULL,
    SUBSTR("word_break"), parse_tag_word_break, NULL, NULL,
    SUBSTR("wrap"), parse_tag_wrap, reset_tag_wrap, NULL,
    SUBSTR("xj"), parse_tag_xjustify, reset_tag_xjustify, NULL,
    SUBSTR("xjustify"), parse_tag_xjustify, reset_tag_xjustify, NULL,
    SUBSTR("xs"), parse_tag_xspace, reset_tag_xspace, NULL,
    SUBSTR("xspace"), parse_tag_xspace, reset_tag_xspace, NULL,
    SUBSTR("yj"), parse_tag_xjustify, reset_tag_xjustify, NULL,
    SUBSTR("yjustify"), parse_tag_xjustify, reset_tag_xjustify, NULL,
    SUBSTR("ys"), parse_tag_xspace, reset_tag_xspace, NULL,
    SUBSTR("yspace"), parse_tag_xspace, reset_tag_xspace, NULL
};

xtextbox::tag_type *format_tags = format_tags_buffer[0];
unsigned int format_tags_size = 48;
}

void xtextbox::register_tags(const tag_type *t, unsigned int size)
{
    const tag_type *s1 = format_tags;
    const tag_type *s2 = t;
    const tag_type *end1 = format_tags + format_tags_size;
    const tag_type *end2 = t + size;

    format_tags = (format_tags == format_tags_buffer[0]) ?
        format_tags_buffer[1] : format_tags_buffer[0];

    tag_type *d = format_tags;

    while (s1 < end1 && s2 < end2)
    {
        int c = icompare(s1->name, s2->name);

        if (c < 0)
        {
            *d = *s1;
            s1++;
        }
        else if (c > 0)
        {
            *d = *s2;
            s2++;
        }
        else
        {
            *d = *s2;
            s1++;
            s2++;
        }

        d++;
    }

    while (s1 < end1)
    {
        *d = *s1;
        d++;
        s1++;
    }

    while (s2 < end2)
    {
        *d = *s2;
        d++;
        s2++;
    }

    format_tags_size = (unsigned int)(d - format_tags);
}