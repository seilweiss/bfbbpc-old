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

static xBox all_ents_box;
static int all_ents_box_init;

void xEntSceneInit()
{
    all_ents_box_init = 1;
}

xBox *xEntGetAllEntsBox()
{
    return &all_ents_box;
}