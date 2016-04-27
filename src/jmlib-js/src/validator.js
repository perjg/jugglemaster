// 	$Id: validator.cpp 208 2006-08-04 01:40:24Z per $	 

/*
 * jmlib-js - Portable JuggleMaster Library (JavaScript Version)
 * Version 2.0
 * (C) Per Johan Groland 2006-2016
 *
 * Based on JMLib 2.0, (C) Per Johan Groland and Gary Briggs
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib_js under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */

/*
 * Some notes on the implementation:
 *
 * - The site validator is entirely independent from the rest of jmlib.
 * - Notation:
 *   VSS  = Vanilla Siteswap (e.g. 534)
 *   MSS  = Multiplexed Siteswap (e.g. 25[75]51)
 *   SSS  = Sync Siteswap (e.g. (4,2x)(2x,4) and ([6x6],2)(2,[6x6]) )
 *          (The last example is a sync multiplexed site.)
 *
 * - The implementation is done in two parts:
 *   * Validate syntax
 *   * Validate site
 *   This is not the most effective methods (that would be to validate syntax
 *   on the fly when validating the site). This method is however more flexible
 *   in that it allows the syntax to be validated separately.
 *
 */

function JMValidator() {}

JMValidator.pos = 0; // tracks the current position in the site

// convert a single siteswap digit (0-9 and a-z) to a number
JMValidator.siteDigit = function (s) {
	var str_0 = new String("0");
	var str_a = new String("a");
	var str_A = new String("A");

	if (s >= '0' && s <= '9')
		return s.charCodeAt(0) - str_0.charCodeAt(0);
	else if (s >= 'a' && s <= 'z')
		return s.charCodeAt(0) - str_a.charCodeAt(0) + 10;
	else if (s >= 'A' && s <= 'Z')
		return s.charCodeAt(0) - str_A.charCodeAt(0) + 10;
	else
		return -1;
}

// convert a siteswap number to character (0-9 and a-z)
JMValidator.siteChar = function (c) {
	var str_a = new String("a");

	if (c == "[" || c == "]")
		return c;
	else if (c >= 0 && c <= 9)
		return c;
	else
		return String.fromCharCode(c - 10 + str_a.charCodeAt(0));
}

// Scans a MSS for valid syntax
JMValidator.scanMSS = function (site) {
	pos++; // skip '['

	if (site.charAt(pos) == ']') // empty multiplex throw
		return false;

	while (site.charAt(pos) != ']') {
		if (pos == site.length) return false; // premature end of site

		if (JMValidator.siteDigit(site.charAt(pos)) == -1) // only single site throws may appear
			return false;

		pos++;
	}

	return true;
}

// Scans a SSS for valid syntax
JMValidator.scanSSS = function (site) {
	pos++; // skip '('

	// premature end of site
	if (pos == site.length) return false; // premature end of site

	if (site.charAt(pos) == '[') { // multiplex throw
		if (!JMValidator.scanMSS(site))
			return false;
	}
	else if (JMValidator.siteDigit(site.charAt(pos)) == -1) // single throw
		return false;

	pos++; // skip to ', or 'x'

	// skip x if present
	if (site.charAt(pos) == 'x') {
		pos++;
	}

	if (site.charAt(pos) != ',') // ',' expected
		return false;

	pos++;
	if (pos == site.length) return false;

	if (site.charAt(pos) == '[') { // multiplex throw
		if (!JMValidator.scanMSS(site)) return false;
	}
	else if (JMValidator.siteDigit(site.charAt(pos)) == -1) // single throw
		return false;

	pos++; // skip to ')' or x

	// skip x if present
	if (site.charAt(pos) == 'x') {
		pos++;
	}

	if (site.charAt(pos) != ')') // ')' expected
		return false;

	pos++;

	return true;
}

// Scans a site for valid syntax
JMValidator.scanSite = function (site) {
	if (site.charAt(pos) == '(') { // sync site
		while (site.charAt(pos) == '(') {
			if (!JMValidator.scanSSS(site))
				return false;
		}
		if (pos < site.length - 1)
			return false;
	}
	else {
		while (pos < site.length) {
			if (site.charAt(pos) == '[') {
				if (!JMValidator.scanMSS(site))
					return false;
				pos++;
			}
			else if (JMValidator.siteDigit(site.charAt(pos)) == -1) { // only single site throws may appear
				return false;
			}
			else {
				pos++;
			}
		}
	}
	return true;
}

JMValidator.validateSiteSyntax = function (site) {
	pos = 0;

	// Check for valid characters
	for (var i = 0; i < site.length; i++) {
		if (site.charAt(i) != '[' && site.charAt(i) != ']' && site.charAt(i) != '(' &&
			site.charAt(i) != ')' && site.charAt(i) != ',') {
			if (JMValidator.siteDigit(site.charAt(i)) == -1)
				return false;
		}
	}

	return JMValidator.scanSite(site);
}

