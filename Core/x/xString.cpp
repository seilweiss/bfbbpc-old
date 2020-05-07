#include "xString.h"

#define BFBB_NOPRINT

#include "print.h"

#include <stdlib.h>
#include <string.h>

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

unsigned int xStrHash(const char *s, unsigned int size)
{
    unsigned int value = 0;
    unsigned int i;
    char c;

    for (i = 0; i < size; i++)
    {
        c = s[i];

        if (c == '\0')
        {
            break;
        }

        value = ((c - ((c & (c >> 1)) & 0x20)) & 0xFF) + (value * 131);
    }

    return value;
}

char *xStrTok(char *string, const char *control, char **nextoken)
{
    BFBBSTUB("xStrTok");
    return strtok(string, control);
}

int xStricmp(const char *string1, const char *string2)
{
    BFBBSTUB("xStricmp");
    return stricmp(string1, string2);
}

int xStrParseFloatList(float *dest, const char *strbuf, int max)
{
    BFBBSTUB("xStrParseFloatList");
    return 0;
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

substr substr::create(const char *text, unsigned int size)
{
    substr s;
    s.text = text;
    s.size = size;
    return s;
}

const char *skip_ws(substr &s)
{
    return skip_ws(s.text, s.size);
}

const char *skip_ws(const char *&text, unsigned int &size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        char c = *text;

        if (c == '\0')
        {
            break;
        }

        if (!is_ws(c))
        {
            size -= i;
            break;
        }

        text++;
    }

    return text;
}

unsigned int atox(const substr &s)
{
    unsigned int read_size;
    return atox(s, read_size);
}

unsigned int atox(const substr &s, unsigned int &read_size)
{
    const char *p = s.text;
    unsigned int size = s.size;

    if (!p)
    {
        return 0;
    }

    unsigned int total = 0;

    if (size > 8)
    {
        size = 8;
    }

    read_size = 0;

    while (read_size < size)
    {
        unsigned int v;

        if (*p >= '0' && *p <= '9')
        {
            v = *p - '0';
        }
        else if (*p >= 'a' && *p <= 'f')
        {
            v = *p - 'a' + 10;
        }
        else if (*p >= 'A' && *p <= 'F')
        {
            v = *p - 'A' + 10;
        }
        else
        {
            break;
        }

        total = (total << 4) + v;
        p++;
        read_size++;
    }

    return total;
}

unsigned int rskip_ws(substr &s)
{
    return rskip_ws(s.text, s.size);
}

unsigned int rskip_ws(const char *&text, unsigned int &size)
{
    while (size)
    {
        char c = text[size - 1];

        if (!is_ws(c))
        {
            break;
        }

        size--;
    }

    return size;
}

bool is_ws(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

const char *find_char(const substr &s, char c)
{
    if (!s.text)
    {
        return NULL;
    }

    const char *p = s.text;
    
    for (int i = s.size; i > 0; i--)
    {
        if (*p == '\0')
        {
            break;
        }

        if (*p == c)
        {
            return p;
        }

        p++;
    }

    return NULL;
}

const char *find_char(const substr &s, const substr &cs)
{
    if (!s.text || !cs.text)
    {
        return NULL;
    }

    const char *p = s.text;
    const char *d = cs.text;

    switch (cs.size)
    {
    case 0:
        break;

    case 1:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0])
            {
                return p;
            }

            p++;
        }
    }

    case 2:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1])
            {
                return p;
            }

            p++;
        }
    }

    case 3:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2])
            {
                return p;
            }

            p++;
        }
    }

    case 4:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3])
            {
                return p;
            }

            p++;
        }
    }

    case 5:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4])
            {
                return p;
            }

            p++;
        }
    }

    case 6:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5])
            {
                return p;
            }

            p++;
        }
    }

    case 7:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5] ||
                *p == d[6])
            {
                return p;
            }

            p++;
        }
    }

    case 8:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5] ||
                *p == d[6] ||
                *p == d[7])
            {
                return p;
            }

            p++;
        }
    }

    case 9:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5] ||
                *p == d[6] ||
                *p == d[7] ||
                *p == d[8])
            {
                return p;
            }

            p++;
        }
    }

    case 10:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5] ||
                *p == d[6] ||
                *p == d[7] ||
                *p == d[8] ||
                *p == d[9])
            {
                return p;
            }

            p++;
        }
    }

    case 11:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            if (*p == d[0] ||
                *p == d[1] ||
                *p == d[2] ||
                *p == d[3] ||
                *p == d[4] ||
                *p == d[5] ||
                *p == d[6] ||
                *p == d[7] ||
                *p == d[8] ||
                *p == d[9] ||
                *p == d[10])
            {
                return p;
            }

            p++;
        }
    }

    default:
    {
        for (int i = s.size; i > 0; i--)
        {
            if (*p == '\0')
            {
                break;
            }

            const char *s = d;

            while (*s != '\0')
            {
                if (*p == *s)
                {
                    return p;
                }

                s++;
            }

            p++;
        }
    }
    }

    return NULL;
}