#ifndef XSTRING_H
#define XSTRING_H

struct substr
{
    const char *text;
    unsigned int size;
};

unsigned int xStrHash(const char *str);

int imemcmp(const void *d1, const void *d2, unsigned int size);
int icompare(const substr &s1, const substr &s2);

#endif