#ifndef XGRID_H
#define XGRID_H

struct xGridBound
{
    void *data;
    unsigned short gx;
    unsigned short gz;
    unsigned char ingrid;
    unsigned char oversize;
    unsigned char deleted;
    unsigned char gpad;
    xGridBound **head;
    xGridBound *next;
};

struct xGrid
{
	unsigned char ingrid_id;
	unsigned char pad[3];
	unsigned short nx;
	unsigned short nz;
	float minx;
	float minz;
	float maxx;
	float maxz;
	float csizex;
	float csizez;
	float inv_csizex;
	float inv_csizez;
	float maxr;
	xGridBound **cells;
	xGridBound *other;
};

void xGridBoundInit(xGridBound *gridb, void *data);

#endif