/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.03
 * (C) Per Johan Groland 2002-2004
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
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

/*
 * Dummy PalmOS.h header file.
 *
 * This file is added to ensure compatibility with Embedded Visual Tools 3.0.
 * In the file jmlib_types.h, the file PalmOS.h must be included when compiling
 * for Palm OS. This include is enclosed within #ifdef's and should only be parsed
 * when the PALMOS symbol is defined. Nevertheless, EVT 3.0 panics on this statement
 * if it sees #include <PalmOS.h> and will complain about a missing header file.
 * 
 * Including this dummy file instead thwarts the stupid bug.
 *
 * The Palm OS source (JMPalm) must include a similar PalmOS.h file, which must
 * include the actual statement #include <PalmOS.h>
 *
 */
