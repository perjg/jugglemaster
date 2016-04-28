'use strict';

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

/* eslint-disable no-var, eqeqeq, no-constant-condition */
var Ball = require('./ball'),
    Arm = require('./arm'),
    Hand = require('./hand'),
    validator = require('./validator');

// JMLib class
function JMLib(errorCallback) {
	var i;

	this.errorCallback = errorCallback;

	// public variables
	this.ap = new Arm();
	this.rhand = new Ball();
	this.lhand = new Ball();
	this.handpoly = new Hand();
	this.handpoly_ex = new Hand();

	this.b = new Array(JMLib.BMAX);
	for (i = 0; i < JMLib.BMAX; i++) {
		this.b[i] = new Ball();
	}

	this.dpm = 0;
	this.gx_max = 0;
	this.gx_min = 0;
	this.gy_max = 0;
	this.gy_min = 0;
	this.imageWidth = 0;
	this.imageHeight = 0;
	this.status = 0;

	// protected variables
	this.balln = 0;
	this.bm1 = 0;
	this.arm_x = 0;
	this.arm_y = 0;
	this.hand_x = 0;
	this.hand_y = 0;
	this.horCenter = 0;
	this.verCenter = 0;
	this.styledata = new Array(JMLib.STYLEMAX * 4);
	this.style_len = 0;
	this.ga = 0.0;
	this.dwell_ratio = 0.0;
	this.height_ratio = 0.0;
	this.base = 0;
	this.mirror = 0;
	this.time_count = 0;
	this.time_period = 0;
	this.cSpeed = 0.0;
	this.beep = 0;
	this.syn = 0;
	this.hand_on = 0;
	this.fpu = true;
	this.tw = 0;
	this.aw = 0;
	this.pmax = 0;
	this.lastError = null;

	//JML_INT32 patt[LMAX][MMAX];
	this.patt = new Array(JMLib.LMAX);
	for (i = 0; i < JMLib.LMAX; i++) {
		this.patt[i] = new Array(JMLib.MMAX);
		for (var j = 0; j < JMLib.MMAX; j++) {
			this.patt[i][j] = 0;
		}
	}

	this.patts = new Array(JMLib.LMAX);
	this.pattw = 0;
	this.r = new Array(JMLib.LMAX * 2);
	this.smode = 0;
	this.high0 = new Array(JMLib.BMAX + 1);
	this.high = new Array(JMLib.BMAX + 1);
	this.kw0 = 0; // XR/KW [m]

	this.siteswap = ""; // The current siteswap
	this.pattname = ""; // The name of the current pattern
	this.steps = new Array(JMLib.LMAX); // used to print the site on screen
	this.stylename = ""; // The name of the current style

	// Scaling
	this.scalingMethod = 0;
	this.scaleImageWidth = 0;
	this.scaleImageHeight = 0;

	this.possible_styles = new Array();
	this.possible_styles[0] = "Normal";
	this.possible_styles[1] = "Reverse";
	this.possible_styles[2] = "Shower";
	this.possible_styles[3] = "Mills Mess";
	this.possible_styles[4] = "Center";
	this.possible_styles[5] = "Windmill";
	//this.possible_styles[6] = "Random";

	//this.self = this;

	this.initialize();
}

// Misc. utility functions
JMLib.max = function (a, b) {
	if (a > b) return a;
	return b;
};

JMLib.min = function (a, b) {
	if (a < b) return a;
	return b;
};

JMLib.jijo = function (x) {
	return x * x;
};

// Calculates |x|
JMLib.xabs = function (x) {
	if (x < 0) return -x;
	return x;
};

// Calculates x^y
JMLib.xpow = function (x, y) {
	for (var i = 0; i < y - 1; i++) {
		x *= y;
		//document.write("i=" + i + " x=" + x + "<br>");
	}
	return x;
};

JMLib.random = function (x) {
	return Math.floor(x * Math.random() % 1);
};

// For status
JMLib.ST_NONE = 0;
JMLib.ST_PAUSE = 1;
JMLib.ST_JUGGLE = 2;

// Misc. constants
JMLib.KW = 0.25;
JMLib.DW = 290; // max of dpm

/* low mem profile
 JMLib.XR = 255;   // Accuracy of x axis, set higher for large screens 255
 JMLib.BMAX = 35;  // max number of balls
 JMLib.MMAX = 11;  // Max multiplex, double+1
 */

// high mem profile
JMLib.XR = 1024; // Accuracy of x axis, set higher for large screens 255
JMLib.BMAX = 630; // max number of balls
JMLib.MMAX = 71; // Max multiplex, double+1
//

JMLib.LMAX = 76; // max column of a pattern

JMLib.OBJECT_HAND = 0x01;
JMLib.OBJECT_UNDER = 0x02;
JMLib.OBJECT_MOVE = 0x04;
JMLib.OBJECT_MOVE2 = 0x08;
JMLib.STYLEMAX = 3000; // maximum length of style data

JMLib.JML_MAX_SECTIONLEN = 40;
JMLib.JML_MAX_NAMELEN = 56;
JMLib.JML_MAX_SITELEN = 56;
JMLib.JML_MAX_STYLELEN = 56;
// high mem profile
//JMLib.JML_MAX_STYLELEN = 500;
JMLib.SPEED_MAX = 2.0;
JMLib.SPEED_MIN = 0.1;
JMLib.SPEED_DEF = 1.0;
JMLib.HR_MAX = 1.00;
JMLib.HR_MIN = 0.04;
JMLib.HR_DEF = 0.17;
JMLib.DR_MAX = 0.90;
JMLib.DR_MIN = 0.10;
JMLib.DR_DEF = 0.50;
JMLib.FS_MAX = 10;
JMLib.FS_MIN = 1;
JMLib.FS_DEF = 1;

// Scaling methods
JMLib.SCALING_METHOD_CLASSIC = 1;
JMLib.SCALING_METHOD_DYNAMIC = 2;

JMLib.prototype.getSiteposStart = function () {
	if (this.syn && this.time_period % 2 == 1) {
		return this.steps[this.time_period - 1];
	}
	return this.steps[this.time_period];
};

JMLib.prototype.getSiteposStop = function () {
	if (this.syn && this.time_period % 2 == 0) {
		return this.steps[this.time_period + 2];
	}
	return this.steps[this.time_period + 1];
};

JMLib.prototype.getSiteposLen = function () {
	return this.getSiteposStop() - this.getSiteposStart();
};

JMLib.prototype.getiterations = function () {
	return this.dpm;
};

