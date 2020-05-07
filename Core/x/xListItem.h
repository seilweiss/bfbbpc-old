#ifndef XLISTITEM_H
#define XLISTITEM_H

template<class T>
struct xListItem
{
	int flg_travFilter;
	T *next;
	T *prev;
};

#endif