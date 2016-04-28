"use strict";

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

var PI = 3.14159265;
var HALF_PI = 1.570796325;
var OBJECT_UNDER = 0x02;

// Ball class
function Ball() {
	this.bh = 0; // throw height
	this.gx = 0; // x position
	this.gy = 0; // y position
	this.gx0 = 0; // previous position
	this.gy0 = 0; // ???
	this.gx1 = 0; // ???
	this.gy1 = 0; // ???
	this.c = 0; // ???
	this.c0 = 0; // ???
	this.chand = 0; // catching hand (0-left, 1-right)
	this.thand = 0; // throwing hand (0-left, 1-right)
	this.st = 0; // object status (OBJECT_HAND etc.)
	this.t = 0; // for spin calculation
}

Ball.prototype.getSpin = function (spins) {
	if (this.isHolding()) {
		return HALF_PI;
	} else {
		return (HALF_PI + this.t * PI) * ((spins * this.bh - 1) / 2);
	}
};

Ball.prototype.isHolding = function () {
	return this.st & OBJECT_UNDER;
};

Ball.prototype.toString = function () {
	return "gx=" + this.gx + ",gy=" + this.gy;
};

module.exports = Ball;