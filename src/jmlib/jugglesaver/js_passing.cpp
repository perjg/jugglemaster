/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2026
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 */

#include "js_passing.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Largest slot value the J<n>: site can spell (0-9, A-Z) */
#define MAX_SLOT_VALUE 35

typedef struct
{
    double Value;
    int Pass;
    int Target;   /* 1-based juggler after 'p', 0 for the next one */
    char Suffix[PASSING_MAX_SUFFIX];
    int Start;
    int Stop;
} RAW_TOKEN;

static int IsDigit(char c) { return c >= '0' && c <= '9'; }

static int IsObject(char c) { return c == 'b' || c == 'c' || c == 'r' || c == 'd'; }

static int IsDelimiter(char c) { return c == '\0' || c == ' ' || c == '|' || c == '>'; }

/* Reads "<int>[.<digits>][p[<int>]][suffix]" ending at a space, '|', '>' or the
 * end, where the suffix is Juggle Saver throw syntax starting with an object
 * letter or '@', '/' or '*' */
static int ReadThrow(const char* Text, int* pPos, RAW_TOKEN* pToken)
{
    const char* p = Text + *pPos;
    const char* begin = p;
    double Scale = 1.0;

    if (!IsDigit(*p))
        return 0;
    pToken->Value = 0.0;
    while (IsDigit(*p) && pToken->Value < 1000.0)
        pToken->Value = pToken->Value * 10.0 + (*p++ - '0');
    if (*p == '.')
    {
        p++;
        if (!IsDigit(*p))
            return 0;
        while (IsDigit(*p))
        {
            Scale /= 10.0;
            pToken->Value += Scale * (*p++ - '0');
        }
    }
    if (IsDigit(*p))
        return 0;  /* too long to be a throw */

    pToken->Pass = 0;
    pToken->Target = 0;
    if (*p == 'p')
    {
        pToken->Pass = 1;
        p++;
        while (IsDigit(*p) && pToken->Target <= JS_MAX_JUGGLERS)
            pToken->Target = pToken->Target * 10 + *p++ - '0';
        if (IsDigit(*p))
            return 0;
    }

    if (!IsDelimiter(*p) && !IsObject(*p) && *p != '@' && *p != '/' && *p != '*')
        return 0;
    {
        /* '>' ends a bar, except where it introduces a catch position */
        size_t n = 0;
        while (!IsDelimiter(p[n]) || (p[n] == '>' && p[n + 1] == '('))
            n++;
        if (n >= PASSING_MAX_SUFFIX)
            return 0;
        memcpy(pToken->Suffix, p, n);
        pToken->Suffix[n] = '\0';
        p += n;
    }

    pToken->Start = (int) (begin - Text);
    pToken->Stop = (int) (p - Text);
    *pPos = (int) (p - Text);
    return 1;
}

/* Reads space-separated throws until '|', '>' or the end */
static int ReadThrows(const char* Text, int* pPos, RAW_TOKEN* pRaw, int* pCount)
{
    *pCount = 0;
    for (;;)
    {
        while (Text[*pPos] == ' ')
            (*pPos)++;
        if (Text[*pPos] == '\0' || Text[*pPos] == '|' || Text[*pPos] == '>')
            return *pCount > 0;
        if (*pCount == PASSING_MAX_TOKENS || !ReadThrow(Text, pPos, &pRaw[*pCount]))
            return 0;
        (*pCount)++;
    }
}

static int WholeSlots(double Value, int Jugglers, int* pSlots)
{
    double Slots = Value * Jugglers;
    int Rounded = (int) floor(Slots + 0.5);
    /* allow the two-decimal thirds and sixths people write (3.33p, 3.67p) */
    if (fabs(Slots - Rounded) > 0.05)
        return 0;
    *pSlots = Rounded;
    return 1;
}

/* Converts raw throws to tokens of a row; 0 if a value is not a whole number
 * of slots or a pass names a juggler that is not there */
