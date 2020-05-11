#include "xJSP.h"

#include "zMain.h"

#include <string.h>

struct __rwMark
{
	RwUInt32 type;
	RwUInt32 length;
	RwUInt32 libraryID;
};

static RpAtomic *ListAtomicCB(RpAtomic *atomic, void *data)
{
	RpAtomic ***aList = (RpAtomic ***)data;

	**aList = atomic;
	*aList = *aList + 1;

	return atomic;
}

void xJSP_MultiStreamRead(void *data, unsigned int size, xJSPHeader **jsp)
{
	unsigned int i;
	RpClump *clump;
	xClumpCollBSPTree *colltree;
	xJSPHeader *hdr;
	__rwMark mark;
	__rwMark *mp;
	xJSPHeader *tmphdr;

	if (!*jsp)
	{
		*jsp = (xJSPHeader *)RwMalloc(sizeof(xJSPHeader), rwMEMHINTDUR_GLOBAL);
		memset(*jsp, 0, sizeof(xJSPHeader));
	}

	hdr = *jsp;
	mp = (__rwMark *)data;

	memmove(&mark, mp, sizeof(__rwMark));
	RwMemNative32(&mark, sizeof(__rwMark));

	if (mark.type == 0xBEEF01)
	{
		data = mp + 1;

		colltree = xClumpColl_StaticBufferInit(data);

		data = (RwUInt8 *)data + mark.length;
		size -= mark.length + sizeof(__rwMark);

		mp = (__rwMark *)data;

		memmove(&mark, mp, sizeof(__rwMark));
		RwMemNative32(&mark, sizeof(__rwMark));

		data = mp + 1;
		tmphdr = (xJSPHeader *)data;

		strncpy(hdr->idtag, tmphdr->idtag, 4);

		hdr->version = tmphdr->version;
		hdr->jspNodeCount = tmphdr->jspNodeCount;
		hdr->colltree = colltree;
		hdr->jspNodeList = (xJSPNodeInfo *)(tmphdr + 1);

		size -= mark.length + sizeof(__rwMark);

		for (i = 0; i < colltree->numTriangles; i++)
		{
			colltree->triangles[i].matIndex = hdr->jspNodeList[
				hdr->jspNodeCount - 1 - colltree->triangles[i].v.i.atomIndex].originalMatIndex;
		}

		/* gamecube precalc vert loading goes here (0xBEEF03)... */
	}
	else
	{
		RwMemory rwmem;
		RwStream *stream;

		rwmem.start = (RwUInt8 *)data;
		rwmem.length = size;
		
		stream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &rwmem);

		RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL);

		clump = RpClumpStreamRead(stream);

		RwStreamClose(stream, NULL);

		if (!hdr->clump)
		{
			hdr->clump = clump;
		}
		else
		{
			int i;
			int atomCount;
			RpAtomic **atomList;
			RpAtomic **atomCurr;

			atomCount = RpClumpGetNumAtomics(clump);

			if (atomCount)
			{
				atomList = (RpAtomic **)RwMalloc(
					atomCount * sizeof(RpAtomic *), rwMEMHINTDUR_FUNCTION);

				RpClumpForAllAtomics(clump, ListAtomicCB, &atomList);

				for (i = atomCount - 1; i >= 0; i--)
				{
					atomCurr = &atomList[i];

					RpClumpRemoveAtomic(clump, *atomCurr);
					RpClumpAddAtomic(hdr->clump, *atomCurr);

					RpAtomicSetFrame(*atomCurr, RpClumpGetFrame(hdr->clump));
				}

				RwFree(atomList);
			}

			RpClumpDestroy(clump);
		}
	}
}

void xJSP_Destroy(xJSPHeader *jsp)
{
	if (globals.sceneCur->env->geom->jsp == jsp &&
		globals.sceneCur->env->geom->world)
	{
		RpWorldDestroy(globals.sceneCur->env->geom->world);

		globals.sceneCur->env->geom->world = NULL;
	}

	RpClumpDestroy(jsp->clump);
	RwFree(jsp->colltree);

	/* free gamecube precalc vert list here */
}