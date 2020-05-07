#include "xFont.h"

#include "xstransvc.h"
#include "xModel.h"
#include "xMemMgr.h"
#include "iTime.h"
#include "iSystem.h"
#include "xMath.h"

#include "print.h"

#include <rwcore.h>
#include <string.h>

#define SUBSTR(text) (text), sizeof((text)) - 1

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

RwIm2DVertex vert_buffer[120];
unsigned int vert_buffer_used;

float rcz;
float nsz;

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

        int baseline = r.h + (r.y - char_bounds.y + 1);

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

    b.x = a.char_pos[char_index].offset + a.du * b.x + a.u;
    b.y = a.dv * b.y + a.v;
    b.w = a.char_pos[char_index].size - 0.5f;
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
    b.w = (float)(a.char_pos[char_index].size + a.space.x) / (a.du + a.space.x);
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

void start_tex_render(unsigned int id)
{
    rcz = 1.0f / RwCameraGetNearClipPlane(RwCameraGetCurrentCamera());
    nsz = RwIm2DGetNearScreenZ();

    xfont::set_render_state(active_fonts[id].raster);
}

void tex_flush()
{
    if (vert_buffer_used)
    {
        RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, vert_buffer, vert_buffer_used);
        vert_buffer_used = 0;
    }
}

void stop_tex_render()
{
    tex_flush();
    xfont::restore_render_state();
}

void set_vert(RwIm2DVertex &vert, float x, float y, float u, float v,
              iColor_tag color)
{
    RwIm2DVertexSetScreenX(&vert, x);
    RwIm2DVertexSetScreenY(&vert, y);
    //RwIm2DVertexSetScreenZ(&vert, nsz);
    RwIm2DVertexSetRecipCameraZ(&vert, rcz);
    RwIm2DVertexSetU(&vert, u, rcz);
    RwIm2DVertexSetV(&vert, v, rcz);
    RwIm2DVertexSetIntRGBA(&vert, color.r, color.g, color.b, color.a);
}

void tex_render(const basic_rect<float> &src, const basic_rect<float> &dst,
                const basic_rect<float> &clip, iColor_tag color)
{
    basic_rect<float> r = dst;
    basic_rect<float> rt = src;

    clip.clip(r, rt);

    if (!r.empty())
    {
        if (vert_buffer_used == 120)
        {
            RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, vert_buffer, 120);
            vert_buffer_used = 0;
        }

        RwIm2DVertex *vert = vert_buffer + vert_buffer_used;

        r.scale(SCREEN_WIDTH, SCREEN_HEIGHT);

        set_vert(vert[0],
                 r.x,
                 r.y,
                 rt.x,
                 rt.y,
                 color);

        set_vert(vert[1],
                 r.x,
                 r.y + r.h,
                 rt.x,
                 rt.y + rt.h,
                 color);

        set_vert(vert[2],
                 r.x + r.w,
                 r.y,
                 rt.x + rt.w,
                 rt.y,
                 color);

        vert[3] = vert[2];
        vert[4] = vert[1];

        set_vert(vert[5],
                 r.x + r.w,
                 r.y + r.h,
                 rt.x + rt.w,
                 rt.y + rt.h,
                 color);

        vert_buffer_used += 6;
    }
}

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

struct
{
    RwBool fogenable;
    RwBool vertexalphaenable;
    RwBool zwriteenable;
    RwBool ztestenable;
    RwBlendFunction srcblend;
    RwBlendFunction destblend;
    RwShadeMode shademode;
    RwRaster *textureraster;
    RwTextureFilterMode filter;
} oldrs;

RwRaster *set_tex_raster(RwRaster *raster)
{
    RwRaster *oldraster;

    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, (void *)&oldraster);

    if (raster == oldraster)
    {
        return raster;
    }

    tex_flush();
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)raster);

    return oldraster;
}

void char_render(unsigned char c, unsigned int font_id, const basic_rect<float> &bounds,
                 const basic_rect<float> &clip, iColor_tag color)
{
    font_data &fd = active_fonts[font_id];
    unsigned int charIndex = fd.char_index[c];

    if (charIndex < fd.index_max)
    {
        basic_rect<float> dst = fd.bounds[charIndex];
        dst.scale(bounds.w, bounds.h);
        dst.x += bounds.x;
        dst.y += bounds.y;
        dst.w *= fd.dstfrac[charIndex].x;
        dst.h *= fd.dstfrac[charIndex].y;

        tex_render(fd.tex_bounds[charIndex], dst, clip, color);
    }
}

