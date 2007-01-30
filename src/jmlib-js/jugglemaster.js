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
var timerInterval;
var siteonscreenEnabled;

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

function setSpeed(s) {
  if      (s == 1) { setTimerInterval(300); } // slow
  else if (s == 2) { setTimerInterval(200); }	
  else if (s == 3) { setTimerInterval(100); }	
  else if (s == 4) { setTimerInterval(80); }	
  else if (s == 5) { setTimerInterval(60); }	
  else if (s == 6) { setTimerInterval(40); }	
  else if (s == 7) { setTimerInterval(20); }	
  else if (s == 8) { setTimerInterval(10); }	
  else if (s == 9) { setTimerInterval(5); }	
  else if (s ==10) { setTimerInterval(1); } // fast
  else { setTimerInterval(10); } // fallback  
}

function setTimerInterval(t) {
  //if (t < 5) { t = 5; }
  timerInterval = t;
}

function enableSiteonscreen() {
  siteonscreenEnabled = true;
  document.getElementById('siteonscreen').style.visibility = "visible";
}

function disableSiteonscreen() {
  siteonscreenEnabled = false;
  document.getElementById('siteonscreen').style.visibility = "hidden";
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

// Shows the site on screen inside an element called siteonscreen
// if the element is not found, no site is shown on screen
function show_siteonscreen() {
  if (!siteonscreenEnabled) { return; }

  var el = document.getElementById('siteonscreen');
  
  if (!el) { return; }

  var sitetext = "";
  var start = jmlib.getSiteposStart();
  var stop  = jmlib.getSiteposStop();
  var diff = stop - start;
  
  // first part of string
  if (start > 0) {
    sitetext += jmlib.siteswap.substring(0, start);
  }
  
  // active part of string
  sitetext += '<b style="color: red; font-weight: normal;">';
  sitetext += jmlib.siteswap.substring(start, stop);
  sitetext += '</b>';
  
  // last part of string
  sitetext += jmlib.siteswap.substring(stop);
  
  el.innerHTML = sitetext;
}

// chooses a random pattern and loads it
function loadRandomPattern() {
  var pattno = Math.floor(Math.random()* (patterns.length+1));
  jmlib.stopJuggle();
  PatternLoader.loadPatternEx(jmlib, pattno);
  jmlib.startJuggle();
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

  timerInterval = 10;
  timerEnabled = true;
  siteonscreenEnabled = true;
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
  siteonscreenEnabled = true;
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

function changePattern(idx) {
  jmlib.stopJuggle();
  var ret = PatternLoader.loadPatternEx(jmlib, idx);
  jmlib.startJuggle();

  return ret;
}