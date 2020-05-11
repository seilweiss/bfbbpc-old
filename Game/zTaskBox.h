#ifndef ZTASKBOX_H
#define ZTASKBOX_H

#include "xBase.h"
#include "xDynAsset.h"

struct ztaskbox : xBase
{
	struct asset_type : xDynAsset
	{
		bool persistent;
		bool loop;
		bool enable;
		bool retry;
		unsigned int talk_box;
		unsigned int next_task;
		unsigned int stages[6];
	};

	enum state_enum
	{
		STATE_INVALID = -1,
		STATE_BEGIN = 0,
		STATE_DESCRIPTION,
		STATE_REMINDER,
		STATE_SUCCESS,
		STATE_FAILURE,
		STATE_END,
		MAX_STATE
	};

	struct callback
	{
	};

	struct
	{
		bool enabled       : 8;
		unsigned int dummy : 24;
	} flag;
	asset_type *asset;
	state_enum state;
	callback *cb;
	ztaskbox *current;

	static void init();
};

#endif