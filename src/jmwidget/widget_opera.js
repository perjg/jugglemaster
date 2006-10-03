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

function showError(error) {
  document.getElementById('site_text').innerHTML = "Error: "+ error;
  setTimeout(hideError, 2000);
}

function hideError() {
  document.getElementById('site_text').innerHTML = "Enter Siteswap";
}

function flipWidget(to){
  switch (to) {
    case "back":
      document.getElementById('front').style.display = "none";
      document.getElementById('back_site').style.display = "block";
      break;
    case "front":
      document.getElementById('back_site').style.display = "none";
      document.getElementById('front').style.display = "block";
      break;
    default:
      break;
  }
}

function init_form() {
  document.getElementById('site').onchange = changeSiteHandler;
  document.getElementById('style').onchange = changeStyleHandler; 
  document.getElementById('entersite').onsubmit = submitSiteHandler;
}