static int FillRow(PASSING_ROW* pRow, const RAW_TOKEN* pRaw, int Count, int Jugglers, int Base)
{
    int i;
    pRow->Count = Count;
    for (i = 0; i < Count; i++)
    {
        PASSING_TOKEN* t = &pRow->Tokens[i];
        if (!WholeSlots(pRaw[i].Value, Jugglers, &t->Slots) || pRaw[i].Target > Jugglers)
            return 0;
        t->To = pRaw[i].Pass ? (pRaw[i].Target > 0 ? pRaw[i].Target - 1 : PASSING_NEXT) : PASSING_SELF;
        t->Cross = 0;
        strcpy(t->Suffix, pRaw[i].Suffix);
        t->Start = pRaw[i].Start - Base;
        t->Stop = pRaw[i].Stop - Base;
    }
    return 1;
}

static int ReadPrefix(const char* Site, const char** pText)
{
    const char* p = Site + 1;
    int n = 0;

    *pText = Site;
    if (Site[0] != 'J' || !IsDigit(Site[1]))
        return 0;
    while (IsDigit(*p) && n <= JS_MAX_JUGGLERS)
        n = n * 10 + *p++ - '0';
    if (*p != ':')
        return 0;
    *pText = p + 1;
    return n;
}

static int ParsePrechac(const char* Site, PASSING_PATTERN* pPattern)
{
    RAW_TOKEN Raw[PASSING_MAX_TOKENS];
    const char* Text;
    int Jugglers = ReadPrefix(Site, &Text);
    int Count, Pos = 0, HasPrechac = 0, i, j;
    int Base = (int) (Text - Site);

    if (!ReadThrows(Text, &Pos, Raw, &Count) || Text[Pos] != '\0')
        return 0;
    for (i = 0; i < Count; i++)
    {
        if (Raw[i].Pass || Raw[i].Value != floor(Raw[i].Value))
            HasPrechac = 1;
    }
    if (!HasPrechac)
        return 0;

    pPattern->Notation = PASSING_PRECHAC;
    pPattern->Formation = FORMATION_AUTO;
    pPattern->Feeder = -1;
    if (Jugglers == 0)
    {
        /* the fewest jugglers that make every throw a whole number of slots */
        for (j = 2; j <= JS_MAX_JUGGLERS && Jugglers == 0; j++)
        {
            int Slots;
            for (i = 0; i < Count && WholeSlots(Raw[i].Value, j, &Slots); i++)
                ;
            if (i == Count)
                Jugglers = j;
        }
    }
    pPattern->Jugglers = 0;
    if (Jugglers < 2 || Jugglers > JS_MAX_JUGGLERS)
        return 1;

    for (j = 0; j < Jugglers; j++)
    {
        PASSING_ROW* pRow = &pPattern->Rows[j];
        if (!FillRow(pRow, Raw, Count, Jugglers, 0))
            return 1;
        pRow->TextStart = Base;
        pRow->TextStop = Base + (int) strlen(Text);
        pPattern->Offset[j] = j * Count;
    }
    pPattern->Jugglers = Jugglers;
    return 1;
}

/* The juggler who passes to each of the others, who only pass back; -1 if none */
static int FindFeeder(const PASSING_PATTERN* pPattern)
{
    int J = pPattern->Jugglers, f, j, k;
    if (J < 3)
        return -1;
    for (f = 0; f < J; f++)
    {
        int Ok = 1, Fed = 0;
        for (j = 0; j < J && Ok; j++)
        {
            const PASSING_ROW* pRow = &pPattern->Rows[j];
            for (k = 0; k < pRow->Count && Ok; k++)
            {
                int To = pRow->Tokens[k].To;
                if (To == PASSING_SELF)
                    continue;
                if (To == PASSING_NEXT)
                    To = (j + 1) % J;
                Ok = j == f || To == f;
                if (j == f)
                    Fed |= 1 << To;
            }
        }
        if (Ok && Fed == ((1 << J) - 1) - (1 << f))
            return f;
    }
    return -1;
}

static int ReadBars(const char* Site, PASSING_PATTERN* pPattern, RAW_TOKEN (*Raw)[PASSING_MAX_TOKENS]);

static int ParseSync(const char* Site, PASSING_PATTERN* pPattern)
{
    RAW_TOKEN (*Raw)[PASSING_MAX_TOKENS];
    int Result;

    if (Site[0] != '<')
        return 0;
    Raw = (RAW_TOKEN (*)[PASSING_MAX_TOKENS]) malloc(sizeof(RAW_TOKEN) * JS_MAX_JUGGLERS * PASSING_MAX_TOKENS);
    Result = ReadBars(Site, pPattern, Raw);
    free(Raw);
    return Result;
}

