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


function JMSiteValidator::blankettest() {
	JMSiteValidator::test("633");
	JMSiteValidator::test("4[45]3");
	JMSiteValidator::test("2[]3");
	JMSiteValidator::test("2[4]3");
	JMSiteValidator::test("2[345");
	JMSiteValidator::test("2]345");
	JMSiteValidator::test("(2x,4)(4x,2)");
	JMSiteValidator::test("(2x,4)(4x,2)7");
	JMSiteValidator::test("8(2x,4)(4x,2)");
	JMSiteValidator::test("(2x,4)6(4x,2)");
	JMSiteValidator::test("(2x(4),(4x,2)");
	JMSiteValidator::test("([2x4],4)([24x],2)");
	JMSiteValidator::test("([2x4]4,4)([24x],2)");
	JMSiteValidator::test("([2x4,4)([24x],2)");
	JMSiteValidator::test("44453");
	JMSiteValidator::test("6337");
	JMSiteValidator::test("b");
	JMSiteValidator::test("b1");
	JMSiteValidator::test("c1");
	JMSiteValidator::test("[54]24");
	JMSiteValidator::test("(6x,4)(4,6x)");
	JMSiteValidator::test("(7,4)(4,5)");
	JMSiteValidator::test("(4,2x)(2x,4)");
	JMSiteValidator::test("(7x,2)(2,5x)");
	JMSiteValidator::test("spam spam spam!\"#!#¤#\"#¤%");
	JMSiteValidator::test("");
	JMSiteValidator::test("[543]3[45][27]2");
	JMSiteValidator::test("(4,4)(0,4x)(4,4)(4x,0)");
	JMSiteValidator::test("([yxy],2)(2,[22])(2,[yxy])([22],2)");
}
