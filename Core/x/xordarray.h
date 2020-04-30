#ifndef XORDARRAY_H
#define XORDARRAY_H

struct st_XORDEREDARRAY
{
	void **list;
	int cnt;
	int max;
	int warnlvl;
};

void XOrdDone(st_XORDEREDARRAY *array, int wasTempAlloc);

#endif