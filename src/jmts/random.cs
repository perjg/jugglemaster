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


function tertiary(%one, %two, %three) {
	if(%one) {
		return %two;
	} else {
		return %three;
	}
}

function JMRand::jm_randnoparam() {
	if(getRandom(0,1) > 0.5) {
		%sync=1;
	} else {
		%sync=0;
	}
	return JMRand::jm_rand(getRandom(1,12), getRandom(1,12), getRandom(1,12), %sync, 0);
}

function JMRand::jm_rand(%numballs, %pattlen, %transformations, %synchronous, $multiplex) {
	if(%synchronous) {
		return JMRand::jm_rand_sync(%numballs,
			mFloor((%pattlen + 1)/2), %transformations);
	} else {
		return JMRand::jm_rand_async(%numballs,
			%pattlen, %transformations);
	}
}


// For internal use only
function JMRand::jm_rand_async(%numballs, %pattlen, %transformations) {
	if(%numballs < 0 || %numballs > 35 || %pattlen == 0) {
	// This is a silly pattern. Let's not generate it
		return "";
	}

	for(%i = 0; %i < %pattlen; %i++) {
		%buf[%i]=%numballs;
	}

	%i = 0;
	%j = 0;

	while(%i < %transformations && %j < %transformations * 5) {
		%first = getRandom(0,%pattlen-1);
		%distance = getRandom(0,%pattlen-1);

		if((%buf[%first] - %distance >= 0) && (%buf[(%first+%distance) % %pattlen] + %distance <= 35)) {
			%tmp = %buf[%first] - %distance;
			%buf[%first] = %buf[(%first+%distance) % %pattlen] + %distance;
			%buf[(%first+%distance) % %pattlen] = %tmp;
			%i++;
		} else {
			%j++;
		}
	}

	%s = "";
	for(%i = 0; %i < %pattlen; %i++) {
		%s = %s @ JMLib::siteChar(%buf[%i]);
	}

	return %s;
}


function JMRand::jm_rand_sync(%numballs, %pattlen, %transformations) {
	if(%numballs < 0 || %numballs > 35 || %pattlen == 0) {
	// This is a silly pattern. Let's not generate it
		return NULL;
	}

    if(%numballs & 1 && %pattlen & 1) {
        %pattlen++;
    }

	for(%i = 0; %i < %pattlen; %i++) {
		if(0 == (%numballs & 1)) {
			%left[%i] = %numballs;
			%right[%i] = %numballs;
			%leftcross[%i] = 0;
			%rightcross[%i] = 0;
		} else {
			if(%i & 1) {
				%left[%i] = %numballs + 1;
				%right[%i] = %numballs - 1;
				%leftcross[%i] = 1;
				%rightcross[%i] = 0;
			} else {
				%left[%i] = %numballs - 1;
				%right[%i] = %numballs + 1;
				%leftcross[%i] = 0;
				%rightcross[%i] = 1;
			}
		}
	}

	%i = 0;
	%j = 0;

	while(%i < %transformations && %j < %transformations * 5) {
		// if(getRandom(0,6) == 1) {
		if(false) {
			%pos = getRandom(0,%pattlen-1);
			if(%leftcross[%pos] != 0 && %right[%pos] == 0) {
				%j++;
			} else if(%rightcross[%pos] != 0 && %left[%pos] == 0) {
				%j++;
			} else {
				%tmp = tertiary(%leftcross[%pos],0,1);
				%leftcross[%pos] = tertiary(%rightcross[%pos],0,1);
				%rightcross[%pos] = %tmp;

				%i++;
			}
		} else {
			%first = getRandom(0, %pattlen-1);
			%distance = getRandom(0, %pattlen-1);
			if(getRandom() > 0.5) {
				if((%left[%first] - %distance * 2 >= 0) &&
					(%left[(%first + %distance) % %pattlen] + %distance * 2 <= 35)) {

					if(%left[%first] - %distance * 2 == 0 && %leftcross[%first] != 0) {
						%j++;
					} else {
						%tmp = %left[%first] - %distance * 2;
						%left[%first] = %left[(%first + %distance) % %pattlen] + %distance * 2;
						%left[(%first + %distance) % %pattlen] = %tmp;

						%tmp = %leftcross[%first];
						%leftcross[%first] = %leftcross[(%first + %distance) % %pattlen];
						%leftcross[(%first + %distance) % %pattlen] = %tmp;
						%i++;
					}
				} else {
					%j++;
				}
			} else {
				if((%right[%first] - %distance * 2 >= 0) &&
					(%right[(%first + %distance) % %pattlen] + %distance * 2 <= 35)) {

					if(%right[%first] - %distance * 2 == 0 && %rightcross[%first] != 0) {
						%j++;
					} else {
						%tmp = %right[%first] - %distance * 2;
						%right[%first] = %right[(%first + %distance) % %pattlen] + %distance * 2;
						%right[(%first + %distance) % %pattlen] = %tmp;

						%tmp = %rightcross[%first];
						%rightcross[%first] = %rightcross[(%first + %distance) % %pattlen];
						%rightcross[(%first + %distance) % %pattlen] = %tmp;
						%i++;
					}
				} else {
					%j++;
				}
			}
		}
	}

	%s = "";
	%leftcrosses = 0;
	%rightcrosses = 0;

	for(%i = 0; %i < %pattlen; %i++) {
		%s = %s @ "(";

		%s = %s @ JMLib::siteChar(%left[%i]);
		%s = %s @ tertiary(%leftcross[%i], "x", "");
		%s = %s @ ",";
		%s = %s @ JMLib::siteChar(%right[%i]);
		%s = %s @ tertiary(%rightcross[%i], "x", "");

		if(%leftcross[%i]) {
			%leftcrosses++;
		}
		if(%rightcross[%i]) {
			%rightcrosses++;
		}

		%s = %s @ ")";
	}
	// echo("LeftCrosses: " @ %leftcrosses @ "    RightCrosses: " @ %rightcrosses);
	if(%leftcrosses != %rightcrosses) {
		// Ooops
		return;
	}
	return %s;
}
