//
// JMLib - Portable JuggleMaster Library
// Version 2.0                                                                  
// (C) Per Johan Persson 2000-2002, Gary Briggs 2003-2005
//                                                                              
// Based on JuggleMaster Version 1.60                                           
// Copyright (c) 1995-1996 Ken Matsuoka                                         
//                                                                              
// You may redistribute and/or modify JMLib under the terms of the              
// Modified BSD License as published in various places online or in the         
// COPYING.jmlib file in the package you downloaded.                            
//                                                                              
// This program is distributed in the hope that it will be useful, but          
// WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of          
// MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the              
// Modified BSD License for more details.
//


function blah() {
	%blah.a = "Bunnies";
	%fzorgle.ck[0] = %blah;
	%foo = %fzorgle.ck[0].a;
	echo(%foo);
}
blah();

function show_arm_line(%this) {
	echo("Arm Line:");
	for(%i=0;%i<5;%i++) {
		echo(" Right " @ %i @ ":" SPC %this.ap.rx[%i] SPC %this.ap.rx[%i]);
		echo(" Left " @ %i @ ":" SPC %this.ap.lx[%i] SPC %this.ap.lx[%i]);
	}
}