JMLib.prototype.initialize = function () {
	// Set default values
	this.ga = 9.8;
	this.dwell_ratio = 0.5;
	this.height_ratio = 0.20;
	this.mirror = 0;
	this.cSpeed = JMLib.SPEED_DEF;
	this.syn = 0;
	this.hand_on = 1;
	this.hand_x = 0;
	this.hand_y = 0;
	this.scalingMethod = JMLib.SCALING_METHOD_CLASSIC;
	this.smode = 50.0;

	this.status = JMLib.ST_NONE;

	//fixme: add random number seed here

	this.setWindowSize(480, 400);
	this.setPatternDefault();
};

JMLib.prototype.shutdown = function () {};

JMLib.prototype.error = function (msg) {
	this.lastError = msg;

	if (this.errorCallback) {
		this.errorCallback(msg);
	}
};

JMLib.prototype.setWindowSizeDefault = function () {
	this.setWindowSize(480, 400);
};

JMLib.prototype.setWindowSize = function (width, height) {
	if (width <= 0 || height <= 0) return false;

	if (this.scalingMethod == JMLib.SCALING_METHOD_DYNAMIC) {
		this.scaleImageWidth = width;
		this.scaleImageHeight = height;
		return true;
	}

	// store current status and stop juggling
	var oldStatus = this.status;
	this.stopJuggle();

	// set size
	this.imageWidth = width;
	this.imageHeight = height;
	this.horCenter = parseInt(this.imageWidth / 2);
	this.verCenter = parseInt(this.imageHeight / 2);

	// recalculate pattern
	if (oldStatus != JMLib.ST_NONE) this.startJuggle();

	// restore state
	this.status = oldStatus;

	return true;
};

JMLib.prototype.setScalingMethod = function (scalingMethod) {
	// no change
	if (this.scalingMethod == scalingMethod) return;

	if (scalingMethod == JMLib.SCALING_METHOD_DYNAMIC) {
		this.scaleImageWidth = this.imageWidth;
		this.scaleImageHeight = this.imageHeight;
		this.setWindowSizeDefault();
		this.scalingMethod = JMLib.SCALING_METHOD_DYNAMIC;
	} else {
		//SCALING_METHOD_CLASSIC
		this.calingMethod = JMLib.SCALING_METHOD_CLASSIC;
		this.setWindowSize(this.scaleImageWidth, this.scaleImageHeight);
	}
};

JMLib.prototype.SCALING_METHOD_CLASSIC = JMLib.SCALING_METHOD_CLASSIC;
JMLib.prototype.SCALING_METHOD_DYNAMIC = JMLib.SCALING_METHOD_DYNAMIC;

JMLib.prototype.getImageWidth = function () {
	if (this.scalingMethod == JMLib.SCALING_METHOD_CLASSIC) return this.imageWidth;else // SCALING_METHOD_DYNAMIC
		return this.scaleImageWidth;
};

JMLib.prototype.getImageHeight = function () {
	if (this.scalingMethod == JMLib.SCALING_METHOD_CLASSIC) return this.imageHeight;else // SCALING_METHOD_DYNAMIC
		return this.scaleImageHeight;
};

JMLib.prototype.getBallRadius = function () {
	var baseRadius = parseInt(11 * this.dpm / JMLib.DW);

	if (this.scalingMethod == JMLib.SCALING_METHOD_CLASSIC) {
		return baseRadius;
	} else {
		var zoomFactorY = this.scaleImageHeight / this.imageHeight;
		zoomFactorY *= 0.9;

		if (this.scaleImageWidth < this.scaleImageHeight) zoomFactorY *= this.scaleImageWidth / this.scaleImageHeight;

		return parseInt(baseRadius * zoomFactorY); // convert return value to int
	}
};

JMLib.prototype.doCoordTransform = function () {
	var i;
	var zoomFactorX = this.scaleImageWidth / this.imageWidth;
	var zoomFactorY = this.scaleImageHeight / this.imageHeight;

	// adjust for aspect ratio
	if (this.scaleImageWidth < this.scaleImageHeight) zoomFactorY *= this.scaleImageWidth / this.scaleImageHeight;else zoomFactorX *= this.scaleImageHeight / this.scaleImageWidth;

	zoomFactorX *= 1.1;
	zoomFactorY *= 0.9;

	// Adjust coordinates
	// head
	this.ap.hx -= parseInt(this.imageWidth / 2);
	this.ap.hy -= parseInt(this.imageHeight / 2);

	this.ap.hx = parseInt(this.ap.hx * zoomFactorX);
	this.ap.hy = parseInt(this.ap.hy * zoomFactorY);
	this.ap.hr = parseInt(this.ap.hr * zoomFactorY);

	this.ap.hx += parseInt(this.scaleImageWidth / 2);
	this.ap.hy += parseInt(this.scaleImageHeight / 2);

	// juggler
	for (i = 0; i < 6; i++) {
		this.ap.rx[i] -= parseInt(this.imageWidth / 2);
		this.ap.ry[i] -= parseInt(this.imageHeight / 2);
		this.ap.rx[i] = parseInt(this.ap.rx[i] * zoomFactorX);
		this.ap.ry[i] = parseInt(this.ap.ry[i] * zoomFactorY);
		this.ap.rx[i] += parseInt(this.scaleImageWidth / 2);
		this.ap.ry[i] += parseInt(this.scaleImageHeight / 2);

		this.ap.lx[i] -= parseInt(this.imageWidth / 2);
		this.ap.ly[i] -= parseInt(this.imageHeight / 2);
		this.ap.lx[i] = parseInt(this.ap.lx[i] * zoomFactorX);
		this.ap.ly[i] = parseInt(this.ap.ly[i] * zoomFactorY);
		this.ap.lx[i] += parseInt(this.scaleImageWidth / 2);
		this.ap.ly[i] += parseInt(this.scaleImageHeight / 2);
	}

	// hands
	this.rhand.gx -= parseInt(this.imageWidth / 2);
	this.rhand.gy -= parseInt(this.imageHeight / 2);
	this.rhand.gx = parseInt(this.rhand.gx * zoomFactorX);
	this.rhand.gy = parseInt(this.rhand.gy * zoomFactorY);
	this.rhand.gx += parseInt(this.scaleImageWidth / 2);
	this.rhand.gy += parseInt(this.scaleImageHeight / 2);

	this.lhand.gx -= parseInt(this.imageWidth / 2);
	this.lhand.gy -= parseInt(this.imageHeight / 2);
	this.lhand.gx = parseInt(this.lhand.gx * zoomFactorX);
	this.lhand.gy = parseInt(this.lhand.gy * zoomFactorY);
	this.lhand.gx += parseInt(this.scaleImageWidth / 2);
	this.lhand.gy += parseInt(this.scaleImageHeight / 2);

	for (i = 0; i <= 9; i++) {
		this.handpoly.rx[i] = parseInt(this.handpoly_ex.rx[i] * zoomFactorX);
		this.handpoly.ry[i] = parseInt(this.handpoly_ex.ry[i] * zoomFactorY);
		this.handpoly.lx[i] = parseInt(this.handpoly_ex.lx[i] * zoomFactorX);
		this.handpoly.ly[i] = parseInt(this.handpoly_ex.ly[i] * zoomFactorY);
	}

	// balls
	for (i = this.numBalls() - 1; i >= 0; i--) {
		this.b[i].gx -= parseInt(this.imageWidth / 2);
		this.b[i].gy -= parseInt(this.imageHeight / 2);

		this.b[i].gx = parseInt(this.b[i].gx * zoomFactorX);
		this.b[i].gy = parseInt(this.b[i].gy * zoomFactorY);

		this.b[i].gx += parseInt(this.scaleImageWidth / 2);
		this.b[i].gy += parseInt(this.scaleImageHeight / 2);
	}
};

