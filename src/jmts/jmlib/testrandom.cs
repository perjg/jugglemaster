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


function JMRand::testrandom(%tries) {
	%j = 0;
	for(%i=0;%i<%tries;%i++) {
		%s = JMRand::jm_rand(getRandom(1,10), getRandom(1,10), getRandom(0,10),
			getRandom(1,2)-1, 0);
		if(strstr(%s, "0x") != -1) {
			echo("Crossing Zero");
			%j++;
		} else if(JMSiteValidator::validateSite(%s) != 0) {
			%j++;
		}
	}
	echo("tries: " @ %i @ "    failures: " @ %j);
	echo("exec(\"jm/jmlib/random.cs\"); exec(\"jm/jmlib/testrandom.cs\"); t();");
}