substr text_delims = { SUBSTR(" \t\n{}=*+:;,") };

unsigned int parse_split_tag(xtextbox::split_tag &ti)
{
    ti.value.size = 0;
    ti.action.size = 0;
    ti.name.size = 0;

    substr s;
    s.text = ti.tag.text;
    s.size = ti.tag.size;
    s.text = ti.tag.text + 1;
    s.size = ti.tag.size - 1;

    ti.name.text = skip_ws(s);

    s.text = find_char(s, text_delims);

    if (!s.text)
    {
        return 0;
    }

    ti.name.size = s.text - ti.name.text;
    s.size -= ti.name.size;
    ti.action.text = skip_ws(s);

    if (!s.size)
    {
        return 0;
    }

    char c = *s.text;

    if (c == '\0' || c == '{')
    {
        return 0;
    }

    s.text++;
    s.size--;

    if (c == '}')
    {
        return ti.tag.size - s.size;
    }

    ti.action.size = 1;
    ti.value.text = skip_ws(s);

    s.text = find_char(s, '}');

    unsigned int size = s.text - ti.value.text;
    s.size -= size;

    if (!s.text)
    {
        return 0;
    }

    ti.value.size = size;

    rskip_ws(ti.value);

    s.text++;
    s.size--;

    return ti.tag.size - s.size;
}

const char *parse_next_tag_jot(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                               const char *text, unsigned int text_size)
{
    xtextbox::split_tag ti = {};
    ti.tag.text = text;
    ti.tag.size = text_size;

    unsigned int size = parse_split_tag(ti);

    if (!size)
    {
        return NULL;
    }

    a.s.text = text;
    a.s.size = size;
    a.flag.invisible = a.flag.ethereal = true;

    if ((icompare(ti.name, substr::create("~", 1)) == 0) ||
        (icompare(ti.name, substr::create("reset", 5)) == 0))
    {
        a.tag = xtextbox::find_format_tag(ti.value);

        if (a.tag && a.tag->reset_tag)
        {
            a.tag->reset_tag(a, tb, ctb, ti);
        }
    }
    else
    {
        a.tag = xtextbox::find_format_tag(ti.name);

        if (a.tag && a.tag->parse_tag)
        {
            a.tag->parse_tag(a, tb, ctb, ti);
        }
    }

    return text + size;
}

const char *parse_next_text_jot(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                                const char *text, unsigned int text_size)
{
    char c = *text;

    a.s.text = text;
    a.s.size = 1;
    a.flag.merge = true;

    if (c == '\n')
    {
        a.flag.line_break = true;
    }
    else if (c == '\t')
    {
        a.flag.tab = true;
    }
    else if (c == '-')
    {
        a.flag.word_end = true;
    }

    if (is_ws(c))
    {
        a.flag.invisible = a.flag.word_break = true;
    }

    a.bounds = tb.font.bounds(c);
    a.cb = &xtextbox::text_cb;
    a.context = NULL;
    a.context_size = 0;

    return a.s.text + a.s.size;
}

const char *parse_next_jot(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                           const char *text, unsigned int text_size)
{
    const char *r3;

    if ((*text != '{') ||
        !(r3 = parse_next_tag_jot(a, tb, ctb, text, text_size)))
    {
        r3 = parse_next_text_jot(a, tb, ctb, text, text_size);
    }

    a.flag.merge = (a.flag.merge && !(tb.flags & 0x80));

    return r3;
}

struct tex_args
{
    RwRaster *raster;
    float rot;
    basic_rect<float> src;
    basic_rect<float> dst;
    xVec2 off;
    enum
    {
        SCALE_FONT,
        SCALE_SCREEN,
        SCALE_SIZE,
        SCALE_FONT_WIDTH,
        SCALE_FONT_HEIGHT,
        SCALE_SCREEN_WIDTH,
        SCALE_SCREEN_HEIGHT
    } scale;
};

tex_args def_tex_args;

void reset_tex_args(tex_args &ta)
{
    ta.raster = NULL;
    ta.rot = 0.0f;
    ta.src = ta.dst = basic_rect<float>::m_Unit;
    ta.off.x = ta.off.y = 1.0f;
    ta.scale = tex_args::SCALE_FONT;
}

struct model_args
{
    xModelInstance *model;
    xVec3 rot;
    basic_rect<float> dst;
    xVec2 off;
    enum
    {
        SCALE_FONT,
        SCALE_SCREEN,
        SCALE_SIZE
    } scale;
};