JMValidator.validateSite = function (site) {
	if (site == null)
		return false;

	if (site.length == 0)
		return false;

	if (JMValidator.validateSiteSyntax(site)) {
		pos = 0;
		if (site.charAt(0) == '(') // SSS
			return JMValidator.validateSSS(site);
		else // VSS or MSS
			return JMValidator.validateMSS(site);
	}
	//document.write("invalid syntax for site '" + site + "'<br>");
	return false;
}

/*
 * Let VSS be the siteswap to validate.
 * Let LS be a sequence of 1's the length of VSS
 *
 * Algorithm:
 *
 * for (i = 0; i <= len(VSS)-1; i++)
 *   x = VSS(i);
 *   LS((i+x) mod len(VSS))--;
 *
 * VSS is valid iff LS is a sequence of zeros
 *
 * Note: The method validateMSS validates VSSs as well.
 */
JMValidator.validateVSS = function (site) {
	var i, x;
	var len = site.length;

	var ls = [];
	for (var i = 0; i < len; i++)
		ls[i] = 1;

	for (var i = 0; i < len; i++) {
		x = JMValidator.siteDigit(site.charAt(i));
		ls[(i + x) % len]--;
	}

	for (var i = 0; i < len; i++) {
		if (ls[i] != 0) {
			return false;
		}
	}

	return true;
}

JMValidator.validateSSS = function (site) {
	var MSS = JMValidator.transSyncMSS(site);

	//document.write("<b>SSS</b> '" + site + "' translates to MSS '" + MSS + "'<br>");

	if (MSS) {
		return JMValidator.validateMSS(MSS);
	}
	else
		return false;
}

/*
 * The throw capacity LS of a MSS is the amount of balls multiplexed for each throw.
 * e.g. for [54]24 the throw capacity is 211.
 *
 * A MSS can be validated by calculating the throw capacity and then verifying it using
 * the same algorithm as used for a VSS with the exception that the modulo must be calculated
 * from the throw count of the pattern (since this will be different from the length).
 *
 * This implementation can also be used to validate a VSS.
 *
 */
JMValidator.validateMSS = function (site) {
	var i, x;
	var len = site.length;
	var ls = [];
	var lsLen = 0;
	var throwCount = 0;

	for (var i = 0; i < len; i++)
		ls[i] = 0;

	for (var i = 0; i < len; i++) {
		if (site.charAt(i) == '[') {
			while (site.charAt(++i) != ']')
				ls[lsLen]++;
			lsLen++;
		}
		else
			ls[lsLen++]++;
	}

	//document.write("BEFORE validateMSS(" + site + "): <b>");
	//document.write(ls);
	//document.write("</b><br>");

	for (var i = 0; i < len; i++) {
		if (site.charAt(i) == '[') {
			while (site.charAt(++i) != ']') {
				x = JMValidator.siteDigit(site.charAt(i));
				ls[(throwCount + x) % lsLen]--;
			}
			throwCount++;
		}
		else {
			x = JMValidator.siteDigit(site.charAt(i));
			//document.write("siteDigit = " + x + "<br>");
			ls[(throwCount + x) % lsLen]--;
			throwCount++;
		}
	}

	//document.write("AFTER validateMSS(" + site + "): <b>");
	//document.write(ls);
	//document.write("</b><br>");

	for (var i = 0; i < lsLen; i++) {
		if (ls[i] != 0) {
			return false;
		}
	}

	return true;
}

JMValidator.transSyncMSS = function (SSS) {
	var i, x;
	var len = SSS.length;
	var add = 1;
	var mssPos = 0;

	var MSS = [];

	for (var i = 0; i < len; i++) {
		if (SSS.charAt(i) == '(' || SSS.charAt(i) == ',') { // start of sync throw or second in pair
			if (SSS.charAt(i) == '(') add = 1;
			else if (SSS.charAt(i) == ',') add = -1;

			i++; // skip '('

			if (SSS.charAt(i) == '[') { // multiplex in sync throw
				MSS[mssPos++] = '[';
				i++; // skip '['

				while (SSS.charAt(i) != ']') {
					x = JMValidator.siteDigit(SSS.charAt(i));
					if (x % 2 != 0) return null;

					if (SSS[i + 1] == 'x') {
						MSS[mssPos++] = x + add;
						i++; // skip 'x'
					}
					else
						MSS[mssPos++] = x;

					i++; // skip to next
				}

				MSS[mssPos++] = ']';
			}
			else { // single sync throw
				x = JMValidator.siteDigit(SSS.charAt(i));

				if (x % 2 != 0) return null;

				if (SSS.charAt(i + 1) == 'x') {
					MSS[mssPos++] = x + add;
					i++; // skip 'x'
				}
				else {
					MSS[mssPos++] = x;
				}
			}
		}
	}

	var MSS_str = new String();
	var cur;
	var str_a = new String("a");

	for (var i = 0; i < mssPos; i++) {
		//document.write("Concatenating '" + MSS[i] + " with value '" + siteChar(MSS[i]) + "''<br>");
		MSS_str = MSS_str.concat(JMValidator.siteChar(MSS[i]));
	}

	//document.write("MSS: '");
	//document.write(MSS_str);
	//document.write("'<br>");

	return MSS_str;
}
