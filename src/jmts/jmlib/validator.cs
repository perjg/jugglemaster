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


// A randomly invalid character that can't appear in siteswap
$invalidsitechar="-";

function JMSiteValidator::validSiteChar(%c) {
	if( JMLib::siteDigit(%c) !$= 0 || %c $= "0" ||
		%c $= "[" || %c $= "]" || %c $= "(" || %c $= ")" || %c $= ",") {

		return 0;
	} else {
		return 1;
	}

}

 // Private Functions

// in C++, scanMSS, scanSSS are the only pass-by-reference functions.
// For us, if they return an invalid site char, that'll be an error condition

function JMSiteValidator::scanSSS(%site) {
	if(strlen(%site) <= 1) {
		return $invalidsitechar;
	}
	%site = getSubStr(%site, 1, strlen(%site)-1);
 
	%c = getSubStr(%site, 0, 1);
	if (%c $= "[") { // multiplex throw
		%site = JMSiteValidator::scanMSS(%site);
		if (%site $= $invalidsitechar) {
			return $invalidsitechar;
		}
	} else if (JMLib::siteDigit(%c) == 0 && %c !$= 0 ) { // single throw
    	return $invalidsitechar;
	}
  
	%site = getSubStr(%site, 1, strlen(%site)-1);

	%c = getSubStr(%site, 0, 1);
	if (%c $= "x") {
		%site = getSubStr(%site, 1, strlen(%site)-1);
	}

	%c = getSubStr(%site, 0, 1);
	if (%c !$= ",") { // "," expected
    	return $invalidsitechar;
	}

	if(strlen(%site) <= 1) {
		return $invalidsitechar; // premature end of site
	}

	%site = getSubStr(%site, 1, strlen(%site)-1);

	%c = getSubStr(%site, 0, 1);
	if (%c $= "[") { // multiplex throw
		%site = JMSiteValidator::scanMSS(%site);
		if (%site $= $invalidsitechar) {
			return $invalidsitechar;
		}
	} else if (JMLib::siteDigit(%c) == 0 && %c !$= "0") { // single throw
		return $invalidsitechar;
	}


	%site = getSubStr(%site, 1, strlen(%site)-1);

	%c = getSubStr(%site, 0, 1);
	if (%c $= "x") {
		%site = getSubStr(%site, 1, strlen(%site)-1);
	}

	%c = getSubStr(%site, 0, 1);
	if (%c !$= ")") { // ")" expected
		return $invalidsitechar;
	}


	%site = getSubStr(%site, 1, strlen(%site)-1);
	return %site;

}

function JMSiteValidator::scanMSS(%site) {
	%site = getSubStr(%site, 1, strlen(%site)-1);

	%c = getSubStr(%site, 0, 1);
	if (%c $= "]") { // empty multiplex throw
		return $invalidsitechar;
	}

	while (%c !$= "]") {

		if (strlen(%site) == 0) {
			return $invalidsitechar; // premature end of site
		}

		if (JMLib::siteDigit(%c) == 0 && %c !$= "0") { // only single site throws may appear
			return $invalidsitechar;
		}

		%site = getSubStr(%site, 1, strlen(%site)-1);
		%c = getSubStr(%site, 0, 1);
	}

	return %site;

}

// Fairly elegant solution
// Stash an array of length site_len with 1s, and iterate across the site
// For each throw in the site, decrement the place in the array where it lands
// By the time you're done, everything should equal zero

function JMSiteValidator::validateVSS(%site) {
	%len = strlen(%site);

	for(%i = 0; %i < %len; %i++) {
		%ls[%i] = 1;
	}
	
	for(%i = 0; %i < %len; %i++) {
		%x = JMLib::siteDigit(getSubStr(%site,%i,1));
		%ls[(%i+%x) % %len]--;
	}
  
	for(%i = 0; %i < %len; %i++) {
		if (%ls[%i] != 0) {
			return 1;
		}
	}

	return 0;
}

function JMSiteValidator::validateSSS(%site) {
	%MSS = JMSiteValidator::transSyncMSS(%site);
	if (strlen(%MSS) > 0) {
		if(strstr(%site, "[") != -1) { // MSS
    		return(JMSiteValidator::validateMSS(%MSS));
		} else {
    		return(JMSiteValidator::validateVSS(%MSS));
		}
	} else {
		return 1;
	}
}

// MSS Validation is the same as vanilla siteswap, except instead of
// filling arrays with 1s, you full it with {how many balls are thrown
// that throw}. This is a superset of validating Vanilla siteswaps.

