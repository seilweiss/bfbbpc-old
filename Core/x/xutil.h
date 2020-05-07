#ifndef XUTIL_H
#define XUTIL_H

unsigned int xUtil_crc_init();
int xUtilStartup();
int xUtilShutdown();
char *xUtil_idtag2string(unsigned int srctag, int bufidx);

#endif