model_args def_model_args;

void reset_model_args(model_args &ma)
{
    ma.model = NULL;
    ma.rot = xVec3::m_Null;
    ma.dst = basic_rect<float>::m_Unit;
    ma.off.x = ma.off.y = 1.0f;
    ma.scale = model_args::SCALE_FONT;
}

void start_layout(const xtextbox &tb)
{
    reset_tex_args(def_tex_args);
    reset_model_args(def_model_args);
}

void stop_layout(const xtextbox &tb)
{
    return;
}

void start_render(const xtextbox &tb)
{
    tb.font.start_render();
}

void stop_render(const xtextbox &tb)
{
    tb.font.stop_render();
}

struct tl_cache_entry
{
    unsigned int used;
    iTime last_used;
    xtextbox::layout tl;
};

tl_cache_entry tl_cache[3];
}

xtextbox::callback xtextbox::text_cb =
{
    xtextbox::text_render,
    NULL,
    NULL
};

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

void xfont::set_render_state(RwRaster *raster)
{
    RwRenderStateGet(rwRENDERSTATEFOGENABLE, (void *)&oldrs.fogenable);
    RwRenderStateGet(rwRENDERSTATESRCBLEND, (void *)&oldrs.srcblend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND, (void *)&oldrs.destblend);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)&oldrs.vertexalphaenable);
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, (void *)&oldrs.textureraster);
    RwRenderStateGet(rwRENDERSTATESHADEMODE, (void *)&oldrs.shademode);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, (void *)&oldrs.zwriteenable);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE, (void *)&oldrs.ztestenable);
    RwRenderStateGet(rwRENDERSTATETEXTUREFILTER, (void *)&oldrs.filter);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)raster);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERLINEAR);
}

void xfont::restore_render_state()
{
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)oldrs.fogenable);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)oldrs.srcblend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)oldrs.destblend);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)oldrs.vertexalphaenable);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)oldrs.textureraster);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)oldrs.shademode);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)oldrs.zwriteenable);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)oldrs.ztestenable);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)oldrs.filter);
}

basic_rect<float> xfont::bounds(char c) const
{
    font_data &fd = active_fonts[id];
    basic_rect<float> r;

    unsigned int charIndex = fd.char_index[c];

    if (charIndex == 0xFF)
    {
        r = basic_rect<float>::m_Null;
    }
    else
    {
        r.x = fd.bounds[charIndex].x;
        r.y = fd.bounds[charIndex].y;
        r.w = fd.bounds[charIndex].w;
        r.h = fd.bounds[charIndex].h;

        r.scale(width, height);
    }

    return r;
}

void xfont::start_render() const
{
    start_tex_render(id);
}

void xfont::stop_render() const
{
    stop_tex_render();
}

void xfont::irender(const char *text, float x, float y) const
{
    irender(text, 0x4000, x, y);
}

void xfont::irender(const char *text, unsigned int text_size, float x, float y) const
{
    if (text)
    {
        font_data &fd = active_fonts[id];

        set_tex_raster(fd.raster);

        basic_rect<float> bounds;
        bounds.x = x;
        bounds.y = y;
        bounds.w = width;
        bounds.h = height;

        for (unsigned int i = 0; i < text_size; i++)
        {
            if (text[i] == '\0')
            {
                break;
            }

            char_render(text[i], id, bounds, clip, color);

            unsigned int charIndex = fd.char_index[text[i]];

            if (charIndex != 0xFF)
            {
                bounds.x += fd.bounds[charIndex].w * width + space;
            }
        }
    }
}

xfont xfont::create(unsigned int id, float width, float height, float space,
                    iColor_tag color, const basic_rect<float> &clip)
{
    xfont r;
    r.id = id;
    r.width = width;
    r.height = height;
    r.space = space;
    r.color = color;
    r.clip = clip;

    return r;
}

void xtextbox::jot::intersect_flags(const jot &a)
{
    flag.dummy &= a.flag.dummy;
}

void xtextbox::jot::reset_flags()
{
    flag.dummy = 0;
}

xtextbox xtextbox::create(const xfont &font, const basic_rect<float> &bounds,
                          unsigned int flags, float line_space, float tab_stop,
                          float left_indent, float right_indent)
{
    xtextbox r;
    r.font = font;
    r.bounds = bounds;
    r.flags = flags;
    r.line_space = line_space;
    r.tab_stop = tab_stop;
    r.left_indent = left_indent;
    r.right_indent = right_indent;
    r.texts_size = 0;
    r.text_hash = 0;
    r.cb = &text_cb;

    return r;
}