/* Fills pPattern from the bars of "<a b|c d>" */
static int ReadBars(const char* Site, PASSING_PATTERN* pPattern, RAW_TOKEN (*Raw)[PASSING_MAX_TOKENS])
{
    int Count[JS_MAX_JUGGLERS];
    int Pos = 1, J = 0, i, k;
    pPattern->Notation = PASSING_SYNC;
    pPattern->Formation = FORMATION_AUTO;
    pPattern->Feeder = -1;
    pPattern->Jugglers = 0;

    for (;;)
    {
        PASSING_ROW* pRow = &pPattern->Rows[J];
        if (J == JS_MAX_JUGGLERS)
            return 1;
        while (Site[Pos] == ' ')
            Pos++;
        pRow->TextStart = Pos;
        if (!ReadThrows(Site, &Pos, Raw[J], &Count[J]))
            return 1;
        for (k = 0; k < Count[J]; k++)
        {
            if (Raw[J][k].Value != floor(Raw[J][k].Value))
                return 1;
        }
        pRow->TextStop = Pos;
        while (pRow->TextStop > pRow->TextStart && Site[pRow->TextStop - 1] == ' ')
            pRow->TextStop--;
        J++;
        if (Site[Pos] == '|')
        {
            Pos++;
            continue;
        }
        if (Site[Pos] == '>' && Site[Pos + 1] == '\0')
            break;
        return 1;
    }
    if (J < 2)
        return 1;

    for (i = 0; i < J; i++)
    {
        if (!FillRow(&pPattern->Rows[i], Raw[i], Count[i], J, pPattern->Rows[i].TextStart))
            return 1;
        pPattern->Offset[i] = 0;
    }
    pPattern->Jugglers = J;
    pPattern->Feeder = FindFeeder(pPattern);
    if (pPattern->Feeder >= 0)
        pPattern->Formation = FORMATION_FEED;
    return 1;
}

/* "(<even>[x][obj],<even>[x][obj])" pairs, with the object letter before or after the x */
static int ParseSoloSync(const char* Site, PASSING_PATTERN* pPattern)
{
    PASSING_ROW* pRow = &pPattern->Rows[0];
    const char* p = Site;
    int Count = 0;

    while (*p == ' ')
        p++;
    if (*p != '(')
        return 0;

    pPattern->Notation = PASSING_SOLO_SYNC;
    pPattern->Formation = FORMATION_AUTO;
    pPattern->Feeder = -1;
    pPattern->Jugglers = 0;
    pPattern->Offset[0] = 0;
    pRow->TextStart = 0;
    pRow->TextStop = (int) strlen(Site);

    while (*p != '\0')
    {
        const char* Begin = p;
        int Hand;
        if (*p != '(' || Count + 2 > PASSING_MAX_TOKENS)
            return 1;
        p++;
        for (Hand = 0; Hand < 2; Hand++)
        {
            PASSING_TOKEN* t = &pRow->Tokens[Count + Hand];
            int Value = 0;
            if (!IsDigit(*p))
                return 1;
            while (IsDigit(*p) && Value <= MAX_SLOT_VALUE)
                Value = Value * 10 + *p++ - '0';
            if (IsDigit(*p))
                return 1;
            /* "x" before or after the object letter, then any other throw
             * syntax up to the ',' or ')' that ends the throw */
            t->Cross = 0;
            if (*p == 'x')
            {
                t->Cross = 1;
                p++;
            }
            {
                size_t n = 0;
                int Depth = 0;
                if (IsObject(*p))
                    t->Suffix[n++] = *p++;
                if (*p == 'x' && !t->Cross)
                {
                    t->Cross = 1;
                    p++;
                }
                while (*p != '\0' && (Depth > 0 || (*p != ',' && *p != ')')))
                {
                    if (n + 1 >= PASSING_MAX_SUFFIX)
                        return 1;
                    Depth += *p == '(' ? 1 : *p == ')' ? -1 : 0;
                    t->Suffix[n++] = *p++;
                }
                t->Suffix[n] = '\0';
            }
            if (Value % 2 != 0 || (t->Cross && Value == 0))
                return 1;
            t->Slots = Value;
            t->To = PASSING_SELF;
            if (*p != (Hand == 0 ? ',' : ')'))
                return 1;
            p++;
        }
        pRow->Tokens[Count].Start = pRow->Tokens[Count + 1].Start = (int) (Begin - Site);
        pRow->Tokens[Count].Stop = pRow->Tokens[Count + 1].Stop = (int) (p - Site);
        Count += 2;
        while (*p == ' ')
            p++;
    }
    pRow->Count = Count;
    pPattern->Jugglers = 1;
    return 1;
}

