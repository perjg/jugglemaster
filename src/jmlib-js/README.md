JuggleMaster library for Javascript (jmlib-js)
=======

jmlib-js is jmlib (JuggleMaster portable library) ported to JavaScript.

BASIC USAGE
=======

To initialize the library:

```javascript
import JuggleMaster from 'jmlib';

const jm = new JuggleMaster(),
	jmlib = jm.jmlib,
	validator = jm.validator;
```

jmlib is an instance of the main JMLib class, which is used to do the actual juggling animation.
validator is an independent class that can be used to verify any siteswap.

DEMO
====

An example implementation using canvas is included in the file demo/index.html.

It expects the arguments site to set the siteswap and style to set the style (to one of the standard
styles, i.e Normal, Reverse, Mills Mess, ...). 

