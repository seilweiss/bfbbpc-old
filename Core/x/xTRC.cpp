#include "xTRC.h"

#include "xColor.h"
#include "xFont.h"
#include "iSystem.h"

#include "print.h"

_tagTRCPadInfo gTrcPad[4];

void xTRCInit()
{
    BFBBSTUB("xTRCInit");
}

void xTRCRender()
{
    BFBBSTUB("xTRCRender");
}

void xTRCPad(int pad_id, _tagTRCState state)
{
    return;
}

void render_mem_card_no_space(int needed, int available, int neededFiles, bool enabled)
{
    BFBBSTUB("render_mem_card_no_space");
}

void RenderText(const char *text, bool enabled)
{
    static xtextbox tb = xtextbox::create(
            xfont::create(1, NSCREENX(19.0f), NSCREENY(22.0f), 0.0f,
            xColorFromRGBA(255, 230, 0, 255), screen_bounds),
        screen_bounds, 0x2, 0.0f, 0.0f, 0.0f, 0.0f);

    tb.set_text((enabled) ? text : "");

    tb.bounds = screen_bounds;
    tb.bounds.contract(0.1f);
    tb.bounds.h = tb.yextent(true);
    tb.bounds.y = -(0.5f * tb.bounds.h - 0.5f);

    tb.render(true);

    if (!enabled)
    {
        render_fill_rect(tb.font.clip, xColorFromRGBA(0, 0, 0, 150));
    }
}