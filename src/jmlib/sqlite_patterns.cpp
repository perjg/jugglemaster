/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
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

#include "jmlib.h"
#include "sqlite_patterns.h"
#include "patterns.h"

/*
 * Pattern:
 * name, pattern, dr, hr, object-style, style, 
 *
 */


void PatternLoader::initializeDatabase(FILE* out, FILE* inJM, FILE* inJS) {
	groups_t jm_groups;
	styles_t jm_styles;
	groups_t js_groups;

	if (!out) return;
	if (!inJM && !inJS) return;

	if (inJM) {
		int result = ParsePatterns(inJM, &jm_groups, &jm_styles);

		pattern_group_t* groups = jm_groups.first;
		pattern_t* patterns = NULL;

		while (groups) {
			patterns = groups->first_patt;

			while (patterns) {

				patterns = patterns->next;
			}

			groups = groups->next;
		}

		style_t* styles = jm_styles.first;

		while (styles) {

			styles = styles->next;
		}
	}

	if (inJS) {
		js_groups;
	}
}