JMLib.prototype.setMirror = function (mir) {
	// store current status and stop juggling
	var oldStatus = this.status;
	this.stopJuggle();

	// set mirror
	this.mirror = mir;

	// recalculate pattern
	if (oldStatus != JMLib.ST_NONE) JMLib.prototype.startJuggle();

	// restore state
	this.status = oldStatus;
};

JMLib.prototype.setPattern = function (name, site, hr, dr) {
	if (site.length > JMLib.JML_MAX_SITELEN) {
		this.error("Siteswap too long");
		return false;
	}

	if (name.length > JMLib.JML_MAX_NAMELEN) {
		this.error("Pattern name too long");
		return false;
	}

	if (!validator.validateSite(site)) {
		this.error("Invalid siteswap");
		return false;
	}

	this.siteswap = site;
	this.pattname = name;
	this.height_ratio = hr;
	this.dwell_ratio = dr;

	// Turn off beep
	this.beep = 0;

	// Check ratios
	if (this.height_ratio < JMLib.HR_MIN || this.height_ratio > JMLib.HR_MAX) this.height_ratio = JMLib.HR_DEF;
	if (this.dwell_ratio < JMLib.DR_MIN || this.dwell_ratio > JMLib.DR_MAX) this.dwell_ratio = JMLib.DR_DEF;

	// Set pattern
	if ((this.jml_errno = this.set_patt(site)) == 0) {
		this.doStepcalc();
		return true;
	} else {
		switch (this.jml_errno) {
			case 4:
				this.error("Syntax error");
				break;
			case 5:
				this.error("Invalid pattern");
				break;
			case 6:
				this.error("Invalid character in pattern");
				break;
			case 7:
				this.error("Synchronous number must be even");
				break;
			case 8:
				this.error("Max 6 balls may be multiplexed");
				break;
			case 9:
				this.error("Too many balls in pattern");
				break;
			case 10:
				this.error("Pattern too long");
				break;
			case 13:
				this.error("0 inside [] is invalid");
				break;
			default:
				this.error("Unexpected error");
				break;
		}

		return false;
	}
};

JMLib.prototype.setStyle = function (name, length, data) {
	if (data.length > JMLib.JML_MAX_STYLELEN) {
		this.error("Style too large");
		return false;
	}

	if (name.length > JMLib.JML_MAX_NAMELEN) {
		this.error("Style name too long");
		return false;
	}

	this.stylename = name;
	this.styledata = data;
	this.style_len = length;

	return true;
};

//fixme: style data is passed as an array
//       should perhaps add a style data class
JMLib.prototype.setStyleEx = function (name) {
	var style;

	if (name == "Normal") {
		style = new Array(13, 0, 4, 0);
		this.setStyle(name, 1, style);
	} else if (name == "Reverse") {
		style = new Array(4, 0, 13, 0);
		this.setStyle(name, 1, style);
	} else if (name == "Shower") {
		style = new Array(5, 0, 10, 0, 10, 0, 5, 0);
		this.setStyle(name, 2, style);
	} else if (name == "Mills Mess") {
		style = new Array(-1, 0, -12, 0, 0, 0, 12, 0, 1, 0, -12, 0);
		this.setStyle(name, 3, style);
	} else if (name == "Center") {
		style = new Array(13, 0, 0, 0);
		this.setStyle(name, 1, style);
	} else if (name == "Windmill") {
		style = new Array(10, 0, -8, 0, -8, 0, 10, 0);
		this.setStyle(name, 2, style);
	}
	// placeholder for adding random style support here
	else {
			// anything else is interpreted as "Normal"
			this.setStyleDefault();
		}

	return true;
};

JMLib.prototype.getStyles = function () {
	return this.possible_styles;
};

JMLib.prototype.numStyles = function () {
	return this.possible_styles.length;
};

JMLib.prototype.setPatternDefault = function () {
	this.setPattern("3 Cascade", "3", JMLib.HR_DEF, JMLib.DR_DEF);
	this.setStyleDefault();
};

JMLib.prototype.setStyleDefault = function () {
	var defStyle = new Array(13, 0, 4, 0);
	//this.setStyle("Normal", 1, defStyle);
	this.stylename = "Normal";
	this.styledata = defStyle;
	this.style_len = 1;
};

JMLib.prototype.setHR = function (HR) {
	if (HR > JMLib.HR_MAX) {
		this.height_ratio = JMLib.HR_MAX;
	} else if (HR < JMLib.HR_MIN) {
		this.height_ratio = JMLib.HR_MIN;
	} else {
		this.height_ratio = HR;
	}
};

JMLib.prototype.getHR = function () {
	return this.height_ratio;
};

JMLib.prototype.setDR = function (DR) {
	if (DR > JMLib.DR_MAX) {
		this.dwell_ratio = JMLib.DR_MAX;
	} else if (DR < JMLib.DR_MIN) {
		this.dwell_ratio = JMLib.DR_MIN;
	} else {
		this.dwell_ratio = DR;
	}
};

JMLib.prototype.getDR = function () {
	return this.dwell_ratio;
};

JMLib.prototype.numBalls = function () {
	return this.balln;
};