static char* BuildSite(const PASSING_PATTERN* pPattern, PASSING_LAYOUT* pLayout, int** pSlotsOut, int* pLenOut);

/* Whether every slot of the site the pattern makes is caught exactly once */
static int LandsCleanly(const PASSING_PATTERN* pPattern)
{
    PASSING_LAYOUT Layout;
    int* pSlots = NULL;
    int Len = 0, s, Ok = 1;
    char* pSite = BuildSite(pPattern, &Layout, &pSlots, &Len);
    unsigned char* pCaught;

    if (pSite == NULL)
        return 0;
    pCaught = (unsigned char*) calloc(Len, 1);
    for (s = 0; s < Len; s++)
    {
        int Land = (s + pSlots[s]) % Len;
        Ok = Ok && !pCaught[Land];
        pCaught[Land] = 1;
    }
    free(pCaught);
    free(pSlots);
    free(pSite);
    return Ok;
}

int ParsePassing(const char* Site, PASSING_PATTERN* pPattern)
{
    int j;
    if (ParseSync(Site, pPattern) || ParseSoloSync(Site, pPattern))
        return 1;
    if (!ParsePrechac(Site, pPattern))
        return 0;

    /* Prechac patterns usually start each juggler a fraction of the period
     * after the previous one ("4.5p 3 3"); whole-beat patterns such as the
     * 4-count "3p 3 3 3" are often written for jugglers throwing together, so
     * try that when the staggered start does not work. */
    if (pPattern->Jugglers > 1 && !LandsCleanly(pPattern))
    {
        for (j = 0; j < pPattern->Jugglers; j++)
            pPattern->Offset[j] = 0;
        if (!LandsCleanly(pPattern))
        {
            for (j = 0; j < pPattern->Jugglers; j++)
                pPattern->Offset[j] = j * pPattern->Rows[0].Count;
        }
    }
    return 1;
}

