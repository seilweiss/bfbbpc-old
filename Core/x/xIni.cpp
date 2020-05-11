#include "xIni.h"

#include "xString.h"

#include "print.h"

#include <rwcore.h>

#include <stdlib.h>
#include <cstring>

static char *TrimWhitespace(char *str)
{
    while (*str == ' ' || *str == '\t')
    {
        str++;
    }

    if (*str == '\0')
    {
        return str;
    }

    char *r4 = strlen(str) + str - 1;

    while (*r4 == ' ' || *r4 == '\t')
    {
        r4--;
    }

    *(r4 + 1) = '\0';
    return str;
}

xIniFile *xIniParse(char *buf, int len)
{
    int i;
    int ccr = 1;
    int clf = 1;
    int copen = 0;
    int lastCRLF = -1;
    char *c = buf;
    char *lastLine;
    //char *line;
    char *ltoken;
    xIniFile *ini;

    for (i = 0; i < len; i++, c++)
    {
        switch (*c)
        {
        case '\n':
        {
            lastCRLF = i;
            clf++;
            break;
        }

        case '\r':
        {
            lastCRLF = i;
            ccr++;
            break;
        }

        case '[':
        {
            copen++;
            break;
        }
        }
    }

    if (clf > ccr)
    {
        ccr = clf;
    }

    ini = (xIniFile *)RwMalloc(
            ccr * sizeof(xIniValue) + copen * sizeof(xIniSection) +
            (len - lastCRLF) + sizeof(xIniFile),
        rwMEMHINTDUR_FUNCTION);

    ini->mem = NULL;

    len -= lastCRLF + 1;

    ini->NumValues = 0;
    ini->NumSections = 0;
    ini->Values = (xIniValue *)(ini + 1);
    ini->Sections = (xIniSection *)(ini->Values + ccr);

    lastLine = (char *)(ini->Sections + copen);
    strncpy(lastLine, buf + lastCRLF + 1, len);

    lastLine[len] = '\0';

    if (lastCRLF >= 0)
    {
        buf[lastCRLF] = '\0';
    }
    else
    {
        buf[0] = '\0';
    }

    char *r3 = xStrTok(buf, "\n\r", &ltoken);

    if (!r3)
    {
        r3 = xStrTok(lastLine, "\n\r", &ltoken);
        lastLine = NULL;
    }

    while (r3)
    {
        r3 = TrimWhitespace(r3);
        char *r27 = r3;

        if (*r3 != '#' && *r3 != '\0')
        {
            if (*r3 == '[')
            {
                r3 = std::strstr(r3, "]");

                if (r3)
                {
                    *r3 = '\0';
                    r3 = TrimWhitespace(r27 + 1);

                    if (*r3)
                    {
                        ini->Sections[ini->NumSections].sec = r3;
                        ini->Sections[ini->NumSections].first = ini->NumValues;
                        ini->Sections[ini->NumSections].count = 0;

                        ini->NumSections++;
                    }
                }
            }
            else
            {
                char *r29 = std::strstr(r3, "=");

                if (r29)
                {
                    *r29 = '\0';
                    r3 = TrimWhitespace(r27);
                    r27 = r3;

                    if (*r3)
                    {
                        r29++;
                        r3 = std::strstr(r29, "#");

                        if (r3)
                        {
                            *r3 = '\0';
                        }

                        r3 = TrimWhitespace(r29);

                        ini->Values[ini->NumValues].tok = r27;
                        ini->Values[ini->NumValues].val = r3;

                        ini->NumValues++;

                        if (ini->NumSections)
                        {
                            ini->Sections[ini->NumSections - 1].count++;
                        }
                    }
                }
            }
        }

        r3 = xStrTok(NULL, "\n\r", &ltoken);

        if (!r3 && lastLine)
        {
            r3 = xStrTok(lastLine, "\n\r", &ltoken);
            lastLine = NULL;
        }
    }

    return ini;
}

void xIniDestroy(xIniFile *ini)
{
    RwFree(ini->mem);
    RwFree(ini);
}

int xIniGetIndex(xIniFile *ini, const char *tok)
{
    for (int i = 0; i < ini->NumValues; i++)
    {
        if (xStricmp(ini->Values[i].tok, tok) == 0)
        {
            return i;
        }
    }

    return -1;
}

int xIniGetInt(xIniFile *ini, const char *tok, int def)
{
    int index = xIniGetIndex(ini, tok);

    if (index == -1)
    {
        return def;
    }

    return atoi(ini->Values[index].val);
}

float xIniGetFloat(xIniFile *ini, const char *tok, float def)
{
    int index = xIniGetIndex(ini, tok);

    if (index == -1)
    {
        return def;
    }

    return atof(ini->Values[index].val);
}

const char *xIniGetString(xIniFile *ini, const char *tok, const char *def)
{
    int index = xIniGetIndex(ini, tok);

    if (index == -1)
    {
        return def;
    }

    return ini->Values[index].val;
}