// Internal functions
JMLib.prototype.arm_line = function () {
	var mx, my, sx, sy;

	if (this.hand_on) {
		// only bother calculating if hands are drawn
		// Notes:
		//  * gx/gy may need to be replaced by gx0/gy0 if erasing old values
		//    the method used in the X11 version
		//  * JMWin uses 11*dpm/DW instead of 11, which causes incorrect
		//    hand placement for some reason.
		this.ap.rx[0] = this.rhand.gx + 11 + this.arm_x;
		this.ap.ry[0] = this.rhand.gy + 11 + this.arm_y;
		this.ap.lx[0] = this.lhand.gx + 11 - this.arm_x;
		this.ap.ly[0] = this.lhand.gy + 11 + this.arm_y;

		sx = parseInt(this.dpm * JMLib.XR / this.kw0);
		sy = this.base - parseInt(this.dpm / 3);

		this.ap.rx[1] = parseInt((this.ap.rx[0] + (this.horCenter + sx) * 2) / 3 + this.dpm / 12);
		this.ap.lx[1] = parseInt((this.ap.lx[0] + (this.horCenter - sx) * 2) / 3 - this.dpm / 12);
		this.ap.ry[1] = parseInt((this.ap.ry[0] + sy) / 2 + this.dpm / 8);
		this.ap.ly[1] = parseInt((this.ap.ly[0] + sy) / 2 + this.dpm / 8);

		this.ap.rx[2] = parseInt((this.ap.rx[1] + (this.horCenter + sx) * 3) / 4);
		this.ap.lx[2] = parseInt((this.ap.lx[1] + (this.horCenter - sx) * 3) / 4);
		this.ap.ry[2] = parseInt((this.ap.ry[1] + sy * 2) / 3 - this.dpm / 25);
		this.ap.ly[2] = parseInt((this.ap.ly[1] + sy * 2) / 3 - this.dpm / 25);

		this.ap.rx[3] = parseInt((this.ap.rx[2] + (this.horCenter + sx) * 2) / 3 - this.dpm / 13);
		this.ap.lx[3] = parseInt((this.ap.lx[2] + (this.horCenter - sx) * 2) / 3 + this.dpm / 13);
		this.ap.ry[3] = parseInt((this.ap.ry[2] + sy * 2) / 3 - this.dpm / 40);
		this.ap.ly[3] = parseInt((this.ap.ly[2] + sy * 2) / 3 - this.dpm / 40);

		mx = (this.ap.rx[3] + this.ap.lx[3]) / 2;
		my = (this.ap.ry[3] + this.ap.ly[3]) / 2;

		this.ap.rx[4] = parseInt((mx * 2 + this.ap.rx[3]) / 3);
		this.ap.lx[4] = parseInt((mx * 2 + this.ap.lx[3]) / 3);
		this.ap.ry[4] = parseInt((my * 2 + this.ap.ry[3]) / 3);
		this.ap.ly[4] = parseInt((my * 2 + this.ap.ly[3]) / 3);

		this.ap.hx = parseInt(mx);
		this.ap.hy = parseInt((my * 2 - this.dpm * 2 / 3 + this.base) / 3);
		this.ap.hr = parseInt(this.dpm / 11);

		this.ap.rx[5] = this.ap.hx + parseInt(this.dpm / 20);
		this.ap.lx[5] = this.ap.hx - parseInt(this.dpm / 20);
		this.ap.ry[5] = this.ap.hy + parseInt(this.dpm / 13);
		this.ap.ly[5] = parseInt(this.ap.ry[5]);
	}
};

JMLib.prototype.applyCorrections = function () {
	// Correct ball coordinates
	for (var i = this.balln - 1; i >= 0; i--) {
		this.b[i].gx += this.bm1;
		this.b[i].gy += this.bm1;
	}
};

JMLib.prototype.hand_pos = function (c, h, x, z) {
	//fixme: for testing only, remove
	// x and z must be arrays with one element (pass-by-reference emulation)
	//if (!(x instanceof Array)) document.write("hand_pos assert failure (x)<br>");
	//if (!(z instanceof Array)) document.write("hand_pos assert failure (z)<br>");

	var a;

	if (this.mirror) {
		if (!this.syn && h) c--;
		if (c & 1) a = (--c + h) % this.style_len * 4 + 2;else a = (c + h) % this.style_len * 4;
	} else {
		if (!this.syn && !h) c--;
		if (c & 1) a = (c - h) % this.style_len * 4 + 2;else a = (c + 1 - h) % this.style_len * 4;
	}

	if (h) x[0] = this.styledata[a];else x[0] = -this.styledata[a];

	z[0] = this.styledata[a + 1];
};

