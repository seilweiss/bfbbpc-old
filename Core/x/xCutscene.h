#ifndef XCUTSCENE_H
#define XCUTSCENE_H

#include "xFile.h"

struct xCutsceneInfo
{
    unsigned int Magic;
    unsigned int AssetID;
    unsigned int NumData;
    unsigned int NumTime;
    unsigned int MaxModel;
    unsigned int MaxBufEven;
    unsigned int MaxBufOdd;
    unsigned int HeaderSize;
    unsigned int VisCount;
    unsigned int VisSize;
    unsigned int BreakCount;
    unsigned int pad;
    char SoundLeft[16];
    char SoundRight[16];
};

struct xCutsceneData
{
    unsigned int DataType;
    unsigned int AssetID;
    unsigned int ChunkSize;
    union
    {
        unsigned int FileOffset;
        void *DataPtr;
    };
};

struct xCutsceneBreak
{
    float Time;
    int Index;
};

struct xCutsceneTime
{
    float StartTime;
    float EndTime;
    unsigned int NumData;
    unsigned int ChunkIndex;
};

struct XCSNNosey
{
    void *userdata;
    int flg_nosey;
};

struct xCutscene
{
    xCutsceneInfo *Info;
    xCutsceneData *Data;
    unsigned int *TimeChunkOffs;
    unsigned int *Visibility;
    xCutsceneBreak *BreakList;
    xCutsceneTime *Play;
    xCutsceneTime *Stream;
    unsigned int Waiting;
    unsigned int BadReadPause;
    float BadReadSpeed;
    void *RawBuf;
    void *AlignBuf;
    float Time;
    float CamTime;
    unsigned int PlayIndex;
    unsigned int Ready;
    int DataLoading;
    unsigned int GotData;
    unsigned int ShutDownWait;
    float PlaybackSpeed;
    unsigned int Opened;
    tag_xFile File;
    int AsyncID;
    void *MemBuf;
    void *MemCurr;
    unsigned int SndStarted;
    unsigned int SndNumChannel;
    unsigned int SndChannelReq[2];
    unsigned int SndAssetID[2];
    unsigned int SndHandle[2];
    XCSNNosey *cb_nosey;
};

struct xCamera;

void xCutscene_Init(void *toc);
void xCutscene_SetCamera(xCutscene *csn, xCamera *cam);

#endif