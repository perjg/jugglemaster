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


// TorqueScript Hurts My Face
$SiteDigitLookup["0"]=0;
$SiteDigitLookup["1"]=1;
$SiteDigitLookup["2"]=2;
$SiteDigitLookup["3"]=3;
$SiteDigitLookup["4"]=4;
$SiteDigitLookup["5"]=5;
$SiteDigitLookup["6"]=6;
$SiteDigitLookup["7"]=7;
$SiteDigitLookup["8"]=8;
$SiteDigitLookup["9"]=9;
$SiteDigitLookup["a"]=10;
$SiteDigitLookup["b"]=11;
$SiteDigitLookup["c"]=12;
$SiteDigitLookup["d"]=13;
$SiteDigitLookup["e"]=14;
$SiteDigitLookup["f"]=15;
$SiteDigitLookup["g"]=16;
$SiteDigitLookup["h"]=17;
$SiteDigitLookup["i"]=18;
$SiteDigitLookup["j"]=19;
$SiteDigitLookup["k"]=20;
$SiteDigitLookup["l"]=21;
$SiteDigitLookup["m"]=22;
$SiteDigitLookup["n"]=23;
$SiteDigitLookup["o"]=24;
$SiteDigitLookup["p"]=25;
$SiteDigitLookup["q"]=26;
$SiteDigitLookup["r"]=27;
$SiteDigitLookup["s"]=28;
$SiteDigitLookup["t"]=29;
$SiteDigitLookup["u"]=30;
$SiteDigitLookup["v"]=31;
$SiteDigitLookup["w"]=32;
$SiteDigitLookup["x"]=33;
$SiteDigitLookup["y"]=34;
$SiteDigitLookup["z"]=35;
$SiteDigitLookup["A"]=10;
$SiteDigitLookup["B"]=11;
$SiteDigitLookup["C"]=12;
$SiteDigitLookup["D"]=13;
$SiteDigitLookup["E"]=14;
$SiteDigitLookup["F"]=15;
$SiteDigitLookup["G"]=16;
$SiteDigitLookup["H"]=17;
$SiteDigitLookup["I"]=18;
$SiteDigitLookup["J"]=19;
$SiteDigitLookup["K"]=20;
$SiteDigitLookup["L"]=21;
$SiteDigitLookup["M"]=22;
$SiteDigitLookup["N"]=23;
$SiteDigitLookup["O"]=24;
$SiteDigitLookup["P"]=25;
$SiteDigitLookup["Q"]=26;
$SiteDigitLookup["R"]=27;
$SiteDigitLookup["S"]=28;
$SiteDigitLookup["T"]=29;
$SiteDigitLookup["U"]=30;
$SiteDigitLookup["V"]=31;
$SiteDigitLookup["W"]=32;
$SiteDigitLookup["X"]=33;
$SiteDigitLookup["Y"]=34;
$SiteDigitLookup["Z"]=35;


$ReverseDigitLookup[0]="0";
$ReverseDigitLookup[1]="1";
$ReverseDigitLookup[2]="2";
$ReverseDigitLookup[3]="3";
$ReverseDigitLookup[4]="4";
$ReverseDigitLookup[5]="5";
$ReverseDigitLookup[6]="6";
$ReverseDigitLookup[7]="7";
$ReverseDigitLookup[8]="8";
$ReverseDigitLookup[9]="9";
$ReverseDigitLookup[10]="a";
$ReverseDigitLookup[11]="b";
$ReverseDigitLookup[12]="c";
$ReverseDigitLookup[13]="d";
$ReverseDigitLookup[14]="e";
$ReverseDigitLookup[15]="f";
$ReverseDigitLookup[16]="g";
$ReverseDigitLookup[17]="h";
$ReverseDigitLookup[18]="i";
$ReverseDigitLookup[19]="j";
$ReverseDigitLookup[20]="k";
$ReverseDigitLookup[21]="l";
$ReverseDigitLookup[22]="m";
$ReverseDigitLookup[23]="n";
$ReverseDigitLookup[24]="o";
$ReverseDigitLookup[25]="p";
$ReverseDigitLookup[26]="q";
$ReverseDigitLookup[27]="r";
$ReverseDigitLookup[28]="s";
$ReverseDigitLookup[29]="t";
$ReverseDigitLookup[30]="u";
$ReverseDigitLookup[31]="v";
$ReverseDigitLookup[32]="w";
$ReverseDigitLookup[33]="x";
$ReverseDigitLookup[34]="y";
$ReverseDigitLookup[35]="z";

function JMLib::siteDigit(%s) {
	return $SiteDigitLookup[%s];
}

function JMLib::siteChar(%s) {
	return $ReverseDigitLookup[%s];
}

