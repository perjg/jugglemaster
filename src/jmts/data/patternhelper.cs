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


function JMPatterns::OnDelete(%this) {
	for(%i=0;%i<%this.numgroups;%i++) {
		%this.groups[%i].delete();
	}
}

function JMPatternGroups::OnDelete(%this) {
	for(%i=0;%i<%this.numpatts;%i++) {
		%this.pattern[%i].delete();
	}
}

function JMStyles::OnDelete(%this) {
	for(%i=0;%i<%this.numstyles;%i++) {
		%this.style[%i].delete();
	}
}


// Example code:
// %i = $jmstyles.FindStyle("Windmill");
// if(%i > -1) {
//   echo("Style Name:" SPC $jmstyles.GetStyleName(%i));
//   echo("Style Data:" SPC $jmstyles.GetStyleData(%i));
//   echo("Style Length:" SPC $jmstyles.GetStyleLength(%i));
// }

function JMStyles::FindStyle(%this, %name) {
	for(%i=0;%i<%this.numstyles;%i++) {
		if(stricmp(%this.style[%i].name, %name) == 0) {
			return %i;
		}
	}
	return -1;
}

function JMStyles::GetStyleName(%this, %index) {
	return %this.style[%index].name;
}

function JMStyles::GetStyleData(%this, %index) {
	return %this.style[%index].data;
}

function JMStyles::GetStyleLength(%this, %index) {
	return %this.style[%index].length;
}


function JMPatterns::NumGroups(%this) {
	return %this.numgroups;
}

function JMPatterns::FindGroup(%this, %name) {
	for(%i=0;%i<%this.numgroups;%i++) {
		if(stricmp(%this.groups[%i].name, %name) == 0) {
			return %i;
		}
	}
	return -1;
}

function JMPatterns::GetGroupName(%this, %index) {
	return %this.groups[%index].name;
}

function JMPatterns::GetGroupNumPatterns(%this, %index) {
	return %this.groups[%index].numpatts;
}

function JMPatterns::GetPatternName(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].name;
}

function JMPatterns::GetPatternStyle(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].style;
}

function JMPatterns::GetPatternSite(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].site;
}
