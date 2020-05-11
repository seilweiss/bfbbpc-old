#include "xMath3.h"

#include "iMath3.h"
#include "iMath.h"

xVec3 g_O3 = { 0.0f, 0.0f, 0.0f };

xVec3 g_X3 = { 1.0f, 0.0f, 0.0f };
xVec3 g_Y3 = { 0.0f, 1.0f, 0.0f };
xVec3 g_Z3 = { 0.0f, 0.0f, 1.0f };

xVec3 g_NX3 = { -1.0f, 0.0f, 0.0f };
xVec3 g_NY3 = { 0.0f, -1.0f, 0.0f };
xVec3 g_NZ3 = { 0.0f, 0.0f, -1.0f };

xVec3 g_Onez = { 1.0f, 1.0f, 1.0f };

xMat4x3 g_I3;

xQuat g_IQ = { 0.0f, 0.0f, 0.0f, 1.0f };

void xMath3Init()
{
    iMath3Init();

    g_I3.right.x = g_X3.x;
    g_I3.right.y = g_X3.y;
    g_I3.right.z = g_X3.z;

    g_I3.up.x = g_Y3.x;
    g_I3.up.y = g_Y3.y;
    g_I3.up.z = g_Y3.z;

    g_I3.at.x = g_Z3.x;
    g_I3.at.y = g_Z3.y;
    g_I3.at.z = g_Z3.z;

    g_I3.pos.x = g_O3.x;
    g_I3.pos.y = g_O3.y;
    g_I3.pos.z = g_O3.z;
}

void xMat3x3Euler(xMat3x3 *m, float yaw, float pitch, float roll)
{
    float f27 = isin(yaw);
    float f28 = icos(yaw);
    float f29 = isin(pitch);
    float f30 = icos(pitch);
    float f31 = isin(roll);
    float f1 = icos(roll);
    float f7 = f27 * f29;
    float f0 = f28 * f29;

    m->right.x = f28 * f1 + f31 * f7;
    m->right.y = f30 * f31;
    m->right.z = -f27 * f1 + f31 * f0;

    m->up.x = -f28 * f31 + f1 * f7;
    m->up.y = f30 * f1;
    m->up.z = f27 * f31 + f1 * f0;

    m->at.x = f27 * f30;
    m->at.y = -f29;
    m->at.z = f28 * f30;

    m->flags = 0;
}