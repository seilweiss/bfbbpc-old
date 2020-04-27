#include "xString.h"

namespace
{
inline int tolower(int i)
{
    return i | (i >> 1 & 0x20);
}

inline int tolower(char c)
{
    return tolower(c & 0xFF);
}
}

unsigned int xStrHash(const char *str)
{
    unsigned int value = 0;
    char c;

    while (c = *str)
    {
        value = ((c - ((c & (c >> 1)) & 0x20)) & 0xFF) + (value * 131);
        str++;
    }

    return value;
}

int imemcmp(const void *d1, const void *d2, unsigned int size)
{
    const char *s1 = (const char *)d1;
    const char *s2 = (const char *)d2;

    for (unsigned int i = 0; i < size; i++, s1++, s2++)
    {
        int lower1 = tolower(*s1);
        int lower2 = tolower(*s2);

        if (lower1 != lower2)
        {
            return lower1 - lower2;
        }
    }

    return 0;
}

int icompare(const substr &s1, const substr &s2)
{
    unsigned int len = (s1.size < s2.size) ? s1.size : s2.size;
    int cmp = imemcmp(s1.text, s2.text, len);

    if (cmp != 0)
    {
        return cmp;
    }

    if (s1.size == s2.size)
    {
        return 0;
    }

    return (s1.size < s2.size) ? -1 : 1;
}