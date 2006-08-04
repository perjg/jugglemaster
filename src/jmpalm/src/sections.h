/*
 * JMPalm - JuggleMaster for Palm OS
 * Version 1.2
 * (C) Per Johan Groland 2001-2006
 *
 * Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPalm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

// --------------------------------------------------------------------------
/*
	To build a multi-section application
	1. In makefile, set MULTIPLE_CODE_SECTIONS = TRUE
	2. Edit Sections.def to specify the number of code sections desired
	3. Modify this file to match the code sections
	   (Sections.def and this file are set up with 2 extra sections
	   by default.)
	4. Include this file in project sources.
	5. Annotate each function with a section define like so: 
		void DoWork() EXTRA_SECTION_ONE;
		void DrawForm() EXTRA_SECTION_TWO;
	   Notice that the annotations need to be visible at the function
	   definition, and everywhere the function is used.  Any function
	   without an annotation will go into the default code section.

	To effectively disable the multi-section annotations, 
	just define these section macros to expand to nothing.

	Be sure to follow the instructions and warnings given at:
	http://prc-tools.sourceforge.net/doc/prc-tools_3.html#SEC17

*/
// --------------------------------------------------------------------------

#ifndef _SECTIONS_H
#define _SECTIONS_H

#define EXTRA_SECTION_ONE  __attribute__ ((section ("code1")))
#define EXTRA_SECTION_TWO  __attribute__ ((section ("code2")))

#endif 
