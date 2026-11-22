# Building JMDlx on Apple Silicon Mac

✅ **Status: Successfully builds and runs on Apple Silicon (ARM64) with wxWidgets 3.3.1**
✅ **OpenGL 3D support enabled with JuggleSaver rendering engine**

This guide provides instructions for building JMDlx (the wxWidgets version of JuggleMaster) on Apple Silicon Macs. The codebase has been updated to work with modern wxWidgets 3.x and includes full 3D OpenGL support.

## Prerequisites

### Required Libraries

1. **wxWidgets** - The main GUI framework (previously called wxWindows)
2. **sqlite3** - For pattern database support
3. **OpenGL** - For 3D rendering (already included in macOS)

### Optional Libraries

- **ffmpeg** - For MPEG video export (not required for basic functionality)

## Installation Steps

### 1. Install Dependencies

Using Homebrew:

```bash
# Install wxWidgets
brew install wxwidgets

# Install sqlite3 (usually already on macOS, but just to be sure)
brew install sqlite3
```

### 2. Build the jmlib Library

The jmdlx application depends on jmlib, so build it first:

```bash
cd src/jmlib
make
```

### 3. Build JMDlx

#### Option A: Build macOS App Bundle (Recommended)

From the `src/jmdlx` directory:

```bash
cd src/jmdlx
make osx
```

This will create `JuggleMaster.app` with all resources bundled.

#### Option B: Build from Top-Level Directory

```bash
# From the project root
make jmdlx
```

This creates the `jmdlx` binary in `src/jmdlx/`.

## Running the Application

### Option 1: Double-click the App Bundle

After building with `make osx`, you can open the app from Finder or use:

```bash
open src/jmdlx/JuggleMaster.app
```

### Option 2: Run from Command Line

Run the binary directly with optional pattern/style arguments:

```bash
# Run with default pattern
./src/jmdlx/JuggleMaster.app/Contents/MacOS/JuggleMaster

# Run with a specific siteswap
./src/jmdlx/JuggleMaster.app/Contents/MacOS/JuggleMaster "3"

# Run with style option
./src/jmdlx/JuggleMaster.app/Contents/MacOS/JuggleMaster -s "Mills Mess" "3"

# View help
./src/jmdlx/JuggleMaster.app/Contents/MacOS/JuggleMaster --help
```

## OpenGL 3D Support

✅ **OpenGL support is now enabled by default!**

The build now includes OpenGL support for 3D rendering with the JuggleSaver engine. This provides:
- 3D ball rendering
- Interactive camera controls (rotate, zoom, pan)
- Auto-rotate mode
- Colored balls option

### Disabling OpenGL Support

If you want to build without OpenGL support, edit `src/jmdlx/Makefile` and change:
```makefile
OPENGL_SUPPORT=1
```
to:
```makefile
OPENGL_SUPPORT=0
```

### Libraries Required

- **wxWidgets with OpenGL support** (`wx_osx_cocoau_gl` library) - ✅ Already installed via Homebrew
- **OpenGL framework** - ✅ Built into macOS
- **JuggleSaver rendering engine** - ✅ Included in source

### Using OpenGL Features

In the application:
- **View menu** - Switch between 2D and 3D rendering modes
- **Mouse controls** in 3D mode:
  - Left click + drag: Rotate camera
  - Right click + drag: Pan camera
  - Mouse wheel: Zoom in/out
  - Double-click: Reset camera
- **Options menu** - Enable auto-rotate and colored balls

## Build Status

✅ Successfully builds on Apple Silicon (ARM64) with wxWidgets 3.3.1

### Compatibility Fixes Applied

The following changes were made to support modern wxWidgets 3.x and enable OpenGL:

