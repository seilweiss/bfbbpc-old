#include "xEnt.h"

struct anim_coll_data
{
	unsigned int flags;
	unsigned int bones;
	xMat4x3 old_mat;
	xMat4x3 new_mat;
	unsigned int verts_size;
	xVec3 *verts;
	xVec3 *normals;
};