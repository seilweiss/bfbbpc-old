#ifndef XINI_H
#define XINI_H

struct xIniValue
{
    char *tok;
    char *val;
};

struct xIniSection
{
    char *sec;
    int first;
    int count;
};

struct xIniFile
{
    int NumValues;
    int NumSections;
    xIniValue *Values;
    xIniSection *Sections;
    void *mem;
    char name[256];
    char pathname[256];
};

xIniFile *xIniParse(char *buf, int len);
void xIniDestroy(xIniFile *ini);
int xIniGetIndex(xIniFile *ini, const char *tok);
int xIniGetInt(xIniFile *ini, const char *tok, int def);
float xIniGetFloat(xIniFile *ini, const char *tok, float def);
const char *xIniGetString(xIniFile *ini, const char *tok, const char *def);

#endif