JMLib.prototype.juggle = function ( /*Ball*/d) {
	var tp;
	var flag = 0;
	var h, t;
	var tpox = new Array(1);
	tpox[0] = 0;
	var rpox = new Array(1);
	rpox[0] = 0;
	var tpoz = new Array(1);
	tpoz[0] = 0;
	var rpoz = new Array(1);
	rpoz[0] = 0;
	var x = 0;
	var y = 0;

	// Save old values
	d.gx1 = d.gx0;
	d.gy1 = d.gy0;

	d.gx0 = d.gx;
	d.gy0 = d.gy;

	if (d.c < 0) {
		if (this.time_count >= -d.c * this.tw) d.c = -d.c;
	}

	while (true) {
		tp = this.time_count - this.tw * JMLib.xabs(d.c);
		if (tp < this.aw) break;
		d.st &= ~JMLib.OBJECT_UNDER;
		d.c0 = d.c;

		if (d.st & JMLib.OBJECT_HAND) {
			d.c += 2;
			this.flag = 1;
		} else {
			t = d.c;

			if (this.syn) {
				if (this.mirror && !d.chand) t++;else if (!this.mirror && d.chand) t++;
			}

			t %= this.pattw;
			d.bh = this.patt[t][this.r[t]];
			d.c += JMLib.xabs(d.bh);
			if (++this.r[t] >= this.patts[t]) this.r[t] = 0;

			d.thand = d.chand;
			if (d.bh & 1 || d.bh < 0) d.chand = 1 - d.chand;

			flag = 1;
		}
	}

	if (d.c >= 0 && tp >= 0 && !(d.st & JMLib.OBJECT_UNDER)) {
		d.st |= JMLib.OBJECT_UNDER;

		if (d.st & JMLib.OBJECT_HAND) {
			if (d.st & JMLib.OBJECT_MOVE2) {
				d.st |= JMLib.OBJECT_MOVE;
				d.st &= ~JMLib.OBJECT_MOVE2;
			} else {
				d.st &= ~JMLib.OBJECT_MOVE;
			}
		} else {
			t = d.c;

			if (this.syn) {
				if (this.mirror && !d.chand) t++;else if (!this.mirror && d.chand) t++;
			}

			t %= this.pattw;

			if (d.bh == 1) d.st |= JMLib.OBJECT_MOVE;else d.st &= ~JMLib.OBJECT_MOVE;

			for (var i = 0; i < this.patts[t]; i++) {
				h = this.patt[t][i];
				if (h == 1) {
					if (d.chand) this.lhand.st |= JMLib.OBJECT_MOVE2;else this.rhand.st |= JMLib.OBJECT_MOVE2;
				}
				if (h != 2) {
					if (d.chand) this.rhand.st |= JMLib.OBJECT_MOVE2;else this.lhand.st |= JMLib.OBJECT_MOVE2;
					d.st |= JMLib.OBJECT_MOVE;
				}
			}
		}
	}

	if (!(d.st & JMLib.OBJECT_MOVE)) {
		if (d.c < 0) {
			//opera.postError("BEFORE hand_pos(" + -d.c + ", " + d.chand + ", " + tpox[0] + ", " + tpoz[0] + ")");
			this.hand_pos(-d.c, d.chand, tpox, tpoz);
			//opera.postError("AFTER hand_pos(" + -d.c + ", " + d.chand + ", " + tpox[0] + ", " + tpoz[0] + ")");
			rpox[0] = tpox[0];
			rpoz[0] = tpoz[0];
		} else {
			if (d.st & JMLib.OBJECT_UNDER) {
				//opera.postError("BEFORE hand_pos(" + d.c + ", " + d.chand + ", " + tpox[0] + ", " + tpoz[0] + ")");
				this.hand_pos(d.c, d.chand, tpox, tpoz);
				//opera.postError("AFTER hand_pos(" + d.c + ", " + d.chand + ", " + tpox[0] + ", " + tpoz[0] + ")");

				//opera.postError("BEFORE hand_pos(" + (d.c + 2) + ", " + d.chand + ", " + rpox[0] + ", " + rpoz[0] + ")");
				this.hand_pos(d.c + 2, d.chand, rpox, rpoz);
				//opera.postError("AFTER hand_pos(" + (d.c + 2) + ", " + d.chand + ", " + rpox[0] + ", " + rpoz[0] + ")");

				if (tpox[0] != rpox[0] || tpoz[0] != rpoz[0]) {
					this.hand_pos(d.c + 1, d.chand, rpox, rpoz);
					if (tpox[0] != rpox[0] || tpoz[0] != rpoz[0]) d.st |= JMLib.OBJECT_MOVE;
				}
			} else {
				this.hand_pos(d.c - 2, d.chand, tpox, tpoz);
				this.hand_pos(d.c, d.chand, rpox, rpoz);
				if (tpox[0] != rpox[0] || tpoz[0] != rpoz[0]) {
					this.hand_pos(d.c - 1, d.chand, tpox, tpoz);
					if (tpox[0] != rpox[0] || tpoz[0] != rpoz[0]) d.st |= JMLib.OBJECT_MOVE;
				}
			}
		}
	}

	if (d.st & JMLib.OBJECT_MOVE) {
		if (d.bh == 1) {
			this.hand_pos(d.c0 + 1, d.thand, tpox, tpoz);
			this.hand_pos(d.c + 1, d.chand, rpox, rpoz);
		} else if (d.st & JMLib.OBJECT_UNDER) {
			this.hand_pos(d.c, d.chand, tpox, tpoz);
			this.hand_pos(d.c + 1, d.chand, rpox, rpoz);
		} else {
			this.hand_pos(d.c0 + 1, d.thand, tpox, tpoz);
			this.hand_pos(d.c, d.chand, rpox, rpoz);
		}
	}

	if (this.fpu) {
		var fx;

		if (!(d.st & JMLib.OBJECT_HAND) && d.c < 0) {
			if (tpox[0] == 0) {
				fx = 0;
				y = parseInt(tpoz[0] * this.dpm / 20 - tp * this.dpm / 12 / this.tw);
				//opera.postError("x=" + x + ", y=" + y);
				//opera.postError(y, tpoz[0], this.dpm, tp, this.dpm, this.tw);
			} else {
					if (tpox[0] > 0) fx = tpox[0] / 10 - tp / 6 / this.tw;else fx = tpox[0] / 10 + tp / 6 / this.tw;
					y = parseInt(tpoz[0] * this.dpm / 20);
				}
		} else if (!(d.st & JMLib.OBJECT_MOVE)) {
			fx = tpox[0] / 10;
			y = tpoz[0] * this.dpm / 20;
		} else {
			if (d.bh == 1) {
				fx = (tp - this.aw) / this.tw * 2 + 1;
				y = parseInt(this.high[1] * (1 - JMLib.jijo(fx)));
			} else if (d.st & JMLib.OBJECT_UNDER) {
				fx = tp / this.aw * 2 - 1;
				y = parseInt(this.high[0] * (1 - JMLib.jijo(fx)));
			} else {
				fx = tp / (this.tw * JMLib.xabs(d.bh) - this.aw) * 2 + 1;
				y = parseInt(this.high[JMLib.xabs(d.bh)] * (1 - JMLib.jijo(fx)));
			}

			y += parseInt((fx * (rpoz[0] - tpoz[0]) + rpoz[0] + tpoz[0]) * this.dpm / 40);
			d.t = fx; // spin
			fx = (fx * (rpox[0] - tpox[0]) + rpox[0] + tpox[0]) / 20;
		}

		x = parseInt(fx * this.dpm * JMLib.KW);
	} else {
		if (!(d.st & JMLib.OBJECT_HAND) && d.c < 0) {
			if (tpox[0] == 0) {
				x = 0;
				y = parseInt(tpoz[0] * this.dpm / 20 - tp * this.dpm / 12 / this.tw);
			} else {
				if (tpox[0] > 0) x = parseInt(JMLib.XR * tpox[0] / 10 - JMLib.XR * tp / 6 / this.tw);else x = parseInt(JMLib.XR * tpox[0] / 10 + JMLib.XR * tp / 6 / this.tw);
				y = parseInt(tpoz[0] * this.dpm / 20);
			}
		} else if (!(d.st & JMLib.OBJECT_MOVE)) {
			x = parseInt(JMLib.XR * tpox[0] / 10);
			y = parseInt(tpoz[0] * this.dpm / 20);
		} else {
			if (d.bh == 1) {
				x = parseInt(JMLib.XR * (tp - this.aw) * 2 / this.tw + JMLib.XR);
				y = parseInt((JMLib.jijo(JMLib.XR) - JMLib.jijo(x)) / this.high0[1]);
			} else if (d.st & JMLib.OBJECT_UNDER) {
				x = parseInt(JMLib.XR * tp * 2 / this.aw - JMLib.XR);
				y = parseInt((JMLib.jijo(JMLib.XR) - JMLib.jijo(x)) / this.high0[0]);
			} else {
				x = parseInt(JMLib.XR * tp * 2 / (this.tw * JMLib.xabs(d.bh) - this.aw) + JMLib.XR);
				y = parseInt((JMLib.jijo(JMLib.XR) - JMLib.jijo(x)) / this.high0[JMLib.xabs(d.bh)]);
			}

			y += parseInt((x * (rpoz[0] - tpoz[0]) + JMLib.XR * (rpoz[0] + tpoz[0])) * this.dpm / JMLib.XR / 40);
			x = parseInt((x * (rpox[0] - tpox[0]) + JMLib.XR * (rpox[0] + tpox[0])) / 20);
		}

		x = parseInt(x * this.dpm / this.kw0);
	}

	// NOTE:
	//  * The alternative calulations of d->gx and gy below are
	//    from JMWin. They cause the entire pattern to be skewed.

	//opera.postError(d);

	d.gx = this.horCenter + x - 11;
	//d->gx=horCenter + x - 11 * dpm / DW;
	//opera.postError(d);
	//opera.postError("hand_x=" + this.hand_x + " hand_y=" + this.hand_y);

	if (d.st & JMLib.OBJECT_HAND) {
		if (d.chand) d.gx += this.hand_x;else d.gx -= this.hand_x;
		y -= this.hand_y;
	}

	d.gy = this.base - y - 11;
	//d->gy = base - y - 11 * dpm / DW;

	return flag;
};

