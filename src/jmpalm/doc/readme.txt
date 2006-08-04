JMPalm - JuggleMaster for Palm OS
Version 1.2
01.08.2006

(C) Per Johan Groland 2001-2006

Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
Based on JuggleMaster Version 1.60
Copyright (C) 1995-1996 Ken Matsuoka

JMPalm is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.



1. INTRODUCTION

JMPalm is a port of the juggling simulator JuggleMaster to
PalmOS. It should run on any device running PalmOS v2.0 or
greater.


2. FEATURES

Here is a list of some of the features of JMPalm.

* Full support for siteswaps, this includes vanilla siteswap as well
  as synchronous and multiplexing siteswap.
* Styles. For instance, a style defining how to juggle a pattern as
  a Mills' Mess is defined.
* Support for the full JuggleMaster pattern library.
* Enter any siteswap and have JMPalm juggle it.
* Turn the juggler on and off (glo-ball feature :)
* Juggle in landscape or portrait mode.
* Color support
* Speed adjustment.
* High resolution support on Palm OS 5.0 (including Sony devices)
* JMC (JuggleMaster Pattern Converter) Utility for converting a regular
  JuggleMaster pattern library file (e.g. patt_e.jm) into a JMPalm pattern
  library file. This utility must be downloaded separately. Please note
  that this utility will not run on Palm OS, but rather on desktop
  computers (Windows, Unix, Linux etc.). Furthermore, the current version
  is a command line utility.

To see a list of stuff that has changed between different versions of
JMPalm, see the file changes.txt.


3. INSTALLATION

First, uninstall all previous JMPalm versions. Then, install the files
jmpalm.prc and jmpalmdb.pdb. The former is the program file, and the
latter is the pattern database. The pattern library included is
Isaac Orr's extended pattern library (372 patterns).

PS: You can install any compatible pattern file you wish, but JMPalm
will not function without one. Do not attempt to install a pattern
file included with any other JuggleMaster (e.g patt_e.jm).
To convert a regular JuggleMaster pattern library into JMPalm format,
use JMC, which must be downloaded separately.

Here is the complete list of files included:

jmpalm.prc	The program file for installation on the Palm.
jmpalmdb.pdb	The pattern library.
readme.txt	This file.
changes.txt	A list of changes in each version of JMPalm.


4. CONTROLS

Tapping the screen will toggle pause on and off.

The hard buttons can be used for quick access to a number of features.
Initially, the up and down buttons can be used to increase and decrease
the juggler's speed. No other hard button is assigned any specific
function (and will function as normal). To assign hard buttons, select
Preferences from the menu and choose Buttons.

All other features are available from the menu. Press the menu
silk button to access the menu, and JMPalm's menu will appear.
The menu contains the following items:

File
  Select Pattern              This will allow you to select a pattern
                              from JMPalm's extensive pattern library.
  Enter Siteswap              This will allow you to enter a siteswap.
                              Style, dwell ratio and height ratio can
                              be selected as well.
  About JMPalm                Show copyright and version information.
Edit
  Toggle Globall Mode         Turns globall mode on/off.
  Toggle Landscape Mode       Turns landscape mode on/off.
  Preferences                 Preferences for JMPalm.

Preferences are split into two pages:

General:

- Uniform Ball Color          If turned on, a single color (red) will
                              be used for all balls in a pattern, rather
                              than one color for each ball. This setting
                              is ignored for non-color palms.
- Mirror Pattern              This will mirror the entire juggler.
- Globall Mode                Turn globall mode on/off
- Height Ratio Reduction      If turned on, the height ratio on all
                              built-in patterns will be reduced by 20
                              percent. The height ratios for the
                              built-in patterns are optimized for a larger
                              screen. A reduction of 20 % will usually
                              look better on the Palm's small screen.
- Warning for Min/Max Speed   Give a warning when adjusting the speed
                              whenever the maximum or minimum speed is
                              reached.
- Landscape mode              When landscape mode is turned on, the juggler
                              will be shown rotated 90 degrees (i.e. sideways)

Buttons:

This will allow you to configure each of your Palm's hard buttons
to perform certain JMPalm actions. If you leave a setting at
"Default," the global button settings will be retained. This usually
means to launch the Calendar when the Calendar button is pressed etc.


5. REPORTING BUGS

If you find a bug, you should consider reporting it. However, before
reporting bugs, check that:

a) Do you have the latest version. If not, always try the latest
   version and see if the bug has been fixed.

b) Is the bug a known bug? If it is there is no use in reporting it.
   See the next section for known bugs.

To report the bug, send me a mail. Please try to be as specific as
possible.


6. KNOWN BUGS AND OTHER CAVEATS

For an up-to-date list of all known bugs, see the web page.

If you have an older Palm OS device (version 4.X or lower) and
JMPalm runs too slowly on your device, you can try to download
and run version 1.1 instead. Version 1.1 does not support Palm
OS 5.0, but is somewhat faster than the new version on old devices.


7. TERMS OF USE

JMPalm is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

8. CONTACT

The official JMPalm site can be found at:
http://jugglemaster.net

Fan mail, constructive criticism, suggestions for improvement,
and bug reports are always welcome, and should be sent by email.

email: jm@jugglemaster.net

Enjoy!
