#include "xMath3.h"

#include "iMath3.h"

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