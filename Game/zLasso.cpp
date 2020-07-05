#include "zLasso.h"

#include "xAnim.h"
#include "xstransvc.h"
#include "xString.h"
#include "iModel.h"
#include "iParMgr.h"

#include <rwcore.h>

#include "print.h"

struct zLassoGuide
{
	xModelInstance *poly;
	xAnimState *lassoAnim;
	int vertMap[16];
};

static RwRaster *sLassoRaster;
static unsigned int sNumGuideLists;
static zLassoGuide *sCurrentGuide;
static RwIm3DVertex *lnverts;

void zLasso_Init(zLasso *lasso, xModelInstance *model, float x, float y, float z)
{
	if (!sLassoRaster)
	{
		RwTexture *tempTexture = (RwTexture *)xSTFindAsset(xStrHash("rope"), NULL);

		if (tempTexture)
		{
			sLassoRaster = RwTextureGetRaster(tempTexture);
		}
		else
		{
			sLassoRaster = NULL;
		}
	}

	iModelTagSetup(&lasso->tag, model->Data, x, y, z);

	lasso->model = model;

	lnverts = gRenderArr.m_vertex;
}

void zLasso_scenePrepare()
{
	sNumGuideLists = 0;
	sCurrentGuide = NULL;
}