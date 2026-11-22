# Overview

The application is a juggling simulator that has two distinct juggling engines
JuggleSaver and JuggleMaster.

Both engines support 2D and 3D rendering. For 3D rendering, three object types are supported: Balls, Rings and Clubs. Each engine also draws a stickman type juggler using cylinders.

The shared library / 3D code is in jmlib

There are two applications of interest:
* jmdlx: Desktop application with OpenGL support
  * Build with "legacy" fixed pipeline rendering: `USE_SHADERS=0 make osx` (default)
  * Build with custom shader rendering: `USE_SHADERS=1 make osx`
* jmwasm: Webassembly port using the same shared jmlib as jmdlx, comes with a simple web page wrapper for testing
  * Build with `make` and run `make serve` to serve the web page for testing
  * jmwasm/ui -> React UI for jmwasm

Current state:
* jmdlx with legacy rendering works perfectly both for 2D and 3D mode
* jmdlx with custom shader:
  * Works for 2D mode
  * Works for 3D mode, but has some minor issues
  * jmdlx with custom shader and the webassembly version render the same

**The new custom shader mode should be visually IDENTICAL to the legacy rendering mode. When writing code to implement a visual feature for the shader mode, refer to the code for legacy rendering and make sure that the resulting visuals will be exactly the same**

When making changes, run make inside the src/jmwasm directory to make sure you did not introduce any compiler errors.

When working on jmwasm, use @src/jmwasm/ui/DEVELOPMENT_RULES.md