1. **Command line parsing** - Updated `wxCmdLineEntryDesc` to use plain string literals instead of deprecated `wxT()` and `_T()` macros
2. **wxBitmap resizing** - Replaced deprecated `SetWidth()`/`SetHeight()` with bitmap recreation
3. **File dialog flags** - Changed `wxSAVE` to `wxFD_SAVE`
4. **List iterators** - Updated `HandlerList::Node` to `HandlerList::compatibility_iterator`
5. **String conversion** - Fixed `mb_str()` usage with proper `.data()` method
6. **wxGLCanvas modernization** - Updated to use `wxGLContext` and modern OpenGL canvas API
7. **JuggleSaver integration** - Added `JUGGLESAVER_SUPPORT` define to enable `JMLibWrapper` for OpenGL rendering
8. **OpenGL context initialization** - Moved `jmlib->initialize()` to `initializeGL()` method called after context is current (**critical fix for white juggler/missing objects on startup**)
9. **Command-line pattern deferral** - Defer setting command-line pattern until after OpenGL initialization (**critical fix for crash when passing siteswap argument**)
10. **File pointer initialization** - Initialize to NULL (robustness: prevents undefined behavior)
11. **File pointer rewinding** - Uncommented `rewind()` calls after database init (**critical fix for startup crash**)
12. **App bundle resource loading** - Native macOS `_NSGetExecutablePath()` (robustness: more reliable than wxStandardPaths during startup)
13. **Makefile updates** - Copy `moresites.txt`, removed deprecated Mac OS Classic resource flags, added comprehensive OpenGL support configuration

## Troubleshooting

### Architecture Issues

- Ensure wxWidgets is built for ARM64 (Apple Silicon)
- The build system uses `wx-config` which should automatically handle architecture detection
- Built binary should be `Mach-O 64-bit executable arm64`

### Missing Libraries

If you get linker errors about missing libraries:
- Verify wxWidgets installation: `wx-config --version`
- Check sqlite3: `which sqlite3`

### Compilation Errors

If you encounter C++ compatibility errors, ensure you're using a modern C++ compiler (Xcode Command Line Tools on macOS).

### Black Screen or White Juggler in OpenGL Mode

✅ **Fixed**: OpenGL context initialization issue resolved.

**Previous symptom**: White juggler with no balls on startup, fixed after toggling 3D mode off and on.

**Root cause**: `jmlib->initialize()` was being called in the `JMOpenGLCanvas` constructor before the OpenGL context was made current. This resulted in incomplete OpenGL state initialization, causing objects not to render and the juggler to appear white.

**The fix**: Moved `jmlib->initialize()` from the constructor to a dedicated `initializeGL()` method that is called immediately after canvas creation. This method:
1. Calls `SetCurrent(*m_context)` to activate the OpenGL context
2. Calls `jmlib->initialize()` with the context current
3. Sets the rendering mode to 3D
4. Enables auto-rotate

This ensures all OpenGL-dependent initialization happens with an active rendering context.

If you still experience rendering issues:
1. Ensure `JUGGLESAVER_SUPPORT` is defined in `src/jmlib/Makefile` (automatically set when `OPENGL_SUPPORT=1`)
2. Verify that `jmlib_wrapper.o` is included in `libjm.a`: `ar -t src/jmlib/libjm.a | grep wrapper`
3. Try toggling the "3D Mode" option in the Options menu to switch between 2D and 3D rendering

### Crash When Passing Siteswap on Command Line

✅ **Fixed**: Crash when launching with a siteswap argument (e.g., `JuggleMaster "531"`) has been resolved.

**Root cause**: When a siteswap was passed as a command-line argument, `jmlib->setPattern()` was called in `JMApp::OnInit()` before the `JMFrame` was created and before OpenGL was initialized. This caused `setPattern()` → `resetCamera()` → `trackball operations` to crash because the trackball state hadn't been initialized yet.

**The fix**: Deferred setting the command-line pattern until after the frame is shown and OpenGL is initialized. The pattern is now stored during command-line parsing and applied via `JMFrame::setInitialPattern()` after `frame->Show()` returns.

### Segfault on Startup (Pattern Loading)

✅ **Fixed**: The crash during pattern file loading has been resolved.

