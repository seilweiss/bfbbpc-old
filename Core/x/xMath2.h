#ifndef XMATH2_H
#define XMATH2_H

struct xVec2
{
	float x;
	float y;
};

template <class T>
struct basic_rect
{
	T x;
	T y;
	T w;
	T h;
};

#endif