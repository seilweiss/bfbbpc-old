#include "iModel.h"

#include "xMathInlines.h"

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

int iModelCull(RpAtomic *model, RwMatrix *mat)
{
    RwCamera *cam = RwCameraGetCurrentCamera();
    RwSphere sph;

    RwV3dTransformPoints(&sph.center, &model->boundingSphere.center, 1, mat);

    RwReal f1 = RwV3dDotProduct(&mat->right, &mat->right);
    RwReal f3 = RwV3dDotProduct(&mat->up, &mat->up);
    RwReal f4 = RwV3dDotProduct(&mat->at, &mat->at);

    sph.radius = model->boundingSphere.radius * xsqrt(xmax(f1, xmax(f3, f4)));

    model->worldBoundingSphere = sph;

    if (!cam)
    {
        return 1;
    }

    return (RwCameraFrustumTestSphere(cam, &sph) == rwSPHEREOUTSIDE);
}