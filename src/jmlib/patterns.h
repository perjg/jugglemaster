/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 


#ifndef PATTERNS__HDR_
#define PATTERNS__HDR_

#include "jmlib.h"

struct style_t;
struct pattern_t;
struct pattern_group_t;

struct style_t {
	JML_CHAR *name;
	JML_INT8 *data;
	JML_UINT8 length;
	struct style_t *next;
};

struct pattern_group_t {
	JML_CHAR *name;
	struct pattern_t *first_patt;
	struct pattern_group_t *next;
};

struct pattern_t {
	JML_CHAR *style;
	JML_CHAR *name;
	JML_CHAR *data;
	JML_FLOAT hr, dr; /* Height Ratio, Dwell Ratio */
	JML_FLOAT ga, sp; /* Gravity, Speed */
	int bgred, bggreen, bgblue; /* Background color */
	int bp, hd, pd, mr; /* Beep, Hand, Pattern, Mirror */
	struct pattern_t *next;
};

struct groups_t {
	struct pattern_group_t *first;
};

struct styles_t {
	struct style_t *first;
};

/* groups and styles better both exist and be allocated.
   If you pass it null pointers, it will instantly, silently, bail. */
int ParsePatterns(FILE *input,
	struct groups_t *groups, struct styles_t *styles);

/* Use FreePatterns and FreeStyles to delete the data structures created */
void FreeGroups(struct groups_t *groups);
void FreeStyles(struct styles_t *styles);


/* Iterate across groups */
struct pattern_group_t *FirstGroup(struct groups_t *g);
struct pattern_group_t *NextGroup(struct pattern_group_t *g);
const JML_CHAR *Group_GetName(struct pattern_group_t *g);
struct pattern_t *Group_GetPatterns(struct pattern_group_t *g);

/* Iterate across patterns [only makes sense as part of a group] */
struct pattern_t *NextPatt(struct pattern_t *p);
const JML_CHAR *Patt_GetName(struct pattern_t *p);
const JML_CHAR *Patt_GetData(struct pattern_t *p);
const JML_CHAR *Patt_GetStyle(struct pattern_t *p);
const JML_FLOAT Patt_GetDR(struct pattern_t *p);
const JML_FLOAT Patt_GetHR(struct pattern_t *p);

/* Iterate across styles */
struct style_t *FirstStyle(struct styles_t *f);
struct style_t *NextStyle(struct style_t *p);
const JML_CHAR *Style_GetName(struct style_t *s);
const JML_INT8 *Style_GetData(struct style_t *s);
const JML_UINT8 Style_GetLength(struct style_t *s);
struct style_t *Find_Style(styles_t *style_list, const char *name);

/* Checks to see if character is legal first character for pattern */
int legal_pattern_first_char(char c);


/* MSVC++ doesn't have strcasecmp, so add our own version that wraps _stricmp */
#ifdef WIN32 ///FIX ME - DO THIS RIGHT
    #define strcasecmp(a, b) _stricmp(a, b)
#endif


#endif
