#include "xGrid.h"

#include <stdlib.h>

void xGridBoundInit(xGridBound *gridb, void *data)
{
    gridb->data = data;
    gridb->gx = 0xFFFF;
    gridb->gz = 0xFFFF;
    gridb->ingrid = 0;
    gridb->oversize = 0;
    gridb->head = NULL;
    gridb->next = NULL;
    gridb->gpad = 0xEA;
}