**The Critical Fix**: After `initializeDatabase()` reads pattern files to populate the SQLite database, `rewind()` is now called to reset file pointers before `ParseAllPatterns()` reads them again. The original code had these rewind calls commented out.

**Additional Robustness Improvements**:
1. **Null pointer initialization** - File pointers initialized to NULL to prevent undefined behavior
2. **Null check before parsing** - Verify file opened successfully before attempting to parse
3. **Native macOS path resolution** - Uses `_NSGetExecutablePath()` to find app bundle Resources directory reliably during startup (more robust than wxWidgets path APIs during early initialization)

The app now launches successfully from any directory!

### Pattern Files

If patterns don't load, the app will attempt to download them to `$HOME/.jugglemaster/`. You can also manually copy pattern files from `data/*.jm` to this location.

## Configuration

Edit `Makefile.cfg` in the project root to:
- Enable ffmpeg support (set `HAVE_FFMPEG=1`)
- Change installation directory
- Enable debug mode

## Cleaning Build Artifacts

To clean the build:

```bash
cd src/jmdlx
make clean
```

This will remove object files, binaries, and the JuggleMaster.app bundle.

## Modified Files for Apple Silicon Compatibility

The following files were modified to support modern wxWidgets 3.x and Apple Silicon:

### jmdlx (Main Application)
- `src/jmdlx/jmdlx.h` - Updated command line parsing structure; added `setInitialPattern()` method
- `src/jmdlx/jmdlx.cpp` - Fixed string handling and bitmap operations; **critical fix**: defer command-line pattern setting until after OpenGL initialization to prevent trackball crash
- `src/jmdlx/print.cpp` - Updated file dialog and list iterator usage
- `src/jmdlx/patt.cpp` - **Critical fix**: uncommented `rewind()` calls; added null pointer initialization; native macOS path resolution
- `src/jmdlx/opengl.h` - Added wxGLContext member; added m_initialized flag; added initializeGL() method
- `src/jmdlx/opengl.cpp` - Updated wxGLCanvas API; **critical fix**: moved jmlib->initialize() from constructor to initializeGL() so it runs with active OpenGL context
- `src/jmdlx/Makefile` - Added OpenGL support, copy moresites.txt, removed deprecated resource handling

### jmlib (Core Library)
- `src/jmlib/Makefile` - Added `JUGGLESAVER_SUPPORT` define and JuggleSaver/wrapper object files for OpenGL builds
- `src/jmlib/jugglesaver/js_patterns.cpp` - Fixed pointer comparison bug

## Notes

- This is based on the wxWidgets port by Gary Briggs
- Original JuggleMaster was written by Ken Matsuoka
- jmlib (C++ library) by Per Johan Groland

## WebAssembly Port (jmwasm)

Requires Emscripten (`brew install emscripten`).

```bash
cd src/jmwasm
make          # builds jmwasm.js/jmwasm.wasm and copies them to ui/public/
make serve    # serves the demo page (jmwasm.html)
```

Rendering variants of the shared jmlib renderer:
- `USE_SHADERS=0 make osx` in `src/jmdlx` — legacy fixed-function pipeline (default)
- `USE_SHADERS=1 make osx` in `src/jmdlx` — shader/VBO pipeline (GLSL 1.20)
- jmwasm always uses the shader pipeline (GLSL ES 3.00 / WebGL 2.0)

The React UI lives in `src/jmwasm/ui`. See its [development rules](src/jmwasm/ui/DEVELOPMENT_RULES.md).

## Visual Regression Tests

Playwright screenshot tests for the shared renderer, driven through the wasm build:

```bash
cd src/jmwasm && make   # tests run against the current wasm build (symlinked)
cd ../../tests
npx playwright test
```

Screenshots are deterministic: the harness pauses the animation loop, pins the
engine clock to 60 fps, and steps a fixed number of frames. After an intentional
rendering change, regenerate baselines with `npx playwright test --update-snapshots`
and inspect the new PNGs before committing.