static int Gcd(int a, int b)
{
    while (b != 0)
    {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static int FloorDiv(int a, int b)
{
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

static char* SpellSite(int Jugglers, const int* pSlots, const PASSING_TOKEN** pTokens, int Len)
{
    /* Every throw gets an object letter: without one, a following B, C, D or R
     * value would be read as the object of the throw before it. */
    size_t Size = 8 + (size_t) Len * (PASSING_MAX_SUFFIX + 3);
    char* pSite = (char*) malloc(Size);
    int o = snprintf(pSite, Size, "J%d:", Jugglers), s;
    for (s = 0; s < Len; s++)
    {
        const char* Suffix = pTokens[s]->Suffix;
        int v = pSlots[s];
        pSite[o++] = (char) (v < 10 ? '0' + v : 'A' + v - 10);
        if (!IsObject(Suffix[0]))
            pSite[o++] = 'd';
        o += snprintf(pSite + o, Size - o, s + 1 < Len ? "%s " : "%s", Suffix);
    }
    pSite[o] = '\0';
    return pSite;
}

static char* SoloSyncToSite(const PASSING_PATTERN* pPattern, PASSING_LAYOUT* pLayout)
{
    const PASSING_ROW* pRow = &pPattern->Rows[0];
    int* pSlots = (int*) malloc(sizeof(int) * pRow->Count);
    const PASSING_TOKEN** pTokens = (const PASSING_TOKEN**) malloc(sizeof(PASSING_TOKEN*) * pRow->Count);
    char* pSite;
    int s;

    /* As JuggleMaster does it: the left hand throws one slot later than the
     * right, shifted back to throw together; a crossing throw from the right
     * is one slot longer and one from the left one slot shorter. */
    for (s = 0; s < pRow->Count; s++)
    {
        const PASSING_TOKEN* t = &pRow->Tokens[s];
        pSlots[s] = t->Slots + (t->Cross ? (s % 2 == 0 ? 1 : -1) : 0);
        pTokens[s] = t;
    }
    pSite = SpellSite(1, pSlots, pTokens, pRow->Count);
    memset(pLayout, 0, sizeof(*pLayout));
    pLayout->Jugglers = 1;
    pLayout->HandShift[1] = -1;
    free(pSlots);
    free(pTokens);
    return pSite;
}

/* PassingToSite, also handing back the slot values when pSlotsOut is given */
static char* BuildSite(const PASSING_PATTERN* pPattern, PASSING_LAYOUT* pLayout, int** pSlotsOut, int* pLenOut)
{
    int J = pPattern->Jugglers;
    int N = 1, Len, j, k;
    int* pSlots;
    const PASSING_TOKEN** pTokens;
    char* pSite;

    if (J < 1)
        return NULL;
    if (pPattern->Notation == PASSING_SOLO_SYNC)
        return SoloSyncToSite(pPattern, pLayout);

    for (j = 0; j < J; j++)
    {
        N = N / Gcd(N, pPattern->Rows[j].Count) * pPattern->Rows[j].Count;
        if (N > 4 * PASSING_MAX_TOKENS)
            return NULL;
    }
    Len = N * J;

    memset(pLayout, 0, sizeof(*pLayout));
    pLayout->Jugglers = J;
    pLayout->Formation = pPattern->Formation;
    pLayout->Feeder = pPattern->Feeder;

    /* Juggler j's k-th throw is made at time k * J + Offset[j]; it goes in
     * slot (k + Index[j]) * J + j.  Index[j] is even, so every juggler makes
     * its first throw with the right hand, and leaves the hand's shift in
     * [0, 2J). */
    for (j = 0; j < J; j++)
    {
        pLayout->Index[j] = 2 * FloorDiv(pPattern->Offset[j] - j, 2 * J);
        pLayout->HandShift[j] = pLayout->HandShift[j + J] = pPattern->Offset[j] - j - pLayout->Index[j] * J;
    }

    pSlots = (int*) malloc(sizeof(int) * Len);
    pTokens = (const PASSING_TOKEN**) malloc(sizeof(PASSING_TOKEN*) * Len);
    for (j = 0; j < J; j++)
    {
        const PASSING_ROW* pRow = &pPattern->Rows[j];
        for (k = 0; k < N; k++)
        {
            const PASSING_TOKEN* t = &pRow->Tokens[k % pRow->Count];
            int Receiver = t->To == PASSING_SELF ? j : t->To == PASSING_NEXT ? (j + 1) % J : t->To;
            if (t->To >= 0 && Receiver == j)
            {
                /* "pN" naming the thrower itself */
                free(pSlots);
                free(pTokens);
                return NULL;
            }
            int Slot = (k + pLayout->Index[j]) * J + j;
            int Land = k * J + pPattern->Offset[j] + t->Slots - pPattern->Offset[Receiver];
            int Weight = (FloorDiv(Land, J) + pLayout->Index[Receiver]) * J + Receiver - Slot;

            /* the throw must land on one of the receiver's beats */
            if (Land % J != 0 || Weight < 0 || Weight > MAX_SLOT_VALUE || (Weight == 0) != (t->Slots == 0))
            {
                free(pSlots);
                free(pTokens);
                return NULL;
            }
            pSlots[((Slot % Len) + Len) % Len] = Weight;
            pTokens[((Slot % Len) + Len) % Len] = t;
        }
    }

    pSite = SpellSite(J, pSlots, pTokens, Len);
    free(pTokens);
    if (pSlotsOut != NULL)
    {
        *pSlotsOut = pSlots;
        *pLenOut = Len;
    }
    else
    {
        free(pSlots);
    }
    return pSite;
}

char* PassingToSite(const PASSING_PATTERN* pPattern, PASSING_LAYOUT* pLayout)
{
    return BuildSite(pPattern, pLayout, NULL, NULL);
}
