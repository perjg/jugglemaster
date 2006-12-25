/*
 * jmwidget - JuggleMaster Widget
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

if (document.implementation.hasFeature("Events", "2.0")) {
  window.addEventListener('load', function(ev) {
    // add behavior to the flip button on the front side
    document.getElementById('flipfront').addEventListener('click', function(ev) {
        flipWidget("back");
    }, false);

    // add behavior to the flip button on the back side
    document.getElementById('flipback').addEventListener('click', function(ev) {
        flipWidget("front");
    }, false);

    // add behavior to the close button
    document.getElementById('closefront').addEventListener('click', function(ev) {
        window.close();
    }, false);

    document.getElementById('closeback').addEventListener('click', function(ev) {
        window.close();
    }, false);

    document.getElementById('switch').addEventListener('click', function(ev) {
        switchHandler();
    }, false);
  }, false);
}

function changeSiteHandler() {
  var site = document.getElementById('site').value;
  var style = document.getElementById('style').value;
  
  /*
  if (!changeSite(site)) {
    showError(getLastError());  
  }
  */
  
  if (changeSite(site)) {
    changeStyle(style);
  }
  else {
    showError(getLastError());
  }  
}

function changeStyleHandler() {
  var style = document.getElementById('style').value;
  if (style == "Mills_Mess") { style = "Mills Mess"; }
  
  if (!changeStyle(style)) {
    showError(getLastError());  
  }
}

function submitSiteHandler() {
  var site = document.getElementById('site').value;
  var style = document.getElementById('style').value;
  
  if (changeSite(site)) {
    changeStyle(style);
  }
  else {
    showError(getLastError());
  }
  
  return false;
}

function changeCategoryHandler() {
  var category = document.getElementById('category').value;
  loadPatterns(category);
  changePatternHandler();
}

function changePatternHandler() {
  var pattern = document.getElementById('pattern').value;
  
  changePattern(pattern);
}

function submitPatternHandler() {

}

function showError(error) {
  document.getElementById('site_text').innerHTML = "Error: "+ error;
  setTimeout(setWindowHeader, 2000);
}

function setWindowHeader() {
  if (switchState == "pattern_content") {
    document.getElementById('site_text').innerHTML = "Select Pattern";
  }
  else {
    document.getElementById('site_text').innerHTML = "Enter Siteswap";
  }
}

function flipWidget(to) {
  switch (to) {
    case "back":
      if (typeof widget != "undefined" && widget.prepareForTransition)
        widget.prepareForTransition("ToBack");
      document.getElementById('front').style.display = "none";
      document.getElementById('back_site').style.display = "block";
      disableTimer();
      if (typeof widget != "undefined" && widget.prepareForTransition)
        setTimeout ('widget.performTransition();', 0);
      break;
    case "front":
      if (typeof widget != "undefined" && widget.prepareForTransition)
        widget.prepareForTransition("ToFront");
      document.getElementById('back_site').style.display = "none";
      document.getElementById('front').style.display = "block";
      enableTimer();
      if (typeof widget != "undefined" && widget.prepareForTransition)
        setTimeout ('widget.performTransition();', 0);
      break;
    default:
      break;
  }
}

var switchState;

function switchHandler() {
  if (switchState == "pattern_content") { // switch to enter siteswap
    document.getElementById('pattern_content').style.visibility = "hidden";
    document.getElementById('site_content').style.visibility = "visible";
    document.getElementById('switch').title = "Switch to Select Pattern View";
    switchState = "site_content";
  }
  else { // switch to select pattern
    document.getElementById('pattern_content').style.visibility = "visible";
    document.getElementById('site_content').style.visibility = "hidden";
    document.getElementById('switch').title = "Switch to Enter Siteswap View";
    switchState = "pattern_content";
  }
  
  setWindowHeader();
}

// Add categories to the category select list
function loadCategories() {
  var catList = document.getElementById('category');
  if (catList == null) { return; }

  for (var i = 0; i < groups.length; i++) {
    var group = groups[i];
	var name = group[0];
	
	if (name.length > 28) {
	  name = name.substring(0, 28);
	}
    catList.options[catList.options.length] = new Option(name, i, false, false);
  }
}

