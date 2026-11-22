# JuggleMaster WebAssembly Port

A minimal WebAssembly port of JuggleMaster for validating OpenGL ES 3.0 rendering in the browser using WebGL 2.0.

## Prerequisites

- Emscripten SDK installed (`brew install emscripten` on macOS)
- Python 3 (for local web server)
- Modern web browser with WebGL 2.0 support

## Building

```bash
cd src/jmwasm
make
```

This will generate:
- `jmwasm.html` - Main HTML page
- `jmwasm.js` - JavaScript glue code
- `jmwasm.wasm` - WebAssembly binary

## Running

```bash
make serve
```

Then open http://localhost:8000/jmwasm.html in your browser.

## URL Parameters

You can control the pattern and style via URL query parameters:

- `pattern` - Siteswap pattern (e.g., `3`, `441`, `5`)
- `name` - Pattern name (e.g., `3 Cascade`, `Mills Mess`)
- `style` - Juggling style (e.g., `Normal`, `Mills Mess`, `Reverse`)

### Examples

```
http://localhost:8000/jmwasm.html?pattern=3&name=3%20Cascade&style=Normal
http://localhost:8000/jmwasm.html?pattern=441&name=Mills%20Mess&style=Mills%20Mess
http://localhost:8000/jmwasm.html?pattern=5&name=5%20Cascade&style=Reverse
```

## Architecture

- `jmwasm.cpp` - Emscripten bindings and main entry point
- `index.html` - Minimal HTML wrapper (embedded in build output)
- Uses existing `jmlib` code with OpenGL ES 3.0 rendering
- WebGL 2.0 provides OpenGL ES 3.0 API in the browser

## Troubleshooting

### Build Errors

Make sure Emscripten is in your PATH:
```bash
which emcc
```

If not found, run:
```bash
brew link emscripten
```

### Browser Errors

Check browser console (F12) for WebGL errors. Make sure your browser supports WebGL 2.0:
- Chrome 56+
- Firefox 51+
- Safari 15+
- Edge 79+

## Development

To rebuild after changes:
```bash
make clean
make
```

The build will automatically include changes from `../jmlib/`.