JMLib.prototype.set_ini = function (rr) {
	var i, j;
	var tw0;
	var aw0;

	this.balln = 0;
	this.pmax = 0;

	if (this.pattw > JMLib.LMAX) return 10;

	if (this.pattw == 0) return 1;

	for (i = 0; i < this.pattw; i++) {
		for (j = 0; j < this.patts[i]; j++) {
			this.balln += JMLib.xabs(this.patt[i][j]);
			this.pmax = JMLib.max(this.pmax, JMLib.xabs(this.patt[i][j]));
		}
	}

	if (this.balln % this.pattw) return 5;
	this.balln = parseInt(this.balln / this.pattw);
	if (this.balln == 0) return 9;
	if (this.balln > JMLib.BMAX) return 9;

	for (i = 0; i < JMLib.LMAX * 2; i++) {
		this.r[i] = 0;
	}for (i = 0; i <= this.balln; i++) {
		j = 0;
		while (this.r[j] == this.patts[j % this.pattw] && j < this.pattw + this.pmax) {
			j++;
		}if (i == this.balln) {
			if (j == this.pattw + this.pmax) break;else return 5;
		}

		this.b[i].st = 0;

		if (this.mirror) {
			if ((j + this.syn) % 2) {
				this.b[i].thand = 1;
				this.b[i].chand = 1;
			} else {
				this.b[i].thand = 0;
				this.b[i].chand = 0;
			}
		} else {
			if ((j + this.syn) % 2) {
				this.b[i].thand = 0;
				this.b[i].chand = 0;
			} else {
				this.b[i].thand = 1;
				this.b[i].chand = 1;
			}
		}

		if (this.syn) this.b[i].c = -parseInt(j / 2) * 2;else this.b[i].c = -j;

		while (j < this.pattw + this.pmax) {
			if (this.r[j] == this.patts[j % this.pattw]) return 5;else this.r[j]++;
			var k = this.patt[j % this.pattw][this.patts[j % this.pattw] - this.r[j]];

			if (this.syn && k < 0) {
				if (j % 2 == 0) j += -k + 1;else j += -k - 1;
			} else {
				j += k;
			}
		}
	}

	if (rr == 0) return 0;

	if (this.pmax < 3) this.pmax = 3;

	tw0 = Math.sqrt(2 / this.ga * this.pmax * this.height_ratio) * 2 / (this.pmax - this.dwell_ratio * 2) * this.smode / this.cSpeed;
	this.tw = parseInt(this.fadd(tw0, 0, 0));
	if (this.tw == 0) return 15;
	aw0 = tw0 * this.dwell_ratio * 2;
	this.aw = parseInt(this.fadd(aw0, 0, 0));
	if (this.aw < 1) this.aw = 1;
	if (this.aw > this.tw * 2 - 1) this.aw = this.tw * 2 - 1;

	this.kw0 = parseInt(JMLib.XR / JMLib.KW);

	if (this.fpu) {
		this.high[0] = -0.2 * this.dpm;
		this.high[1] = this.ga * JMLib.jijo(tw0 / this.smode * this.cSpeed) / 8 * this.dpm;
		for (i = 2; i <= this.pmax; i++) {
			this.high[i] = this.ga * JMLib.jijo((tw0 * i - aw0) / this.smode * this.cSpeed) / 8 * this.dpm;
		}
	} else {
		this.high0[0] = parseInt(-JMLib.jijo(JMLib.XR) / 0.2 * this.dpm);
		this.high0[1] = parseInt(JMLib.jijo(JMLib.XR) / (this.ga * JMLib.jijo(tw0 / this.smode * this.cSpeed) / 8 * this.dpm));
		for (i = 2; i <= this.pmax; i++) {
			this.high0[i] = parseInt(JMLib.jijo(JMLib.XR) / (this.ga * JMLib.jijo((tw0 * i - aw0) / this.smode * this.cSpeed) / 8 * this.dpm));
		}
	}

	for (i = 0; i < this.balln; i++) {
		this.b[i].bh = 0;
		this.b[i].gx = this.horCenter;
		this.b[i].gy = this.verCenter;
		this.b[i].gx0 = this.horCenter;
		this.b[i].gy0 = this.verCenter;
		this.b[i].gx1 = this.horCenter;
		this.b[i].gy1 = this.verCenter;
	}

	if (this.mirror) {
		this.lhand.c = 0;
		if (this.syn) this.rhand.c = 0;else this.rhand.c = -1;
	} else {
		this.rhand.c = 0;
		if (this.syn) this.lhand.c = 0;else this.lhand.c = -1;
	}

	this.rhand.bh = 2;
	this.rhand.st = JMLib.OBJECT_HAND;
	this.rhand.thand = 1;
	this.rhand.chand = 1;
	this.rhand.gx = this.horCenter;
	this.rhand.gy = this.verCenter;
	this.rhand.gx0 = this.horCenter;
	this.rhand.gy0 = this.verCenter;
	this.rhand.gx1 = this.horCenter;
	this.rhand.gy1 = this.verCenter;

	this.lhand.bh = 2;
	this.lhand.st = JMLib.OBJECT_HAND;
	this.lhand.thand = 0;
	this.lhand.chand = 0;
	this.lhand.gx = this.horCenter;
	this.lhand.gy = this.verCenter;
	this.lhand.gx0 = this.horCenter;
	this.lhand.gy0 = this.verCenter;
	this.lhand.gx1 = this.horCenter;
	this.lhand.gy1 = this.verCenter;

	for (i = 0; i < this.pattw; i++) {
		this.r[i] = 0;
	}return 0;
};

