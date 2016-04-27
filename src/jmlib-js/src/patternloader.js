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

const patternLibrary = require('./patterns');

function PatternLoader() {}

PatternLoader.getGroups = function () {
	return patternLibrary.groups;
};

PatternLoader.getPatterns = function () {
	return patternLibrary.patterns;
};

PatternLoader.getStyles = function () {
	return patternLibrary.styles;
};

PatternLoader.getPattern = function (index) {
	return patternLibrary.patterns[index];
};

PatternLoader.getStyle = function (name) {
	return patternLibrary.styles[name];
};

// load style by name
PatternLoader.loadStyle = function (jmlib, name) {
	const style = patternLibrary.styles[name];

	if (style === null) {
		return false;
	}

	const length = style[0],
		data = style[1];

	return jmlib.setStyle(name, length, data);
};

// Load pattern by index
PatternLoader.loadPatternEx = function (jmlib, index) {
	const pattern = patternLibrary.patterns[index];

	if (pattern === null) {
		return false;
	}

	const name = pattern[0],
		site = pattern[1],
		style = pattern[2],
		hr = pattern[3],
		dr = pattern[4];

	if (jmlib.setPattern(name, site, hr, dr)) {
		return PatternLoader.loadStyle(jmlib, style);
	}
	return false;
};

// Load pattern by name
PatternLoader.loadPattern = function (jmlib, name) {
	for (let i = 0; i < patternLibrary.patterns.length; i++) {
		let pattern = patternLibrary.patterns[i];

		if (pattern === null) {
			return false;
		}

		if (pattern[0] === name)
			return PatternLoader.loadPatternEx(jmlib, i);
	}

	return false;
};

module.exports = PatternLoader;
