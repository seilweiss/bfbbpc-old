#ifndef XSTRING_H
#define XSTRING_H

struct substr
{
    const char *text;
    unsigned int size;

    static substr create(const char *text, unsigned int size);
};

unsigned int xStrHash(const char *str);
unsigned int xStrHash(const char *s, unsigned int size);

int imemcmp(const void *d1, const void *d2, unsigned int size);
int icompare(const substr &s1, const substr &s2);

const char *skip_ws(substr &s);
const char *skip_ws(const char *&text, unsigned int &size);
unsigned int rskip_ws(substr &s);
unsigned int rskip_ws(const char *&text, unsigned int &size);
bool is_ws(char c);
const char *find_char(const substr &s, char c);
const char *find_char(const substr &s, const substr &cs);

#endif