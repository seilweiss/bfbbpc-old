#include "iModel.h"

#include <rwcore.h>
#include <rpworld.h>

static RpLight *sEmptyDirectionalLight[4];
static RpLight *sEmptyAmbientLight;

void iModelInit()
{
	RwFrame *frame;
	RwRGBAReal black = { 0, 0, 0, 0 };
	int i;

	if (!sEmptyDirectionalLight[0])
	{
		for (i = 0; i < 4; i++)
		{
			sEmptyDirectionalLight[i] = RpLightCreate(rpLIGHTDIRECTIONAL);

			RpLightSetColor(sEmptyDirectionalLight[i], &black);

			frame = RwFrameCreate();
			RpLightSetFrame(sEmptyDirectionalLight[i], frame);
		}

		sEmptyAmbientLight = RpLightCreate(rpLIGHTAMBIENT);

		RpLightSetColor(sEmptyAmbientLight, &black);
	}
}