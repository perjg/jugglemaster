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

function PatternLoader() {}
 
PatternLoader.getGroups = function() {
  return groups;
}

PatternLoader.getPatterns = function() {
  return patterns;
}

PatternLoader.getStyles = function() {
  return styles;
}

PatternLoader.getPattern = function(index) {
  return patterns[index];
}

PatternLoader.getStyle = function(name) {
  return styles[name];
}
 
// load style by name
PatternLoader.loadStyle = function(jmlib, name) {
  var style = styles[name];
  
  if (style == null) { return false; }
  
  var length = style[0];
  var data   = style[1];

  //opera.postError("Loading style name='" + name + " length = '" + length + "' data='" + data + "'");
  
  return jmlib.setStyle(name, length, data);
}
 
// Load pattern by index
PatternLoader.loadPatternEx = function(jmlib, index) {
  var pattern = patterns[index];
  
  if (pattern == null) { return false; }
  
  var name  = pattern[0];
  var site  = pattern[1];
  var style = pattern[2];
  var hr    = pattern[3];
  var dr    = pattern[4];
  
  //opera.postError("Loading pattern " + index + " name='" + name + "' site='" + site + "' style='" + style + "' hr='" + hr + "' dr='" + dr +"'");
  
  if (jmlib.setPattern(name, site, hr, dr)) {
    return PatternLoader.loadStyle(jmlib, style);
  }
  return false;
}

// Load pattern by name
PatternLoader.loadPattern = function(jmlib, name) {
  for (var i = 0; i < patterns.length; i++) {
    var pattern = patterns[i];
	
	if (pattern == null) { return false; }
	
	if (pattern[0] == name)
	  return PatternLoader.loadPatternEx(jmlib, i);
  }
  
  return false;
}