void xtextbox::text_render(const jot &j, const xtextbox &tb, float x, float y)
{
    tb.font.irender(j.s.text, j.s.size, x, y);
}

void xtextbox::set_text(const char *text)
{
    set_text(text, 0x4000);
}

void xtextbox::set_text(const char *text, unsigned int size)
{
    if (text && size)
    {
        this->text.text = text;
        this->text.size = size;

        set_text(&this->text.text, &this->text.size, 1);
    }
    else
    {
        texts_size = 0;
        text_hash = 0;
    }
}

void xtextbox::set_text(const char **texts, unsigned int size)
{
    set_text(texts, NULL, size);
}

void xtextbox::set_text(const char **texts, const unsigned int *text_sizes,
                        unsigned int size)
{
    texts_size = size;

    if (size)
    {
        this->texts = texts;
        this->text_sizes = text_sizes;

        if (!text_sizes)
        {
            for (unsigned int i = 0; i < size; i++)
            {
                text_hash = text_hash * 131 + xStrHash(texts[i]);
            }
        }
        else
        {
            for (unsigned int i = 0; i < size; i++)
            {
                text_hash = text_hash * 131 + xStrHash(texts[i], text_sizes[i]);
            }
        }
    }
}

namespace tweaker
{
namespace
{
void log_cache(bool r3)
{
    return;
}
}
}

xtextbox::layout &xtextbox::temp_layout(bool cache) const
{
    BFBBSTUB("xtextbox::temp_layout");

    //int min_used;
    //unsigned int i;
    //int used;

    iTime cur_time = iTimeGet();
    bool refresh = false;

    unsigned int index = 0;

    if (cache)
    {
        if (tl_cache[0].tl.changed(*this))
        {
            index = 1;
        }
    }
    else
    {
        index = 1;
    }

    //tweaker::log_cache(???);

    if (index >= 1)
    {
        refresh = true;
        index = 0;

        // ...
    }

    tl_cache_entry &e = tl_cache[index];

    if (refresh)
    {
        e.used = 0;
        e.tl.refresh(*this, true);
    }
    else
    {
        e.tl.tb = *this;
    }

    if (cache)
    {
        e.used++;
        e.last_used = cur_time;
    }

    return e.tl;
}

void xtextbox::render(bool cache) const
{
    render(temp_layout(cache), 0, -1);
}

void xtextbox::render(layout &l, int begin_jot, int end_jot) const
{
    l.render(*this, begin_jot, end_jot);
}

float xtextbox::yextent(bool cache) const
{
    return yextent(temp_layout(cache), 0, -1);
}

float xtextbox::yextent(const layout &l, int begin_jot, int end_jot) const
{
    int size;
    return yextent(1e+38f, size, l, begin_jot, end_jot);
}

float xtextbox::yextent(float max, int &size, const layout &l, int begin_jot, int end_jot) const
{
    return l.yextent(max, size, begin_jot, end_jot);
}

void xtextbox::layout::refresh(const xtextbox &tb, bool force)
{
    if (force || changed(tb))
    {
        this->tb = tb;
        calc(tb, 0);
    }
}

void xtextbox::layout::trim_line(jot_line &line)
{
    for (int i = line.last - 1; i >= line.first; i--)
    {
        jot &a = _jots[i];

        if (!a.flag.ethereal)
        {
            if (a.flag.invisible)
            {
                erase_jots(i, i + 1);
                line.last--;
            }

            break;
        }
    }

    for (unsigned int i = line.first; i < line.last; i++)
    {
        jot &a = _jots[i];

        if (!a.flag.ethereal)
        {
            if (a.flag.invisible)
            {
                erase_jots(i, i + 1);
            }

            break;
        }
    }
}

void xtextbox::layout::erase_jots(unsigned int begin_jot, unsigned int end_jot)
{
    if (end_jot >= _jots_size)
    {
        _jots_size = begin_jot;
    }
    else
    {
        unsigned int size = end_jot - begin_jot;
        _jots_size -= size;

        for (int i = begin_jot; i < _jots_size; i++)
        {
            _jots[i] = _jots[i + size];
        }
    }
}

