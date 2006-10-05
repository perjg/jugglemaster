/*
 * jmlib-js - Portable JuggleMaster Library (JavaScript Version)
 * Version 1.0
 * (C) Per Johan Groland 2006
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

var canvas, ctx;
var jmlib;
var timerEnabled;

function log(s) {
  // Safari
  if (window.console) { window.console.log(s); }
  // Opera
  if (opera) { opera.postError(s); }
}

function getArgs() {
  var args = [];
  var query = location.search.substring(1);
  if (query == null) { return null; }    
  var pairs = query.split("&");
  
  for (var i = 0; i < pairs.length; i++) {
    var pos = pairs[i].indexOf("=");
    if (pos == -1) { continue; }
    var argname = pairs[i].substring(0, pos);
    var value   = pairs[i].substring(pos+1);
    value = decodeURIComponent(value);
    
    args[argname] = value;
  }
  return args;
}

function enableTimer() {
  timerEnabled = true;
  loop();
}

function disableTimer() {
  timerEnabled = false;
}

function loop() {
  jmlib.doJuggle();  
  ctx.clearRect(0, 0, canvas.width, canvas.height);	
  ctx.lineWidth = "1.0";	

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
  for (var i = 0; i < jmlib.numBalls(); i++) {
    var ball = jmlib.b[i];
  
    ctx.beginPath();
    ctx.moveTo(ball.gx+r+r, ball.gy);
    ctx.arc(ball.gx+r, ball.gy+r, r, 0, Math.PI*2, false);
    ctx.fillStyle = getColor(i);
    ctx.fill();
    ctx.closePath();	  
	}

  if (timerEnabled) {
    setTimeout(loop, 10);
  }
}

function getColor(i) {
  switch(i%10) {
    case 0:
	  return "red";
    case 1:
	  return "blue";
    case 2:
	  return "green";
    case 3:
	  return "yellow";
    case 4:
	  return "pink";
    case 5:
	  return "purple";
    case 6:
	  return "magenta";
    case 7:
	  return "red";
    case 8:
	  return "red";
    case 9:
	  return "red";
	default:
	  return "red";
  }
}

function load() {
  var args = getArgs();
  var site = null;
  var style = null;
  var pattern = null;
  var pattno  = null;
  
  if (args != null) {
    site = args["site"];
    style = args["style"];
	pattern = args["pattern"];
	pattno = args["pattno"];	
  }

  if (site == null)  { site = "3"; }
  if (style == null) { style = "Normal"; }

  canvas = document.getElementById("cv");
  ctx = canvas.getContext("2d");

  //cb = function(err) { alert(err); }

  jmlib = new JMLib(null);
  jmlib.setScalingMethod(JMLib.SCALING_METHOD_DYNAMIC);
  jmlib.setWindowSize(canvas.width, canvas.height);
  
  if (pattern != null) {
    PatternLoader.loadPattern(jmlib, pattern);    
  }
  else if (pattno != null) {
    PatternLoader.loadPatternEx(jmlib, pattno);
  }
  else {
    jmlib.setPattern(site, site, jmlib.height_ratio, jmlib.dwell_ratio);
    jmlib.setStyleEx(style);
  }
  
  jmlib.startJuggle();

  timerEnabled = true;
  loop();
}

function loadWidget() {
  //fixme: read from cookie here
  site = "3";
  style = "Normal";

  canvas = document.getElementById("cv");
  ctx = canvas.getContext("2d");

  //fixme: callback doesn't work
  //cb = function(err) { alert(err); }

  jmlib = new JMLib(null);
  jmlib.setScalingMethod(JMLib.SCALING_METHOD_DYNAMIC);
  jmlib.setWindowSize(canvas.width, canvas.height);
  jmlib.setPattern(site, site, jmlib.height_ratio, jmlib.dwell_ratio);
  jmlib.setStyleEx(style);
  jmlib.startJuggle();

  timerEnabled = true;
  loop();
}

function changeSite(site) {
  if (jmlib.siteswap == site) {
    return true;
  }

  if (!JMValidator.validateSite(site)) {
    jmlib.lastError = "Invalid siteswap";
    return false;
  }

  jmlib.stopJuggle();
  var ret = jmlib.setPattern(site, site, jmlib.height_ratio, jmlib.dwell_ratio);
  jmlib.startJuggle();
  
  return ret;
}

function validateSite(site) {
  return JMValidator.validateSite(site);
}


function changeStyle(style) {
  return jmlib.setStyleEx(style);
}

function getLastError() {
  return jmlib.lastError;
}

function getCurrentSite() {
  return jmlib.siteswap;
}