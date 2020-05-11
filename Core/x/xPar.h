#ifndef XPAR_H
#define XPAR_H

#include "xMath3.h"

struct xParEmitterAsset;

struct xPar
{
    xPar *m_next;
    xPar *m_prev;
    float m_lifetime;
    unsigned char m_c[4];
    xVec3 m_pos;
    float m_size;
    xVec3 m_vel;
    float m_sizeVel;
    unsigned char m_flag;
    unsigned char m_mode;
    unsigned char m_texIdx[2];
    unsigned char m_rotdeg[3];
    unsigned char pad8;
    float totalLifespan;
    xParEmitterAsset *m_asset;
    float m_cvel[4];
    float m_cfl[4];
};

#endif