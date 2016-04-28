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

import JuggleMaster from '../index';

const jm = new JuggleMaster(),
	jmlib = jm.jmlib,
	validator = jm.validator;

let canvas, ctx, timerEnabled, timerInterval, siteonscreenEnabled;

function getArgs() {
	let args = {},
		query = location.search.substring(1),
		pairs;

	if (query === null) { return null; }

	pairs = query.split('&');

	for (let i = 0; i < pairs.length; i++) {
		let pos = pairs[i].indexOf('=');
		if (pos === -1) { continue; }

		let argName = pairs[i].substring(0, pos),
			value   = pairs[i].substring(pos+1);

		args[argName] = decodeURIComponent(value);
	}

	return args;
}

function loop() {
	jmlib.doJuggle();
	ctx.clearRect(0, 0, canvas.width, canvas.height);
	ctx.lineWidth = '1.0';

	// draw head
	ctx.beginPath();
	ctx.moveTo(jmlib.ap.hx+jmlib.ap.hr, jmlib.ap.hy);
	ctx.arc(jmlib.ap.hx, jmlib.ap.hy, jmlib.ap.hr, 0, Math.PI*2, false);
	ctx.closePath();
	ctx.stroke();

	// draw arms
	for (var i=0;i<5;i++) {
		ctx.beginPath();
		ctx.moveTo(jmlib.ap.rx[i], jmlib.ap.ry[i]);
		ctx.lineTo(jmlib.ap.rx[i+1], jmlib.ap.ry[i+1]);
		ctx.closePath();
		ctx.stroke();

		ctx.beginPath();
		ctx.moveTo(jmlib.ap.lx[i], jmlib.ap.ly[i]);
		ctx.lineTo(jmlib.ap.lx[i+1], jmlib.ap.ly[i+1]);
		ctx.closePath();
		ctx.stroke();
	}

	// draw hands
	for (var i=0; i < 10; i++) {
		ctx.beginPath();
		ctx.moveTo(jmlib.rhand.gx + jmlib.handpoly.rx[i],        jmlib.rhand.gy + jmlib.handpoly.ry[i]);
		ctx.lineTo(jmlib.rhand.gx + jmlib.handpoly.rx[(i+1)%10], jmlib.rhand.gy + jmlib.handpoly.ry[(i+1)%10]);
		ctx.closePath();
		ctx.stroke();

		ctx.beginPath();
		ctx.moveTo(jmlib.lhand.gx + jmlib.handpoly.lx[i],        jmlib.lhand.gy + jmlib.handpoly.ly[i]);
		ctx.lineTo(jmlib.lhand.gx + jmlib.handpoly.lx[(i+1)%10], jmlib.lhand.gy + jmlib.handpoly.ly[(i+1)%10]);
		ctx.closePath();
		ctx.stroke();
	}

	// draw balls
	var r = jmlib.getBallRadius();
	if (r < 1) { r = 1; }
	for (var i = 0; i < jmlib.numBalls(); i++) {
		var ball = jmlib.b[i];

		ctx.beginPath();
		ctx.moveTo(ball.gx+r+r, ball.gy);
		ctx.arc(ball.gx+r, ball.gy+r, r, 0, Math.PI*2, false);
		ctx.fillStyle = getColor(i);
		ctx.fill();
		ctx.closePath();
	}

	// show siteswap
	show_siteonscreen();

	if (timerEnabled) {
		setTimeout(loop, timerInterval);
	}
}

function getColor(i) {
	switch(i%10) {
		case 0:
			return 'red';
		case 1:
			return 'blue';
		case 2:
			return 'green';
		case 3:
			return 'orange';
		case 4:
			return 'pink';
		case 5:
			return 'purple';
		case 6:
			return 'magenta';
		case 7:
			return 'red';
		case 8:
			return 'red';
		case 9:
			return 'red';
		default:
			return 'red';
	}
}

// Shows the site on screen inside an element called siteonscreen
// if the element is not found, no site is shown on screen
function show_siteonscreen() {
	if (!siteonscreenEnabled) { return; }

	var el = document.getElementById('siteonscreen');

	if (!el) { return; }

	var sitetext = '';
	var start = jmlib.getSiteposStart();
	var stop  = jmlib.getSiteposStop();
	var diff = stop - start;

	// first part of string
	if (start > 0) {
		sitetext += jmlib.siteswap.substring(0, start);
	}

	// active part of string
	sitetext += '<b style=\'color: red; font-weight: normal;\'>';
	sitetext += jmlib.siteswap.substring(start, stop);
	sitetext += '</b>';

	// last part of string
	sitetext += jmlib.siteswap.substring(stop);

	el.innerHTML = sitetext;
}

function initialize() {
	const args  = getArgs();
	let site    = args.hasOwnProperty('site') ? args.site : '3',
		style   = args.hasOwnProperty('style') ? args.style : 'Normal',
		pattern = args.hasOwnProperty('pattern') ? args.pattern : null,
		pattNo  = args.hasOwnProperty('pattno') ? args.pattno : null;

	if (!validator.validateSite(site)) {
		site = '0';
	}

	canvas = document.getElementById('cv');
	ctx = canvas.getContext('2d');

	jmlib.setScalingMethod(jmlib.SCALING_METHOD_DYNAMIC);
	jmlib.setWindowSize(canvas.width, canvas.height);

	if (pattern != null) {
		PatternLoader.loadPattern(jmlib, pattern);
	}
	else if (pattNo != null) {
		PatternLoader.loadPatternEx(jmlib, pattNo);
	}
	else {
		jmlib.setPattern(site, site, jmlib.height_ratio, jmlib.dwell_ratio);
		jmlib.setStyleEx(style);
	}

	jmlib.startJuggle();

	timerInterval = 10;
	timerEnabled = true;
	siteonscreenEnabled = true;
	loop();
}

initialize();