void xtextbox::layout::merge_line(jot_line &line)
{
    unsigned int d = line.first;

    for (unsigned int i = line.first + 1; i != line.last; i++)
    {
        jot &a1 = _jots[d];
        jot &a2 = _jots[i];

        if (!a1.flag.ethereal && !a2.flag.ethereal &&
            a1.flag.merge && a2.flag.merge &&
            a1.cb == a2.cb)
        {
            a1.s.size = a2.s.size + (a2.s.text - a1.s.text);
            a1.bounds |= a2.bounds;
            a1.intersect_flags(a2);
        }
        else
        {
            d++;

            if (d != i)
            {
                _jots[d] = _jots[i];
            }
        }
    }

    erase_jots(d + 1, line.last);
    line.last = d + 1;
}

void xtextbox::layout::bound_line(jot_line &line)
{
    line.baseline = 0.0f;
    line.bounds.w = line.bounds.h = 0.0f;

    for (unsigned int i = line.first; i != line.last; i++)
    {
        jot &a = _jots[i];

        if (!a.flag.ethereal)
        {
            if (-a.bounds.y > line.baseline)
            {
                line.baseline = -a.bounds.y;
            }
        }
    }

    for (unsigned int i = line.first; i != line.last; i++)
    {
        jot &a = _jots[i];

        if (!a.flag.ethereal)
        {
            a.bounds.x = line.bounds.w;
            line.bounds.w += a.bounds.w;

            float total_height = a.bounds.h + line.baseline + a.bounds.y;

            if (total_height > line.bounds.h)
            {
                line.bounds.h = total_height;
            }
        }
    }

    line.page_break = (line.last > line.first) && (_jots[line.last - 1].flag.page_break);
}

bool xtextbox::layout::fit_line()
{
    jot_line &line = _lines[_lines_size];

    if (line.bounds.w > tb.bounds.w)
    {
        switch (tb.flags & 0x30)
        {
        case 0x10:
        {
            if (line.last > (line.first + 1))
            {
                line.last--;
            }

            break;
        }

        case 0x20:
        {
            return false;
        }

        default:
        {
            for (int i = line.last - 1; i > line.first; i--)
            {
                if (_jots[i].flag.word_break)
                {
                    line.last = i + 1;
                    break;
                }

                if (_jots[i - 1].flag.word_end)
                {
                    line.last = i;
                    break;
                }
            }

            if (line.last <= line.first)
            {
                line.last = line.first + 1;
            }

            trim_line(line);
        }
        }
    }

    merge_line(line);
    bound_line(line);

    return true;
}

void xtextbox::layout::next_line()
{
    jot_line &prev_line = _lines[_lines_size++];
    jot_line &line = _lines[_lines_size];

    line.first = prev_line.last;
    line.last = _jots_size;
    line.bounds.x = 0.0f;
    line.bounds.y = prev_line.bounds.y + prev_line.bounds.h;

    bound_line(line);
}