// Load patterns for a specific category into the pattern select list
function loadPatterns(idx) {
  var patternList = document.getElementById('pattern');
  if (patternList == null) { return; }

  var group = groups[idx];
  if (group == null) { return; }
  var from = group[1];
  var to   = group[2];
  
  //alert("loading patterns from " + from + " to " + to);
  patternList.options.length = 0;
  
  for (var i = from; i <= to; i++) {  
    var pattern = patterns[i];
	var name = pattern[0];
	
	if (name.length > 28) {
	  name = name.substring(0, 28);
	}
	//alert("Adding pattern " + i + " " + name);
	patternList.options[patternList.options.length] = new Option(name, i, false, false);
  }
}

// Load settings options
function loadSettings() {
  var speed = document.getElementById('speed');
  var randomize = document.getElementById('randomize');
  var showsite = document.getElementById('showsite');
  
  speed.options[speed.options.length] = new Option("Normal", "normal", false, false);
  speed.options[speed.options.length] = new Option("Slow", "slow", false, false);
  speed.options[speed.options.length] = new Option("Fast", "fast", false, false);
  
  randomize.options[randomize.options.length] = new Option("Off", "0", false, false);
  randomize.options[randomize.options.length] = new Option("15 sec", "15", false, false);
  randomize.options[randomize.options.length] = new Option("30 sec", "30", false, false);
  randomize.options[randomize.options.length] = new Option("1 min", "60", false, false);
  
  showsite.options[showsite.options.length] = new Option("Off", "off", false, false);
  showsite.options[showsite.options.length] = new Option("On", "on", false, false);
}

// Dashboard onhide handler
function onhide() {
  disableTimer();
}

// Dashboard onshow handler
function onshow() {
  enableTimer();
}

// IE specific event handlers
function flipWidgetBack()  { flipWidget("front"); }
function flipWidgetFront() { flipWidget("back"); }
function closeWindow()     { window.close(); }

function initialize() {
  // IE specific event handlers
  // DOM 2 event model compliant browsers are registered through window.addEventListener
  if (!document.implementation.hasFeature("Events", "2.0")) {
    document.getElementById('flipback').attachEvent('onclick', flipWidgetBack);
    document.getElementById('flipfront').attachEvent('onclick', flipWidgetFront);
    document.getElementById('closefront').attachEvent('onclick', closeWindow);
    document.getElementById('closeback').attachEvent('onclick', closeWindow);
    document.getElementById('switch').attachEvent('onclick', switchHandler);
  }

  // set event handlers for site entry form
  document.getElementById('site').onchange      = changeSiteHandler;
  document.getElementById('style').onchange     = changeStyleHandler; 
  document.getElementById('entersite').onsubmit = submitSiteHandler;
  
  // set event handlers for pattern selection form
  document.getElementById('category').onchange      = changeCategoryHandler;
  document.getElementById('pattern').onchange       = changePatternHandler; 
  document.getElementById('selectpattern').onsubmit = submitPatternHandler;

  // set initial state of pattern selector and siteswap selector
  switchState = "pattern_content";
  document.getElementById('pattern_content').style.visibility = "visible";
  document.getElementById('site_content').style.visibility = "hidden";
  

  // Dashboard initialization
  if (typeof widget != "undefined" && widget.prepareForTransition) {
    widget.onhide = onhide;
    widget.onshow = onshow;

    infoButton = new AppleInfoButton(document.getElementById("infobutton"),
                                     document.getElementById("front"),
                                     "black", "black", flipWidgetFront);

    // Get rid of the standard buttons
    document.getElementById("closefront").style.visibility = "hidden";
    document.getElementById("flipfront").style.visibility = "hidden";
    document.getElementById("closeback").style.visibility = "hidden";
  }
  
  // load categories and patterns
  loadCategories();
  loadPatterns(0);
  
  // load settings
  loadSettings();
}
