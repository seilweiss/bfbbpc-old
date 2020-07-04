#ifndef ZANIMLIST_H
#define ZANIMLIST_H

struct xAnimTable;

void zAnimListInit();
void zAnimListExit();
xAnimTable *zAnimListGetTable(unsigned int id);
int zAnimListGetNumUsed(unsigned int id);

#endif