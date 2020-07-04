#ifndef ZNPCTYPECOMMON_H
#define ZNPCTYPECOMMON_H

#include "xNPCBasic.h"
#include "xDynAsset.h"
#include "xListItem.h"
#include "zNPCSndTable.h"
#include "zMovePoint.h"
#include "xEntDrive.h"
#include "xBehaveMgr.h"
#include "zShrapnel.h"
#include "xAnim.h"

struct xEntNPCAsset
{
	int npcFlags;
	int npcModel;
	int npcProps;
	unsigned int movepoint;
	unsigned int taskWidgetPrime;
	unsigned int taskWidgetSecond;
};

enum en_npcbtyp
{
	NPCP_BASIS_NONE,
	NPCP_BASIS_EVILROBOT,
	NPCP_BASIS_FRIENDLYROBOT,
	NPCP_BASIS_LOVINGCITIZEN,
	NPCP_BASIS_GRUMPYCITIZEN,
	NPCP_BASIS_NOMORE,
	NPCP_BASIS_FORCE = 0x7fffffff
};

enum en_dupowavmod
{
	NPCP_DUPOWAVE_CONTINUOUS,
	NPCP_DUPOWAVE_DISCREET,
	NPCP_DUPOWAVE_NOMORE,
	NPCP_DUPOWAVE_FORCE = 0x7fffffff
};

struct zNPCSettings : xDynAsset
{
	en_npcbtyp basisType;
	char allowDetect;
	char allowPatrol;
	char allowWander;
	char reduceCollide;
	char useNavSplines;
	char pad[3];
	char allowChase;
	char allowAttack;
	char assumeLOS;
	char assumeFOV;
	en_dupowavmod duploWaveMode;
	float duploSpawnDelay;
	int duploSpawnLifeMax;
};

struct NPCConfig : xListItem<NPCConfig>
{
	unsigned int modelID;
	int flg_config;
	float spd_turnMax;
	float spd_moveMax;
	float fac_accelMax;
	float fac_driftMax;
	float fac_gravKnock;
	float fac_elastic;
	int pts_damage;
	int useBoxBound;
	xVec3 off_bound;
	xVec3 dim_bound;
	float npcMass;
	float npcMassInv;
	float rad_detect;
	float hyt_detect;
	float off_detect;
	float rad_attack;
	float fov_attack;
	xVec3 scl_model;
	float tym_attack;
	float tym_fidget;
	float tym_stun;
	float tym_alert;
	float dst_castShadow;
	float rad_shadowCache;
	float rad_shadowRaster;
	float rad_dmgSize;
	int flg_vert;
	xModelTag tag_vert[20];
	xVec3 animFrameRange[9];
	int cnt_esteem[5];
	float rad_sound;
	NPCSndTrax *snd_trax;
	NPCSndTrax *snd_traxShare;
	int test_count;
	unsigned char talk_filter[4];
	unsigned char talk_filter_size;
};

enum en_LASSO_STATUS
{
	LASS_STAT_DONE,
	LASS_STAT_PENDING,
	LASS_STAT_GRABBING,
	LASS_STAT_TOSSING,
	LASS_STAT_NOMORE,
	LASS_STAT_FORCEINT = 0x7fffffff
};

struct zNPCLassoInfo
{
	en_LASSO_STATUS stage;
	xEnt *lassoee;
	xAnimState *holdGuideAnim;
	xModelInstance *holdGuideModel;
	xAnimState *grabGuideAnim;
	xModelInstance *grabGuideModel;
};

struct zNPCCommon : xNPCBasic
{
	xEntAsset *entass;
	xEntNPCAsset *npcass;
	zNPCSettings *npcsetass;
	int flg_vuln;
	int flg_move;
	int flg_misc;
	int flg_able;
	NPCConfig *cfg_npc;
	zNPCSettings npcset;
	zMovePoint *nav_past;
	zMovePoint *nav_curr;
	zMovePoint *nav_dest;
	zMovePoint *nav_lead;
	xSpline3 *spl_mvptspline;
	float len_mvptspline;
	float dst_curspline;
	xEntDrive *drv_data;
	xPsyche *psy_instinct;
	zNPCCommon *npc_duplodude;
	float spd_throttle;
	int flg_xtrarend;
	float tmr_fidget;
	float tmr_invuln;
	zShrapnelAsset *explosion;
	xModelAssetParam *parmdata;
	unsigned int pdatsize;
	zNPCLassoInfo *lassdata;
	NPCSndQueue snd_queue[4];
};

xAnimTable *ZNPC_AnimTable_Common();
xAnimTable *ZNPC_AnimTable_LassoGuide();

void zNPCCommon_EjectPhlemOnPawz();

#endif