JMLib.prototype.set_dpm = function () {
	var cSpeed0;

	cSpeed0 = this.cSpeed;
	this.cSpeed = 2.0;
	this.base = 0;
	this.dpm = 400;

	this.gy_max = 80 - 11;
	this.gy_min = -200 - 11;
	this.gx_max = -1000;
	this.gx_min = 1000;

	if (this.set_ini(1) == 0) {
		for (this.time_count = 0; this.time_count < this.tw * (this.pattw + this.pmax + this.style_len); this.time_count++) {
			for (var i = 0; i < this.balln; i++) {
				this.juggle(this.b[i]);
				this.gy_max = JMLib.max(this.gy_max, this.b[i].gy);
				this.gy_min = JMLib.min(this.gy_min, this.b[i].gy);
				this.gx_max = JMLib.max(this.gx_max, this.b[i].gx + 2 * 11 * this.dpm / this.DW); // changed from X11 version
				this.gx_min = JMLib.min(this.gx_min, this.b[i].gx);
			}

			this.juggle(this.rhand);
			this.juggle(this.lhand);

			this.gy_max = JMLib.max(this.gy_max, this.rhand.gy);
			this.gy_min = JMLib.min(this.gy_min, this.rhand.gy);
			this.gy_max = JMLib.max(this.gy_max, this.lhand.gy);
			this.gy_min = JMLib.min(this.gy_min, this.lhand.gy);

			this.gx_max = JMLib.max(this.gx_max, this.rhand.gx);
			this.gx_min = JMLib.min(this.gx_min, this.rhand.gx);
			this.gx_max = JMLib.max(this.gx_max, this.lhand.gx);
			this.gx_min = JMLib.min(this.gx_min, this.lhand.gx);

			this.arm_x = parseInt((22 - 11) * this.dpm / JMLib.DW);
			this.arm_y = parseInt((16 - 11) * this.dpm / JMLib.DW);

			// from JMWin:
			//ap.rx[0]=rhand.gx +11*dpm/DW+arm_x;
			//ap.ry[0]=rhand.gy +11*dpm/DW+arm_y;
			//ap.lx[0]=lhand.gx +11*dpm/DW-arm_x;
			//ap.ly[0]=lhand.gy +11*dpm/DW+arm_y;
			//

			this.arm_line();

			for (i = 0; i < 5; i++) {
				this.gx_max = JMLib.max(this.gx_max, this.ap.rx[i]);
				this.gx_max = JMLib.max(this.gx_max, this.ap.lx[i]);
				this.gx_min = JMLib.min(this.gx_min, this.ap.rx[i]);
				this.gx_min = JMLib.min(this.gx_min, this.ap.lx[i]);
			}
		}
	}

	if (this.gy_max - this.gy_min > 0) {
		// special handling for smaller screens
		if (this.imageWidth <= 320) {
			if (this.imageWidth > 160) {
				// 160-320 width
				this.dpm = parseInt(this.imageHeight * 280 / (this.gy_max - this.gy_min));
				this.base = parseInt(this.imageHeight - this.gy_max * this.dpm / this.imageHeight - 5);
			} else {
				// 0-160 width
				this.dpm = parseInt(this.imageHeight * 280 / (this.gy_max - this.gy_min));
				this.base = parseInt(this.imageHeight - this.imageHeight / 4);
			}
		} else {
			this.dpm = parseInt(400.0 * (this.imageHeight - 30 * 2) / (this.gy_max - this.gy_min));
			this.base = parseInt(this.imageHeight - 30 - this.gy_max * this.dpm / 400);
		}

		this.gx_min = parseInt(this.horCenter - (this.horCenter - this.gx_min) * this.dpm / 400);
		this.gx_max = parseInt(this.horCenter - (this.horCenter - this.gx_max) * this.dpm / 400);

		// original version
		//dpm=(JML_INT32)(400.0*340/(gy_max-gy_min));
		//if(dpm>DW) dpm=DW;
		//base=370-(JML_INT32)( (JML_FLOAT)gy_max*dpm/400 );
	}

	this.cSpeed = cSpeed0;
};

JMLib.prototype.set_patt = function (s) {
	var flag = 0;
	var flag2 = 0;
	var a = 0;
	var pos = 0;

	if (s.length > JMLib.LMAX) return 10;

	this.pattw = 0;
	this.balln = 0;

	if (s.charAt(0) == '(') this.syn = 1;else this.syn = 0;

	for (var i = 0; i < JMLib.LMAX; i++) {
		if (i >= s.length) {
			if (flag != 0 || flag2 != 0) return 4;
			break;
		}

		if (s.charAt(pos) == '[') {
			flag2 = 1;
			this.patts[this.pattw] = 0;
			pos++;
			continue;
		}
		if (s.charAt(pos) == ']') {
			if (flag2 == 0) return 4;
			flag2 = 0;
			this.pattw++;
			pos++;
			continue;
		}

		if (this.syn == 1) {
			switch (s.charAt(pos)) {
				case '(':
					if (flag != 0) return 4;
					flag = 1;
					pos++;
					continue;
				case ')':
					if (flag != 5) return 4;
					flag = 0;
					pos++;
					continue;
				case ',':
					if (flag != 2) return 4;
					flag = 4;
					pos++;
					continue;
				case 'X':
				case 'x':
					if (flag != 2 && flag != 5) return 4;
					if (flag2) this.patt[this.pattw][this.patts[this.pattw] - 1] = -a;else this.patt[this.pattw - 1][0] = -a;
					pos++;
					continue;
			}
		}

		a = this.ctod(s.charAt(pos));
		if (a == -1) return 6;

		if (this.syn) {
			if (a % 2) return 7;
			if (flag2 == 0 && flag != 1 && flag != 4) return 4;
			if (flag == 1) flag = 2;
			if (flag == 4) flag = 5;
		}

		if (flag2) {
			if (a == 0) return 13;
			this.patt[this.pattw][this.patts[this.pattw]++] = a;
			if (this.patts[this.pattw] > JMLib.MMAX) return 8;
		} else {
			this.patt[this.pattw][0] = a;
			if (a == 0) this.patts[this.pattw++] = 0;else this.patts[this.pattw++] = 1;
		}
		pos++;
		this.balln += a;
	}

	if (this.pattw == 0) return 9;
	this.balln = parseInt(this.balln / this.pattw);

	if (this.balln > JMLib.BMAX) return 9;

	return 0;
};