void xtextbox::layout::calc(const xtextbox &ctb, unsigned int start_text)
{
    if (start_text == 0)
    {
        dynamics_size = 0;
        context_buffer_size = 0;
        _lines_size = 0;
        _jots_size = 0;
    }

    if (tb.texts_size)
    {
        start_layout(ctb);

        jot_line &first_line = _lines[_lines_size];

        struct
        {
            const char *s;
            const char *end;
        } text_stack[16];

        unsigned int text_stack_size = 0;

        first_line.first = 0;
        first_line.bounds.w = 0.0f;
        first_line.bounds.x = 0.0f;
        first_line.bounds.y = 0.0f;
        first_line.baseline = 0.0f;

        unsigned int text_index = start_text + 1;
        const char *s = tb.texts[start_text];
        const char *end = s + ((!tb.text_sizes) ? 0x4000 : tb.text_sizes[start_text]);

        while (true)
        {
            jot &a = _jots[_jots_size];
            jot_line &line = _lines[_lines_size];

            a.context = context_buffer;
            a.context_size = 0;
            a.reset_flags();
            a.cb = NULL;
            a.tag = NULL;

            if (s == end || *s == '\0')
            {
                if (text_stack_size)
                {
                    text_stack_size--;
                    s = text_stack[text_stack_size].s;
                    end = text_stack[text_stack_size].end;
                }
                else if (text_index < tb.texts_size)
                {
                    s = tb.texts[text_index];
                    end = s + ((!tb.text_sizes) ? 0x4000 : tb.text_sizes[text_index]);
                    text_index++;
                }
                else
                {
                    break;
                }

                a.flag.invisible = a.flag.ethereal = true;
                a.s = substr::create(NULL, 0);

                _jots_size++;
            }
            else
            {
                const char *r25 = parse_next_jot(a, tb, ctb, s, end - s);

                if (a.context == &context_buffer[context_buffer_size])
                {
                    context_buffer_size += (a.context_size + 3) & ~3;
                }

                _jots_size++;

                if (a.cb && a.cb->layout_update)
                {
                    a.cb->layout_update(a, tb, ctb);
                }

                if (!a.flag.stop)
                {
                    if (!a.flag.ethereal)
                    {
                        a.bounds.x += line.bounds.w;
                        line.bounds.w += a.bounds.w;

                        if (line.bounds.w >= tb.bounds.w)
                        {
                            line.last = _jots_size;

                            if (fit_line())
                            {
                                next_line();
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (a.flag.line_break || a.flag.page_break)
                    {
                        line.last = _jots_size;

                        if (fit_line())
                        {
                            next_line();
                        }
                        else
                        {
                            break;
                        }
                    }

                    s = r25;

                    if (a.flag.insert)
                    {
                        text_stack[text_stack_size].s = r25;
                        text_stack[text_stack_size].end = end;

                        text_stack_size++;
                        s = (const char *)a.context;
                        end = s + a.context_size;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        jot_line &last_line = _lines[_lines_size];

        if (last_line.first < _jots_size)
        {
            last_line.last = _jots_size;

            if (fit_line())
            {
                _lines_size++;
            }
        }

        for (unsigned int i = 0; i < _jots_size; i++)
        {
            if (_jots[i].flag.dynamic)
            {
                dynamics[dynamics_size++] = i;
            }
        }

        stop_layout(ctb);
    }
}

void xtextbox::layout::render(const xtextbox &ctb, int begin_jot, int end_jot)
{
    if (begin_jot < 0)
    {
        begin_jot = 0;
    }

    if (end_jot < begin_jot)
    {
        end_jot = _jots_size;
    }

    if (begin_jot < end_jot)
    {
        tb = ctb;

        start_render(ctb);

        for (int begin_line = 0; begin_line < _lines_size; begin_line++)
        {
            if (_lines[begin_line].last > begin_jot)
            {
                for (int i = 0; i < begin_jot; i++)
                {
                    jot &j = _jots[i];

                    if (j.cb && j.cb->render_update)
                    {
                        j.cb->render_update(j, tb, ctb);
                    }
                }

                float top = _lines[begin_line].bounds.y;
                unsigned int li = begin_line - 1;
                int line_last = -1;
                float x, y;

                for (int i = begin_jot; i < end_jot; i++)
                {
                    if (i >= line_last)
                    {
                        jot_line &line = _lines[++li];

                        line_last = line.last;
                        x = tb.bounds.x + line.bounds.x;
                        y = line.baseline + tb.bounds.y + line.bounds.y - top;

                        unsigned int xj = tb.flags & 0x3;

                        if (xj == 2)
                        {
                            x += 0.5f * (tb.bounds.w - line.bounds.w);
                        }
                        else if (xj == 1)
                        {
                            x += tb.bounds.w - line.bounds.w;
                        }

                        if (line.page_break && (end_jot > line_last))
                        {
                            end_jot = line_last;
                        }
                    }

                    jot &j = _jots[i];
                    
                    if (j.cb)
                    {
                        if (j.cb->render_update)
                        {
                            j.cb->render_update(j, tb, ctb);
                        }

                        if (!j.flag.ethereal && !j.flag.invisible && j.cb->render)
                        {
                            j.cb->render(j, tb, x + j.bounds.x, y);
                        }
                    }
                }

                stop_render(ctb);
                return;
            }
        }

        stop_render(ctb);
        return;
    }
}

float xtextbox::layout::yextent(float max, int &size, int begin_jot, int end_jot) const
{
    size = 0;

    if (begin_jot < 0)
    {
        begin_jot = 0;
    }

    if (end_jot < begin_jot)
    {
        end_jot = _jots_size;
    }

    if (begin_jot >= end_jot)
    {
        return 0.0f;
    }

    int begin_line = 0;

    while (true)
    {
        if (begin_line >= _lines_size)
        {
            return 0.0f;
        }

        if (_lines[begin_line].last > begin_jot)
        {
            break;
        }

        begin_line++;
    }

    float top = max + _lines[begin_line].bounds.y;
    int i = begin_line;

    while (i != _lines_size)
    {
        const jot_line &line = _lines[i];

        if ((line.bounds.y + line.bounds.h) > top)
        {
            i--;
            break;
        }

        if (line.last >= end_jot)
        {
            break;
        }

        if (line.page_break)
        {
            break;
        }

        i++;
    }

    if (i < begin_line)
    {
        return 0.0f;
    }

    const jot_line &line = _lines[i];

    size = ((line.last >= end_jot) ? end_jot : line.last) - begin_jot;

    return line.bounds.y + line.bounds.h - _lines[begin_line].bounds.y;
}

bool xtextbox::layout::changed(const xtextbox &ctb)
{
    unsigned int flags1 = tb.flags & 0x70;
    unsigned int flags2 = ctb.flags & 0x70;

    if (tb.text_hash == ctb.text_hash &&
        tb.font.id == ctb.font.id &&
        tb.font.width == ctb.font.width &&
        tb.font.height == ctb.font.height &&
        tb.font.space == ctb.font.space &&
        tb.bounds.w == ctb.bounds.w &&
        flags1 == flags2 &&
        tb.line_space == ctb.line_space)
    {
        for (int i = dynamics_size; i > 0; i--)
        {
            jot &j = _jots[dynamics[i - 1]];

            unsigned int oldval = xStrHash((const char *)j.context, j.context_size);

            parse_next_jot(j, tb, ctb, j.s.text, j.s.size);

            unsigned int val = xStrHash((const char *)j.context, j.context_size);

            if (val != oldval)
            {
                return true;
            }
        }

        return false;
    }

    return true;
}

namespace
{
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

void update_tag_width(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.width = (float &)a.context;
}

void update_tag_reset_width(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.width = ctb.font.width;
}

void parse_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    if (ti.value.size)
    {
        if (!ti.action.size)
        {
            return;
        }

        float &v = (float &)a.context;
        v = xatof(ti.value.text);

        switch (ti.action.text[0])
        {
        case '+':
        {
            v += tb.font.width;
            break;
        }
        case '*':
        {
            v *= tb.font.width;
            break;
        }
        case '=':
        {
            break;
        }
        default:
        {
            return;
        }
        }

        if (v < 0.0f)
        {
            v = 0.0f;
        }

        if (v > 1.0f)
        {
            v = 1.0f;
        }

        static xtextbox::callback cb =
        {
            NULL, update_tag_width, update_tag_width
        };

        a.cb = &cb;
    }
}

void reset_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    static xtextbox::callback cb =
    {
        NULL, update_tag_reset_width, update_tag_reset_width
    };

    a.cb = &cb;
}

void update_tag_height(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.height = (float &)a.context;
}

void update_tag_reset_height(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.height = ctb.font.height;
}

void parse_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    if (ti.value.size)
    {
        if (!ti.action.size)
        {
            return;
        }

        float &v = (float &)a.context;
        v = xatof(ti.value.text);

        switch (ti.action.text[0])
        {
        case '+':
        {
            v += tb.font.height;
            break;
        }
        case '*':
        {
            v *= tb.font.height;
            break;
        }
        case '=':
        {
            break;
        }
        default:
        {
            return;
        }
        }

        if (v < 0.0f)
        {
            v = 0.0f;
        }
        else if (v > 1.0f)
        {
            v = 1.0f;
        }

        static xtextbox::callback cb =
        {
            NULL, update_tag_height, update_tag_height
        };

        a.cb = &cb;
    }
}

void reset_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                      const xtextbox::split_tag &ti)
{
    static xtextbox::callback cb =
    {
        NULL, update_tag_reset_height, update_tag_reset_height
    };

    a.cb = &cb;
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

void update_tag_color(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.color = (iColor_tag &)a.context;
}

void update_tag_reset_color(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.color = ctb.font.color;
}

void parse_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    if (ti.value.size >= 6)
    {
        if (!ti.action.size)
        {
            return;
        }

        iColor_tag &color = (iColor_tag &)a.context;

        unsigned int v = atox(ti.value);

        if (ti.value.size < 8)
        {
            v = (v & 0xFF000000) | (tb.font.color.a << 24);
        }

        unsigned int temp;

        switch (ti.action.text[0])
        {
        case '=':
        {
            color.a = (v & 0xFF000000) >> 24;
            color.r = (v & 0xFF0000) >> 16;
            color.g = (v & 0xFF00) >> 8;
            color.b = (v & 0xFF);
            break;
        }
        case '+':
        {
            temp = ((v & 0xFF000000) >> 24) + tb.font.color.a;
            color.a = (temp <= 0xFF) ? temp : 0xFF;
            temp = ((v & 0xFF0000) >> 16) + tb.font.color.r;
            color.r = (temp <= 0xFF) ? temp : 0xFF;
            temp = ((v & 0xFF00) >> 8) + tb.font.color.g;
            color.g = (temp <= 0xFF) ? temp : 0xFF;
            temp = (v & 0xFF) + tb.font.color.b;
            color.b = (temp <= 0xFF) ? temp : 0xFF;
            break;
        }
        case '*':
        {
            color.a = ((v & 0xFF000000) >> 24) * tb.font.color.a / 0xFF;
            color.r = ((v & 0xFF0000) >> 16) * tb.font.color.r / 0xFF;
            color.g = ((v & 0xFF00) >> 8) * tb.font.color.g / 0xFF;
            color.b = (v & 0xFF) * tb.font.color.b / 0xFF;
            break;
        }
        default:
        {
            return;
        }
        }

        static xtextbox::callback cb =
        {
            NULL, update_tag_color, update_tag_color
        };

        a.cb = &cb;
    }
}

void reset_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                     const xtextbox::split_tag &ti)
{
    static xtextbox::callback cb =
    {
        NULL, update_tag_reset_color, update_tag_reset_color
    };

    a.cb = &cb;
}

void update_tag_font(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.id = (unsigned int)a.context;
}

void update_tag_reset_font(const xtextbox::jot &a, xtextbox &tb, const xtextbox &ctb)
{
    tb.font.id = ctb.font.id;
}

void parse_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    if ((ti.action.size >= 1) && (ti.action.text[0] == '='))
    {
        if (ti.value.size == 0)
        {
            return;
        }

        unsigned int &id = (unsigned int &)a.context;
        id = atoi(ti.value.text);

        if (id < active_fonts_size)
        {
            static xtextbox::callback cb =
            {
                NULL, update_tag_font, update_tag_font
            };

            a.cb = &cb;
        }
    }
}

void reset_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
                    const xtextbox::split_tag &ti)
{
    static xtextbox::callback cb =
    {
        NULL, update_tag_reset_font, update_tag_reset_font
    };

    a.cb = &cb;
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

xtextbox::tag_type *xtextbox::find_format_tag(const substr &s)
{
    int index;
    return find_format_tag(s, index);
}

xtextbox::tag_type *xtextbox::find_format_tag(const substr &s, int &index)
{
    int start = 0;
    int end = format_tags_size;

    while (start != end)
    {
        index = (start + end) / 2;

        tag_type &t = format_tags[index];
        int c = icompare(s, t.name);

        if (c < 0)
        {
            end = index;
        }
        else if (c > 0)
        {
            start = index + 1;
        }
        else
        {
            return &t;
        }
    }

    index = -1;
    return NULL;
}

namespace
{
void set_rect_vert(RwIm2DVertex &vert, float x, float y, float z, iColor_tag c,
                   float rcz)
{
    RwIm2DVertexSetScreenX(&vert, x);
    RwIm2DVertexSetScreenY(&vert, y);
    RwIm2DVertexSetScreenZ(&vert, z);
    RwIm2DVertexSetRecipCameraZ(&vert, rcz);
    RwIm2DVertexSetIntRGBA(&vert, c.r, c.g, c.b, c.a);
}

void set_rect_verts(RwIm2DVertex *verts, float x, float y, float w, float h,
                    iColor_tag c, float rcz, float nsz)
{
    set_rect_vert(verts[0], x,     y,     nsz, c, rcz);
    set_rect_vert(verts[1], x,     y + h, nsz, c, rcz);
    set_rect_vert(verts[2], x + w, y,     nsz, c, rcz);
    set_rect_vert(verts[3], x + w, y + h, nsz, c, rcz);
}
}

void render_fill_rect(const basic_rect<float> &bounds, iColor_tag color)
{
    if (!bounds.empty())
    {
        float rcz = RwCameraGetNearClipPlane(RwCameraGetCurrentCamera());
        float nsz = RwIm2DGetNearScreenZ();        

        xfont::set_render_state(NULL);

        RwIm2DVertex vert[4];

        basic_rect<float> r = bounds;
        r.scale(SCREEN_WIDTH, SCREEN_HEIGHT);

        set_rect_verts(vert, r.x, r.y, r.w, r.h, color, rcz, nsz);

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);

        xfont::restore_render_state();
    }
}