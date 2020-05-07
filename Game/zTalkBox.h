#ifndef ZTALKBOX_H
#define ZTALKBOX_H

#include "xBase.h"
#include "xDynAsset.h"
#include "zTextBox.h"
#include "zNPCTypeCommon.h"
#include "xIni.h"

struct ztalkbox : xBase
{
	struct asset_type : xDynAsset
	{
		unsigned int dialog_box;
		unsigned int prompt_box;
		unsigned int quit_box;
		unsigned char trap;
		unsigned char pause;
		unsigned char allow_quit;
		unsigned char trigger_pads;
		unsigned char page;
		unsigned char show;
		unsigned char hide;
		unsigned char audio_effect;
		unsigned int teleport;
		struct
		{
			struct
			{
				unsigned char time;
				unsigned char prompt;
				unsigned char sound;
				unsigned char event;
			} type;
			float delay;
			int which_event;
		} auto_wait;
		struct
		{
			unsigned int skip;
			unsigned int noskip;
			unsigned int quit;
			unsigned int noquit;
			unsigned int yesno;
		} prompt;
	};

	struct
	{
		bool visible : 1;
	} flag;
	asset_type *asset;
	ztextbox *dialog_box;
	ztextbox *prompt_box;
	ztextbox *quit_box;
	struct
	{
		char *skip;
		char *noskip;
		char *quit;
		char *noquit;
		char *yesno;
	} prompt;
	zNPCCommon *npc;

	static void load_settings(xIniFile &ini);
};

#endif