#ifndef XPKRSVC_H
#define XPKRSVC_H

#include "xordarray.h"

struct st_PACKER_ASSETTYPE
{
	unsigned int typetag;
	unsigned int tflags;
	int typalign;
	void *(*readXForm)(void *, unsigned int assetid, void *indata, unsigned int insize, unsigned int *outsize);
	void *(*writeXForm)(void *, unsigned int, void *, void *, unsigned int, unsigned int *);
	int(*assetLoaded)(void *, unsigned int, void *, int);
	void *(*makeData)(void *, unsigned int, void *, int *, int *);
	void(*cleanup)(void *, unsigned int, void *);
	void(*assetUnloaded)(void *userdata, unsigned int);
	void(*writePeek)(void *, unsigned int, void *, char *);
};

struct st_HIPLOADDATA;
struct st_PACKER_READ_DATA;

struct st_PACKER_ATOC_NODE
{
	unsigned int aid;
	unsigned int asstype;
	int d_off;
	int d_size;
	int d_pad;
	unsigned int d_chksum;
	int assalign;
	int infoflag;
	int loadflag;
	char *memloc;
	int x_size;
	int readcnt;
	int readrem;
	st_PACKER_ASSETTYPE *typeref;
	st_HIPLOADDATA *ownpkg;
	st_PACKER_READ_DATA *ownpr;
	char basename[32];

	const char *Name() const;
};

struct st_PACKER_READ_DATA
{
	st_PACKER_ASSETTYPE *types;
	void *userdata;
	unsigned int opts;
	unsigned int pkgver;
	int cltver;
	int subver;
	int compatver;
	st_HIPLOADDATA *pkg;
	unsigned int base_sector;
	int lockid;
	char packfile[128];
	int asscnt;
	int laycnt;
	st_XORDEREDARRAY asstoc;
	st_XORDEREDARRAY laytoc;
	st_PACKER_ATOC_NODE *pool_anode;
	int pool_nextaidx;
	st_XORDEREDARRAY typelist[129];
	long time_made;
	long time_mod;
};

enum en_LAYER_TYPE
{
	PKR_LTYPE_ALL = 0xffffffff,
	PKR_LTYPE_DEFAULT = 0,
	PKR_LTYPE_TEXTURE,
	PKR_LTYPE_BSP,
	PKR_LTYPE_MODEL,
	PKR_LTYPE_ANIMATION,
	PKR_LTYPE_VRAM,
	PKR_LTYPE_SRAM,
	PKR_LTYPE_SNDTOC,
	PKR_LTYPE_CUTSCENE,
	PKR_LTYPE_CUTSCENETOC,
	PKR_LTYPE_JSPINFO,
	PKR_LTYPE_NOMORE
};

struct st_PACKER_LTOC_NODE
{
	en_LAYER_TYPE laytyp;
	st_XORDEREDARRAY assref;
	int flg_ldstat;
	int danglecnt;
	unsigned int chksum;
	int laysize;
	char *laymem;
	char *laytru;
};

struct st_PKR_ASSET_TOCINFO
{
	unsigned int aid;
	st_PACKER_ASSETTYPE *typeref;
	unsigned int sector;
	unsigned int plus_offset;
	unsigned int size;
	void *mempos;
};

struct st_PACKER_READ_FUNCS
{
	unsigned int api_ver;
	st_PACKER_READ_DATA *(*Init)(void *userdata, const char *pkgfile, unsigned int opts,
								 int *cltver, st_PACKER_ASSETTYPE *typelist);
	void(*Done)(st_PACKER_READ_DATA *pr);
	int(*LoadLayer)(st_PACKER_READ_DATA *, en_LAYER_TYPE);
	unsigned int(*GetAssetSize)(st_PACKER_READ_DATA *pr, unsigned int aid);
	void *(*LoadAsset)(st_PACKER_READ_DATA *pr, unsigned int aid, const char *, void *);
	void *(*AssetByType)(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
						 unsigned int *size);
	int(*AssetCount)(st_PACKER_READ_DATA *pr, unsigned int type);
	int(*IsAssetReady)(st_PACKER_READ_DATA *pr, unsigned int aid);
	int(*SetActive)(st_PACKER_READ_DATA *pr, en_LAYER_TYPE layer);
	const char *(*AssetName)(st_PACKER_READ_DATA *pr, unsigned int aid);
	unsigned int(*GetBaseSector)(st_PACKER_READ_DATA *pr);
	int(*GetAssetInfo)(st_PACKER_READ_DATA *pr, unsigned int aid,
					   st_PKR_ASSET_TOCINFO *tocinfo);
	int(*GetAssetInfoByType)(st_PACKER_READ_DATA *pr, unsigned int type, int idx,
							 st_PKR_ASSET_TOCINFO *tocinfo);
	int(*PkgHasAsset)(st_PACKER_READ_DATA *pr, unsigned int aid);
	unsigned int(*PkgTimeStamp)(st_PACKER_READ_DATA *pr);
	void(*PkgDisconnect)(st_PACKER_READ_DATA *pr);
};

st_PACKER_READ_FUNCS *PKRGetReadFuncs(int apiver);
int PKRStartup();
int PKRShutdown();
int PKRLoadStep(int);
unsigned int PKRAssetIDFromInst(void *asset_inst);

#endif