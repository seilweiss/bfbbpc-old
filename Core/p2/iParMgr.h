#ifndef IPARMGR_H
#define IPARMGR_H

#include <rwcore.h>

struct tagiRenderArrays
{
	unsigned short m_index[960];
	RwIm3DVertex m_vertex[480];
	float m_vertexTZ[480];
};

extern tagiRenderArrays gRenderArr;

#endif