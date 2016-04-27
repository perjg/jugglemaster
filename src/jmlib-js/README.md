README for jmlib-js

jmlib-js is jmlib (JuggleMaster portable library) ported to JavaScript.

The core library is contained in the following files:
pattern_data.js
patterns.js
validator.js
jmlib.js

The files should be included in that order using <script> tags.

An example implementation using canvas is included in the file
jugglemaster.html. It expects the arguments site to set the
siteswap and style to set the style (to one of the standard
styles, i.e Normal, Reverse, Mills Mess, ...). 


BROWSER COMPATIBILITY

The core library does not rely on client-side JavaScript extensions and should work
with any browser with a JavaScript 1.5 compliant interpreter. The included example
uses the canvas tag, so it will only work in browsers that support it, this includes:

- Opera 9 (tested in Windows and in Mac OS X)
- Firefox 1.5 (tested in Windows and in Mac OS X)
- Safari (Mac OS X 10.4)
- Internet Explorer 6 using excanvas to simulate canvas functionality.

Any speed issues are mostly due to the drawing of lines using the canvas tag, and
can probably be optimized for a specific browser if necessary.


Per Johan Groland (per AT akafugu.jp)

