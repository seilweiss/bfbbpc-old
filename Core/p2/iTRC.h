#ifndef ITRC_H
#define ITRC_H

#include "iPad.h"

struct _tagiTRCPadInfo
{
	_tagPadInit pad_init;
};

namespace iTRCDisk
{
unsigned int CheckDVDAndResetState();
}

#endif