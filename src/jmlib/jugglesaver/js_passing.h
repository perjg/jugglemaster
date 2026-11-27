/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2026
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 */

#ifndef JS_PASSING_H
#define JS_PASSING_H

#include "jugglesaver.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Passing patterns written per juggler, and solo sync patterns, turned into
 * the engine's interleaved form: a "J<n>:<site>" site where slot s belongs to
 * hand s mod 2n, plus a time shift per hand.  A throw from slot s is made at
 * time s + HandShift[hand], which lets jugglers (or a juggler's two hands)
 * throw at the same moment.
 *
 * Notations:
 *   Prechac      "4.5p 3 3", "4p 3", "3.67p 3": every juggler throws the
 *                same sequence, juggler j starting j * Count / Jugglers beats
 *                after juggler 0
 *   synchronous  "<3p 3 3 3|3p 3 3 3>", "<3p2 3p3|3p1 3|3 3p1>": one bar per
 *                juggler, all throwing at the same moments
 *   solo sync    "(4c,4c)", "(4c,2cx)(2cx,4c)": both hands at once, as in
 *                JuggleMaster
 * Throws are in beats; "p" passes to the next juggler and "pN" to juggler N
 * (1-based).  Anything after that is Juggle Saver throw syntax for the throw:
 * the object (b, c, r, d), positions, snatch and spins ("4.5pc*2"). */

#define PASSING_MAX_TOKENS 64
#define PASSING_MAX_SUFFIX 48

#define PASSING_SELF -2
#define PASSING_NEXT -1

typedef struct
{
    int Slots;    /* the throw's length in slots (beats * jugglers) */
    int To;       /* receiving juggler, or PASSING_SELF / PASSING_NEXT */
    int Cross;    /* solo sync: "x" */
    char Suffix[PASSING_MAX_SUFFIX];  /* object, positions, spins as written */
    int Start;    /* where the token is in the row's text */
    int Stop;
} PASSING_TOKEN;

typedef struct
{
    int Count;
    PASSING_TOKEN Tokens[PASSING_MAX_TOKENS];
    int TextStart;  /* the row's text in the pattern as written */
    int TextStop;
} PASSING_ROW;

#define PASSING_PRECHAC 1
#define PASSING_SYNC 2
#define PASSING_SOLO_SYNC 3

typedef struct
{
    int Notation;
    int Jugglers;                  /* 0 when the text is not a valid pattern */
    int Offset[JS_MAX_JUGGLERS];   /* when each juggler starts, in slots */
    PASSING_ROW Rows[JS_MAX_JUGGLERS];
    int Formation;
    int Feeder;
} PASSING_PATTERN;

/* Whether Site is written in one of the notations above.  Fills pPattern when
 * it is, with Jugglers = 0 if the throws do not make sense. */
int ParsePassing(const char* Site, PASSING_PATTERN* pPattern);

/* The J<n>: site of a parsed pattern (malloc'd) and its layout, or NULL when a
 * throw does not land on a beat of the juggler it is written for. */
char* PassingToSite(const PASSING_PATTERN* pPattern, PASSING_LAYOUT* pLayout);

#ifdef __cplusplus
}
#endif

#endif
