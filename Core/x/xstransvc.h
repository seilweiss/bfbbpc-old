#ifndef XSTRANSVC_H
#define XSTRANSVC_H

struct st_PACKER_READ_DATA;
struct st_PACKER_ASSETTYPE;

struct st_STRAN_SCENE
{
    unsigned int scnid;
    int lockid;
    st_PACKER_READ_DATA *spkg;
    int isHOP;
    void *userdata;
    char fnam[256];
};

struct st_STRAN_DATA
{
    st_STRAN_SCENE hipscn[16];
    unsigned int loadlock;
};

int xSTStartup(st_PACKER_ASSETTYPE *handlers);
int xSTPreLoadScene(unsigned int sid, void *userdata, int flg_hiphop);
int xSTQueueSceneAssets(unsigned int sid, int flg_hiphop);
float xSTLoadStep(unsigned int sid);
void xSTDisconnect(unsigned int sid, int flg_hiphop);
void *xSTFindAsset(unsigned int aid, unsigned int *size);
void *xSTFindAssetByType(unsigned int type, int idx, unsigned int *size);

#endif