function JMSiteValidator::validateMSS(%site) {
	%len = strlen(%site);

	%throwCount = 0;

	for(%i = 0; %i < %len; %i++) {
		%ls[%i] = 0;
	}

	%lsLen = 0;
	for(%i = 0; %i < %len; %i++) {
		if (getSubStr(%site, %i, 1) $= "[") {
			%i++;
			while (getSubStr(%site, %i, 1) !$= "]") {
				%ls[%lsLen]++;
				%i++;
				if(%i >= %len) {
					break;
				}
			}
			%lsLen++;
		} else {
			%ls[%lsLen]++;
			%lsLen++;
		}
	}

	for(%i = 0; %i < %len; %i++) {
		if (getSubStr(%site, %i, 1) $= "[") {
			%i++;
			%c = getSubStr(%site, %i, 1);
			while (%c !$= "]") {
				%x = JMLib::siteDigit(%c);
				%ls[(%throwCount+%x) % %lsLen]--;

				%i++;
				if(%i >= %len) {
					break;
				}
				%c = getSubStr(%site, %i, 1);
			}
			%throwCount++;
		} else {
			%c = getSubStr(%site, %i, 1);
			%x = JMLib::siteDigit(%c);
			%ls[(%throwCount+%x) % %lsLen]--;
			%throwCount++;
		}
	}

	for(%i = 0; %i < %lsLen; %i++) {
		if (%ls[%i] != 0) {
			return 1;
		}
	}

	return 0;

}

// This is a damn cool trick for synchronous validation
// Reduce the syncchrnous one to a VSS or MSS, as follows:
// For each throw, if it's jsut a normal throw [not crossing], just put
//  it into the new modified temporary siteswap
// If it's a crossing throw from the left, add one to it
// If it's a crossing throw from the right, subtract one.
// Then validate as you would a normal MSS or VSS

function JMSiteValidator::transSyncMSS(%SSS) {
	%len = strlen(%SSS);
	%add = 1;

	%MSS = "";

	for (%i = 0; %i < %len; %i++) {
		%c = getSubStr(%SSS,%i,1);
    	if (%c $= "(" || %c $= ",") { // start of sync throw or second in pair
			if (%c $= "(") {
				%add = 1;
			} else if (%c $= ",") {
				%add = -1;
			}

			%i++; // skip "("
      
			%c = getSubStr(%SSS,%i,1);
			if (%c $= "[") { // multiplex in sync throw
				%MSS = %MSS @ "[";
				%i++; // skip "["

				%c = getSubStr(%SSS,%i,1);
				while (%c !$= "]") {
					%x = JMLib::siteDigit(%c);

					if (%x % 2 != 0) {
						return $invalidsitechar;
					}

					%c = getSubStr(%SSS,%i+1,1);
					if (%c $= "x") {
						%MSS = %MSS @ JMLib::siteChar(%x+%add);
						%i++; // skip "x"
					} else {
						%MSS = %MSS @ JMLib::siteChar(%x);
					}

					%i++; // skip to next

					if(%i >= %len) {
						break;
					}

					%c = getSubStr(%SSS,%i,1);
				}

				%MSS = %MSS @ "]";
			} else { // single sync throw
				%x = JMLib::siteDigit(%c);
				if (%x % 2 != 0) {
					return $invalidsitechar;
				}

				%c = getSubStr(%SSS,%i+1,1);
				if (%c $= "x") {
					%MSS = %MSS @ JMLib::siteChar(%x+%add);
					%i++; // skip "x"
				} else {
					%MSS = %MSS @ JMLib::siteChar(%x);
				}
			}
		}
	}
	return %MSS;
}


function JMSiteValidator::scanSite(%site) {
	%c = getSubStr(%site,0,1);

	if (%c $= "(") { // sync site
		while (strlen(%site) > 0 && %c $= "(") {
			%site = JMSiteValidator::scanSSS(%site);
			%c = getSubStr(%site,0,1);
			if (JMSiteValidator::validSiteChar(%c) != 0) {
				return 1;
			}
		}
		if (strlen(%site) > 0) { // rubbish after site end
			return 1;
		}
	} else {
		while (strlen(%site) > 0) {
			if (%c $= "[") {
				%site = JMSiteValidator::scanMSS(%site);
				%c = getSubStr(%site,0,1);
				if (JMSiteValidator::validSiteChar(%c) != 0) {
					return 1;
				}
				%site = getSubStr(%site,1,strlen(%site)-1);
			} else if (JMLib::siteDigit(%c) == 0 && %c !$= "0") {
				// only single site throws may appear
				return 1;
			} else {
				%site = getSubStr(%site,1,strlen(%site)-1);
			}
			%c = getSubStr(%site,0,1);
		}
	}
	return 0;
}

function JMSiteValidator::validateSiteSyntax(%site) {
	// Rely on Torque initialising values to zero
	for (%i=0; %i<strlen(%site); %i++) {
		%c = getSubStr(%site,%i,1);

		if(JMSiteValidator::validSiteChar(%c) != 0) {
			return 1;
		}
	}

	return JMSiteValidator::scanSite(%site);

}

 // Public Functions

function JMSiteValidator::validateSite(%site) {
	if (strlen(%site) == 0) {
		return 1;
	}

	if (JMSiteValidator::validateSiteSyntax(%site) == 0) { // SSS
		if (getSubStr(%site, 0, 1) $= "(") {
			return JMSiteValidator::validateSSS(%site);
		} else if (strstr(%site, "[") == -1) { // VSS
			return JMSiteValidator::validateVSS(%site); 
		} else { // MSS
			return JMSiteValidator::validateMSS(%site);
		}
		return 1;
	} else {
		return 1;
	}
}


function JMSiteValidator::test(%site) {
	if(JMSiteValidator::validateSite(%site) == 0) {
		%status="valid";
	} else {
		%status="invalid";
	}
	echo("Site \'" @ %site @ "\' is " @ %status);
}
