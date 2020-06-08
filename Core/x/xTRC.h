#ifndef XTRC_H
#define XTRC_H

#include "iTRC.h"

enum _tagTRCState
{
	TRC_Unknown,
	TRC_PadMissing,
	TRC_PadInserted,
	TRC_PadInvalidNoAnalog,
	TRC_PadInvalidType,
	TRC_DiskNotIdentified,
	TRC_DiskIdentified,
	TRC_DiskTrayOpen,
	TRC_DiskTrayClosed,
	TRC_DiskNoDisk,
	TRC_DiskInvalid,
	TRC_DiskRetry,
	TRC_DiskFatal,
	TRC_Total
};

struct _tagTRCPadInfo : _tagiTRCPadInfo
{
	int id;
	_tagTRCState state;
};

extern _tagTRCPadInfo gTrcPad[4];

void xTRCInit();
void xTRCRender();
void xTRCPad(int pad_id, _tagTRCState state);
void RenderText(const char *text, bool enabled);
void render_mem_card_no_space(int needed, int available, int neededFiles, bool enabled);

#endif