// fixme: this function won't work
// use similar function from validator instead
/*
 JMLib.prototype.ctod = function(c) {
 if      (c >= '0' && c <= '9') return c - '0';
 else if (c >= 'a' && c <= 'z') return c - 'a' + 10;
 else if (c >= 'A' && c <= 'Z') return c - 'A' + 10;

 return -1;
 }*/

JMLib.prototype.ctod = function (s) {
	var str_0 = new String("0");
	var str_a = new String("a");
	var str_A = new String("A");

	if (s >= '0' && s <= '9') return s.charCodeAt(0) - str_0.charCodeAt(0);else if (s >= 'a' && s <= 'z') return s.charCodeAt(0) - str_a.charCodeAt(0) + 10;else if (s >= 'A' && s <= 'Z') return s.charCodeAt(0) - str_A.charCodeAt(0) + 10;else return -1;
};

JMLib.prototype.startJuggle = function () {
	this.set_dpm();
	if (this.set_ini(1) != 0) return;
	this.xbitset();

	// apply corrections
	this.bm1 = 11 - parseInt(11 * this.dpm / JMLib.DW);
	this.time_count = 0;
	this.time_period = 0;

	this.status = JMLib.ST_JUGGLE;
};

JMLib.prototype.stopJuggle = function () {
	this.status = JMLib.ST_NONE;
};

JMLib.prototype.togglePause = function () {
	if (this.status == JMLib.ST_JUGGLE) this.status = JMLib.ST_PAUSE;else if (this.status == JMLib.ST_PAUSE) this.status = JMLib.ST_JUGGLE;
};

JMLib.prototype.setPause = function (pauseOn) {
	if (this.status != JMLib.ST_NONE) {
		if (pauseOn) this.status = JMLib.ST_PAUSE;else this.status = JMLib.ST_JUGGLE;
	}
};

JMLib.prototype.getStatus = function () {
	return this.status;
};

// fixme: xpos function seems to be wrong
JMLib.prototype.fadd = function (x, k, t) {
	return ((x + t) * JMLib.xpow(10, k) + .5) / JMLib.xpow(10, k);
};

JMLib.prototype.speedUp = function () {
	this.cSpeed = JMLib.SPEED_MAX;
	this.set_ini(0);
};

JMLib.prototype.speedDown = function () {
	this.cSpeed = JMLib.SPEED_MIN;
	this.set_ini(0);
};

JMLib.prototype.speedReset = function () {
	this.cSpeed = JMLib.SPEED_DEF;
	this.set_ini(0);
};

JMLib.prototype.setSpeed = function (s) {
	this.cSpeed = s;
	this.set_ini(0);
};

JMLib.prototype.speed = function () {
	return this.cSpeed;
};

JMLib.prototype.doJuggle = function () {
	var i,
	    tone = 0;

	if (this.status == JMLib.ST_PAUSE || this.status == JMLib.ST_NONE) {
		return 0;
	}

	this.time_count++;

	if (this.time_count < this.aw) this.time_count = this.aw;

	this.time_period = parseInt((this.time_count - this.aw) / this.tw);
	this.time_period %= this.pattw;
	//this.time_period = time_count % pattw;

	for (i = 0; i < this.balln; i++) {
		if (this.juggle(this.b[i]) && this.beep) tone = JMLib.max(tone, JMLib.xabs(this.b[i].bh));
	}if (this.juggle(this.rhand) + this.juggle(this.lhand) > 0) {
		//if (back_on==1) patt_print(1);
	}

	this.arm_line();
	this.applyCorrections();

	if (this.scalingMethod == JMLib.SCALING_METHOD_DYNAMIC) this.doCoordTransform();

	return tone;
};

JMLib.prototype.xbitset = function () {
	var i,
	    j = 0;

	// data is used to create the hand bitmaps
	var data = [0, 18, 0, 23, 17, 23, 20, 22, 22, 20, 23, 17, 23, 12, 18, 12, 18, 16, 16, 18, 0, 18, 12, 15, 23, 17, 99, 99];

	// initialize the data array.
	for (i = 0; data[i] < 99; i++) {
		data[i] = parseInt((data[i] - 11) * this.dpm / JMLib.DW);
	} // apply hand placement offsets
	this.hand_x = data[i - 4] + 2;
	this.hand_y = data[i - 3] + 2;
	this.arm_x = data[i - 2];
	this.arm_y = data[i - 1];

	// calculate hand polygons
	for (i = 0; data[i + 6] < 99; i += 2) {
		if (j > 9) break;

		this.handpoly_ex.rx[j] = 11 + data[i];
		this.handpoly_ex.ry[j] = 10 + data[i + 1];

		this.handpoly_ex.lx[j] = 12 - data[i];
		this.handpoly_ex.ly[j] = 10 + data[i + 1];

		j++;
	}

	for (i = 0; i <= 9; i++) {
		this.handpoly.rx[i] = this.handpoly_ex.rx[i];
		this.handpoly.ry[i] = this.handpoly_ex.ry[i];
		this.handpoly.lx[i] = this.handpoly_ex.lx[i];
		this.handpoly.ly[i] = this.handpoly_ex.ly[i];
	}
};

JMLib.prototype.doStepcalc = function () {
	var i;
	var stp = 0; // position in steps array
	var pos = 0; // position in string

	// reset
	for (i = 0; i < JMLib.LMAX; i++) {
		this.steps[i] = -1;
	}

	// Synchronous pattern
	if (this.syn) {
		while (pos <= this.siteswap.length) {
			if (this.siteswap.charAt(pos) == '(') {
				this.steps[stp] = pos;
				stp += 2;
				while (this.siteswap.charAt(pos) != ')') {
					pos++;
				}
				pos++;
			} else if (pos == this.siteswap.length) {
				this.steps[stp] = pos;
				break;
			} else {
				this.error("Internal error");
				return;
			}
		}
	} else {
		while (pos <= this.siteswap.length) {
			if (this.siteswap.charAt(pos) == '(') {
				this.error("Internal error");
				return;
			}
			// Multiplex
			else if (this.siteswap.charAt(pos) == '[') {
					this.steps[stp++] = pos;
					while (this.siteswap.charAt(pos) != ']') {
						pos++;
					}
					pos++;
				}
				// Normal throw
				else {
						this.steps[stp++] = pos++;
					}
		}
	}
};

module.exports = JMLib;