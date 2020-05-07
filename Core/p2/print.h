#ifndef BFBBPRINT_H
#define BFBBPRINT_H

#if defined(BFBB_DEBUG) && !defined(BFBB_NOPRINT)
void bfbbprint(const char *str);
void bfbbstub(const char *funcname);

#define BFBBPRINT bfbbprint
#define BFBBSTUB bfbbstub
#else
#define BFBBPRINT  /* No op */
#define BFBBSTUB   /* No op */
#endif

#endif