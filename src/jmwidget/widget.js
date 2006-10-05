// 	$Id: validator.cpp 208 2006-08-04 01:40:24Z per $	 

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

  
}, false);

function changeSiteHandler() {
  var site = document.getElementById('site').value;
  
  if (!changeSite(site)) {
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
  setTimeout(hideError, 2000);
}

function hideError() {
  document.getElementById('site_text').innerHTML = "Enter Siteswap";
}

function flipWidget(to) {
  switch (to) {
    case "back":
      document.getElementById('front').style.display = "none";
      document.getElementById('back_site').style.display = "block";
	  disableTimer();
      break;
    case "front":
      document.getElementById('back_site').style.display = "none";
      document.getElementById('front').style.display = "block";
	  enableTimer();
      break;
    default:
      break;
  }
}

// Add categories to the category select list
function loadCategories() {
  var catList = document.getElementById('category');
  if (catList == null) { return; }

  for (var i = 0; i < groups.length; i++) {
    var group = groups[i];
	var name = group[0];
	
	if (name.length > 30) {
	  name = name.substring(0, 30);
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
	
	if (name.length > 30) {
	  name = name.substring(0, 30);
	}
	//alert("Adding pattern " + i + " " + name);
	patternList.options[patternList.options.length] = new Option(name, i, false, false);
  }
}

// Dashboard onhide handler
function onhide() {
  disableTimer();
}

// Dashboard onshow handler
function onshow() {
  enableTimer();
}

function initialize() {
  // set event handlers for site entry form
  document.getElementById('site').onchange      = changeSiteHandler;
  document.getElementById('style').onchange     = changeStyleHandler; 
  document.getElementById('entersite').onsubmit = submitSiteHandler;
  
  // set event handlers for pattern selection form
  document.getElementById('category').onchange      = changeCategoryHandler;
  document.getElementById('pattern').onchange       = changePatternHandler; 
  document.getElementById('selectpattern').onsubmit = submitPatternHandler;
  
  // set event handlers for dashboard
  if (window.widget) {
    widget.onhide = onhide;
    widget.onshow = onshow;
  }
  
  // load categories and patterns
  loadCategories();
  loadPatterns(0);
}
