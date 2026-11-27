# JuggleMaster for Apple Platforms (tvOS / iOS / iPadOS / macOS)

## Overview

Native Swift/SwiftUI port of the jmwasm React UI (`src/jmwasm/ui`) to Apple TV, iPhone, iPad and Mac,
calling into the shared C++ core (`src/jmlib`). **Apple TV is the highest priority.**

The old `src/jmtouch` (iOS) and `src/jmcocoa` (Mac) ports are abandoned; nothing is reused from them.

Primary goal: feature parity with the web UI, adapted per platform (10-foot/remote-only UI on tvOS,
compact layout on iPhone).

Rendering goal (from AGENTS.md): output must be visually IDENTICAL to the legacy renderer, i.e. identical
to jmwasm and jmdlx `USE_SHADERS=1`.

### Decisions

| Topic | Decision |
|---|---|
| Graphics | OpenGL first (ES 3.0 on iOS/tvOS, desktop GL 2.1 + GLSL 1.20 on macOS). Metal renderer as a follow-up (Phase 7). |
| Minimum OS | iOS / iPadOS 18, tvOS 18, macOS 15 |
| Language / UI | Swift 6, SwiftUI, `@Observable` models |
| Swift ↔ C++ | Plain C API (`jm_capi.h`) shared with jmwasm, imported via a module map. No Swift C++-interop mode. |
| Project | XcodeGen (`project.yml`), generated `.xcodeproj` not committed |
| macOS | Native SwiftUI macOS (not Catalyst: Catalyst has no OpenGL ES) |
| tvOS pattern browser | Live preview of the focused pattern: **in** |
| Localization | English + Norwegian Bokmål (`nb`), mirroring the web `en` / `no` strings |
| Web app | `https://jugglemaster.net/` — target of share links / QR codes; host for universal links |

### Findings that shape the plan

* `jmwasm.cpp` is almost entirely platform-neutral: ~40 `jm_*` functions over the `JMLib` facade. Only
  WebGL context creation, the rAF loop and `EMSCRIPTEN_KEEPALIVE` are web-specific.
* The only GLES include point in jmlib is `jugglesaver/gl_includes.h` (`<GLES3/gl3.h>`). Apple's header
  is `<OpenGLES/ES3/gl.h>`. With that redirected, **all 14 jmlib sources of the wasm build compile cleanly
  against the tvOS 27.2 SDK** (verified with `clang++ -fsyntax-only -target arm64-apple-tvos18.0
  -DUSE_OPENGL_ES -DUSE_SHADER_RENDERING -DJUGGLESAVER_SUPPORT`).
* There are no `__EMSCRIPTEN__` branches in jmlib.
* jmlib never binds framebuffer 0 and sets its own viewport from `setWindowSize`, so it renders into
  whatever FBO the host binds (required for `CAEAGLLayer`). The host FBO needs a depth attachment.
* OpenGL ES is deprecated but still present in the iOS / tvOS 27.2 SDKs.
* jmdlx `USE_SHADERS=1` is *not* GLES: it is desktop GL 2.1 with the `#version 120` shaders in
  `shaders.h`. The ES 3.0 path is only exercised by wasm today. iOS/tvOS will run the ES path (same as
  web); macOS runs the GLSL 1.20 path (same as jmdlx).
* `render_shader.cpp` keeps renderer and camera state in file-level statics (`gPhong`, `extraZoom`, mesh
  VBOs, ...), so **one engine instance per process**. Fine for this app; to be removed in Phase 7.
* Core licences (jmlib BSD; JuggleSaver/trackball permissive) are App Store compatible. `jmwasm.cpp` is
  GPL but authored by the project owner; `jm_capi.cpp` should carry the jmlib (BSD) licence.

---

## Architecture

```
┌──────────────────────────────────────────────────────────────────────────┐
│ Apps (src/jmapple)                                                       │
│  JuggleMaster (iOS / iPadOS / macOS)      JuggleMaster TV (tvOS)         │
│  - platform UI (SwiftUI)                  - 10-foot UI, focus engine      │
│  - GLRenderView (UIKit / AppKit)          - remote / game controller      │
└──────────────────────────────────────────────────────────────────────────┘
                 │                                   │
                 ▼                                   ▼
┌──────────────────────────────────────────────────────────────────────────┐
│ Shared Swift (src/jmapple/Shared)                                        │
│  - JugglerModel (@Observable, single source of truth, calls jm_*)         │
│  - GLRenderView host (context, FBO, display link)                        │
│  - Shared views: SiteswapOverlay, CameraPresetsBar, ZoomControls,        │
│    ObjectTypePicker, RecentList, QR share                                 │
└──────────────────────────────────────────────────────────────────────────┘
                 │                                   │
                 ▼                                   ▼
┌───────────────────────────────┐   ┌──────────────────────────────────────┐
│ JMKit (local Swift package)   │   │ JMLib (static library target)        │
│ pure Swift, `swift test`able: │   │ jmlib C++ sources (wasm source list) │
│ - PatternLibrary (parseJm,    │   │ + jm_capi.{h,cpp}                    │
│   parseJs, filter)            │   │ module map `CJMLib` → jm_capi.h      │
│ - RecentStore                 │   └──────────────────────────────────────┘
│ - AppState URL codec          │                     ▲
└───────────────────────────────┘                     │ same C API
                                    ┌──────────────────────────────────────┐
                                    │ jmwasm.cpp: WebGL context + rAF only │
                                    └──────────────────────────────────────┘
```

Directory layout:

```
src/jmapple/
  project.yml                 XcodeGen spec (generated .xcodeproj not committed)
  Makefile                    `make help`: project, build-*, test-*, lint, format, install-tv
  JMKit/                      Swift package: pattern library, recents, app state (+ tests, TS parity fixture)
  JugglerKit/                 framework: CJMLib module map, Engine, GL hosting, JugglerModel, shared views,
                              Resources/Localizable.xcstrings (+ JugglerKitTests: engine, model, render parity)
  App/                        iPhone / iPad / Mac app
  TV/                         tvOS app
  AppUITests/ TVUITests/ MacUITests/
  Resources/                  InfoPlist.xcstrings, PrivacyInfo.xcprivacy
  scripts/                    xcodebuild output filter, string catalog helper
                              (patterns.jm / moresites.txt referenced from data/, not copied)
```

---

## Phase 0 — Shared C API in jmlib

Move the platform-neutral part of `jmwasm.cpp` into jmlib so wasm and Swift call the identical API and
engine-switching logic.

* [x] Create `src/jmlib/jm_capi.h` / `jm_capi.cpp`
  * `extern "C"`, C-only types in the header (int/float/const char*), so it imports into Swift as-is
  * `JM_API` macro: `EMSCRIPTEN_KEEPALIVE` under `__EMSCRIPTEN__`, `__attribute__((visibility("default")))` otherwise
  * Keep the existing function names and signatures so `EXPORTED_FUNCTIONS`, `wasm.d.ts`, the React UI,
    the legacy `index.html` UI and the Playwright harness are unaffected
  * Move: pattern/style/speed, render mode, render debug, trackball, orbit, autorotate, zoom, camera
    angle/presets, object type, site + sitepos, show-siteswap flag, the three validators, fixed frame
    rate, step frames
  * Split `jm_init`: `jm_capi_init(width, height)` (engine alloc + defaults, requires a current GL
    context) in jm_capi; WebGL context creation + `emscripten_request_animation_frame_loop` stay in
    `jmwasm.cpp`
  * Paused state moves into jm_capi (`jm_set_paused` / `jm_get_paused`); hosts call a single
    `jm_frame()` per display refresh
* [x] New functions needed by the native UI
  * `jm_draw()` — render without advancing the animation, so orbit/zoom/resize redraw while paused
    (tvOS Play/Pause + orbit to study a throw)
  * `jm_get_paused()`
  * `jm_get_pattern_name()`, `jm_get_style()` — state restore / share without shadow state in Swift.
    The name is kept in jm_capi: the wrapper hands JuggleSaver the pattern text as its name
  * ~~`jm_shutdown()`~~ — dropped: the shader renderer caches GL objects in process-wide statics, so hosts
    keep one GL context for the process lifetime instead (see Phase 1)
  * Document in the header which calls require the GL context to be current (all of them, to be safe)
* [x] `jugglesaver/gl_includes.h`: `USE_OPENGL_ES && __APPLE__` → `<OpenGLES/ES3/gl.h>` +
  `<OpenGLES/ES3/glext.h>`; keep `<GLES3/gl3.h>` for wasm
* [x] `jmwasm/Makefile`: add `jm_capi.cpp` to `JMLIB_SOURCES`
* [x] Verification
  * `make` in `src/jmwasm`; `npm run lint` + `npm run build` in `src/jmwasm/ui`
  * Playwright visual suite (`tests/`) passes with **unchanged** baselines
  * Smoke test of the React UI (JuggleSaver pattern from URL, no console errors)
  * `jm_capi_init` consumes the wrapper's first-frame engine reset, so a pattern set right after init is no
    longer switched back to JuggleMaster by the first rendered frame
  * `jmwasm/Makefile` links with `em++` (emcc 7 no longer links libc++ for `emcc`)
  * The rAF loop calls `jm_frame()`, so a paused web canvas still redraws on orbit/zoom

## Phase 1 — Project skeleton and rendering spike

Goal: "3 Cascade" juggling on all three platforms, pixel-compared against jmwasm. De-risks GL hosting and
tvOS input before any UI work.

* [x] XcodeGen `project.yml` (`make project`)
  * `JMLib` static library (iOS / tvOS / macOS): the jmwasm `JMLIB_SOURCES` list + `jm_capi.cpp`;
    `JUGGLESAVER_SUPPORT USE_SHADER_RENDERING` everywhere, `USE_OPENGL_ES GLES_SILENCE_DEPRECATION` for
    `sdk=iphone*` / `sdk=appletv*`, `GL_SILENCE_DEPRECATION` for macOS; `-fsigned-char`; C++17
  * `JugglerKit` framework: Swift bridge (`Engine`), GL hosting, SwiftUI views; imports the `CJMLib`
    module map (`jm_capi.h`) with `internal import`
  * `JuggleMaster` app (iPhone, iPad, Mac) and `JuggleMasterTV` app (tvOS), both bundle id
    `net.jugglemaster.JuggleMaster`; `data/patterns.jm` and `data/moresites.txt` as resources
  * Team `637V48V963`, automatic signing in `project.yml`
* [x] `src/jmapple/Makefile`: `project`, `build[-tv|-ios|-mac]`, `test[-kit|-mac|-tv|-ios]`, `lint`, `format`,
  `install-tv`; `scripts/xcfilter.sh` condenses xcodebuild output
* [x] `.gitignore`: generated `.xcodeproj`, `build/`
* [x] One process-wide GL context (`GLContext.shared`): the renderer caches GL objects in statics, so views
  never create their own context
* [x] `GLRenderView` (iOS / tvOS): `CAEAGLLayer`, ES 3 context, 4× MSAA target resolved into the layer
  renderbuffer, screen scale, `jm_resize` in pixels, `CADisplayLink` up to 120 Hz, stops + `glFinish()` when
  the app resigns active
* [x] Layout while inactive is deferred until the app is active again (no GL work in the background); the
  macOS app uses a single `Window` because the engine is process-wide
* [x] `GLRenderView` (macOS): `NSOpenGLView` subclass sharing the legacy 2.1 context (GLSL 1.20 path),
  same MSAA target resolved into the default framebuffer, `NSView.displayLink`. `NSOpenGLContext.view` /
  `wantsBestResolutionOpenGLSurface` are deprecated without a silencing macro, so `NSOpenGLView` handles
  attachment (Retina backing is its default for apps linked against current SDKs)
* [x] SwiftUI wrapper `JugglerView`
* [x] Antialiasing parity: 4× MSAA matches the (antialiased) WebGL canvas within the parity tolerance
* [x] Render parity tests (`JugglerKitTests/RenderParityTests`, macOS = GLSL 1.20 path, tvOS simulator = ES
  path): harness sequence of `tests/visual`, 800×600, 120 frames at a pinned 60 fps, compared with the
  Playwright PNGs; ≤ 1 % of pixels may differ by more than 48 per channel and the count of colored (object)
  pixels must be within 10 %. All 14 baselines (7 patterns × 2D/3D) pass on both paths; the 2D cases render a
  3D frame first, so they also cover switching away from the 3D renderer. On macOS 2D is the fixed-function
  jmdlx path (`GL_LINE_SMOOTH`, visibly bolder lines than the web)
* [x] Desktop 2D fix found by the parity tests: the 3D shader renderer leaves its program, VBO and generic
  vertex attributes bound, which broke the fixed-function 2D paths (JuggleMaster and JuggleSaver) after a 3D
  frame (macOS app and jmdlx `USE_SHADERS=1`). `render_jm_flat.cpp` resets them before drawing
* [x] Retina: kept native resolution. 2D lines stay 1 physical pixel (thin at 2×/4K, where a browser
  upscales the 1× canvas to 2 px); revisit after trying it on the TV
* [x] tvOS input decision (no physical remote in the loop; verified in the simulator, tried on the TV in
  Phase 3): a focusable UIKit input view over the renderer with a `UIPanGestureRecognizer` restricted to
  indirect touches for orbit, `pressesBegan` for select / play-pause / arrow clicks, a long-press recognizer
  on select, and `.menu` passed to `super` so Back exits to Home. With no other focusable views in watching
  mode, swipes don't move focus, so pan and arrow clicks don't double-fire. `GCMicroGamepad` is not needed
* [x] Verified: tvOS / iOS simulator and macOS builds, app renders in the tvOS simulator, tests green on
  macOS and the tvOS simulator. Real Apple TV: Phase 3

## Phase 2 — Shared Swift core

* [x] **JMKit: PatternLibrary** (port of `ui/src/lib/patterns`)
  * `Pattern`, `Category` (incl. `source: jm | js`), `PatternLibrary` (`search` = `filterPatterns`,
    `categories(for:)`, `patterns(in:)`, `Pattern.isJuggleSaver` / `effectiveStyle`)
  * Identical semantics including the slug-based **ids** (the share URL's `cat` param matches the web app)
  * Parity test: `src/jmwasm/ui/scripts/dump-patterns.mjs` bundles the TS parsers with the existing
    esbuild and dumps the library plus sample searches to `JMKit/Tests/JMKitTests/Fixtures/library.json`;
    the Swift output for the real `patterns.jm` / `moresites.txt` is identical (42 categories, 1284 patterns)
* [x] **JMKit: RecentStore** (port of `useRecentItems`)
  * Siteswaps `{site, style, favorite}`, patterns `{name, data, style, categoryId, categoryName, favorite}`;
    max 10; favorites first; dedupe rules and storage keys as on the web; `UserDefaults` JSON
  * Deliberate difference: re-adding an entry that is a favorite keeps it a favorite (the web drops the flag
    when the library pushes a fresh object)
* [x] **JMKit: AppState URL codec** (port of `useUrlParams`)
  * Same params, defaults, validation and omission rules; form-encoding as `URLSearchParams` writes it
  * Base URL for share links / QR codes: `https://jugglemaster.net/`
  * Reads web share links and `jugglemaster://` deep links (`+` and `%20` spaces)
  * Round-trip tests + tests against URLs produced by the web app
* [x] **JugglerModel** (`@Observable @MainActor`, in Shared) — single source of truth; replaces the
  React state + `setTimeout` syncing (C calls are synchronous)
  * State: current pattern (name/site/fromLibrary/categoryId), style, preferred engine, actual engine,
    render mode, object type, speed, autorotate, show siteswap, zoom, paused, siteswap info
    (site/start/stop)
  * Actions mirroring `Menu.tsx` / `PatternTabContent.tsx` / `Canvas.tsx`, preserving these rules:
    * Library pattern: `jm_set_pattern` **then** `jm_set_style` (setPattern re-selects the engine);
      JuggleSaver if `source == js` or category name contains "JuggleSaver"
    * Siteswap submit: validate with `jm_is_valid_pattern` first (no-op if invalid); after setting,
      if actual engine ≠ preferred engine, apply `JuggleSaver` / selected style and re-read the engine
    * Engine toggle: apply `JuggleSaver` or the selected JM style; re-read actual engine; show the
      warning state when preferred ≠ actual
    * Style picker disabled while the actual engine is JuggleSaver
    * Selecting a pattern that lands in JuggleSaver resets object type to Default; JM keeps the user's
      choice; in JM, Default renders and displays as Balls
    * Orbit disables autorotate (once per drag); double-tap / equivalent toggles autorotate; camera
      presets disable autorotate
    * Speed range 0.1–10, step 0.1, default 3
    * Live validation exposes `isValid` (an engine will juggle it), `juggleMaster`, `juggleSaver`
    * A pattern no engine accepts leaves state untouched (`jm_set_pattern` reports acceptance); a rejected
      restored/deep-linked pattern falls back to 3 Cascade
  * Per-frame poll (from the display link, after `jm_frame()`): site/start/stop and zoom; publish only
    on change (replaces the web's rAF poll and 500 ms zoom interval)
  * Apply initial state at startup in the same order as `Canvas.tsx`: object type, pattern, style, mode,
    autorotate, speed (`GLRenderView.onStart` → `engineDidStart()`)
  * Last state saved as a query string (`jm_last_state`); a deep link's state wins over it
  * Deliberate difference: applying a JuggleMaster library/recent pattern also updates the style picker
    when the style is one of the picker's (the web leaves the picker showing the previous style); custom
    library styles such as "Tennis" render as Normal in the engine and leave the picker unchanged
  * Slider speed changes are saved once they settle (also covers keyboard and VoiceOver adjustments)
  * Tests: `JugglerKitTests/JugglerModelTests` against the real engine on macOS and the tvOS simulator
* [x] **Localization**: `Localizable.xcstrings`, en + nb, keys mirroring the web (`menu.enterSiteswap`,
  `zoom.zoomIn`, ...). Also localize what the web hardcodes: "Recent", Favorite/Unfavorite/Remove,
  camera preset labels. Native-only strings are added with the platform UIs that use them
  (`scripts/add-strings.py`); ambient mode is Phase 6
* [x] **Shared views**
  * `SiteswapOverlay`: monospaced `Text` from an `AttributedString` (white / `#ff4444` active throw /
    white) on a `rgba(0,0,0,0.45)` rounded background; font size per platform
  * `CameraPresetsBar` (Left, Back, Front, High, Right → presets 1, 2, 0, 4, 3)
  * `ZoomControls` (step 0.05, disabled at min 0.01 / max 0.9 ± 0.005 like the web)
  * `ObjectTypePicker` with the four icons from `ObjectIcons.tsx` converted to SwiftUI `Shape`s
    (SVG paths → `Path`), plus a "Default" text button while JuggleSaver is active
  * `RecentSiteswapLabel` / `RecentPatternLabel` (row content; actions differ per platform and live with
    the platform UIs)
  * `ShareQRCode` (`CIFilter.qrCodeGenerator`, all platforms incl. tvOS)
* [x] `PrivacyInfo.xcprivacy` (UserDefaults is a required-reason API: `CA92.1`)

## Phase 3 — tvOS UI (priority)

Design principles: full-screen juggler by default, chrome only on demand, everything reachable with
the Siri Remote alone, the juggler stays visible behind panels, respect the TV safe area (~60/80 pt).

* [x] **Watching mode** (default; no visible chrome except the optional siteswap overlay)
  * Touch-surface swipe/drag → orbit (`jm_orbit_camera`, disables autorotate), per the Phase 1 spike
  * Click (select) → show the controls bar
  * Long press select → toggle autorotate (the web's double-click)
  * Play/Pause → pause / resume (`onPlayPauseCommand`); orbit still works while paused (`jm_draw`)
  * Arrow clicks left/right → previous / next camera preset; up/down → zoom in / out
  * Back (Menu) → **not handled** in watching mode, so the system returns to the Home screen (HIG)
  * A pause indicator shows while paused
* [x] **Controls bar** (bottom, material background, focus section)
  * Buttons: Patterns, Siteswap, Settings, Share, Camera, Zoom − / + / reset, Pause. Camera shows the current
    preset and opens a row of the five presets above the bar (five text buttons inline made the bar wrap
    on a real TV). Auto-rotate is in Settings and on a long press, not in the bar
  * Auto-hides after ~8 s without interaction; Back hides it
* [x] **Pattern browser** (left panel ~40% width, juggler visible on the right)
  * Header: source filter All | JM | JS, Search (outside the list: a tvOS list row is one focus target)
  * Sections: Recent (favorites first, starred), then categories → patterns (NavigationStack in the panel)
  * **Live preview**: focusing a pattern row applies it after a ~500 ms debounce, without touching
    recents; Select commits (adds to recents, closes the panel); Back from the browser without
    committing restores the pattern, style and object type active when the browser opened
  * Search: full screen (the tvOS search keyboard is unusable inside the side panel), `.searchable` with
    dictation and the iPhone Continuity Keyboard; results across the library with the source filter
    applied; Select juggles a result, Back returns to the browser
  * While a side panel is open the juggler renders in the space beside it, so previews are not hidden
  * Recent rows: context menu (long press) for Favorite / Unfavorite / Remove
* [x] **Siteswap entry** (panel)
  * Custom focusable keypad instead of the tvOS keyboard: `0–9`, `a–z`, `[ ] ( ) , . - @ >` and space
    (siteswap digits and brackets plus JuggleSaver's object letters and `@(x,y)>(x,y)` positions), plus
    delete, clear, Juggle; Juggle closes the panel to show the result
  * Entry field with live green / red state and JM / JS badges; Juggle enabled only when valid
  * "Keyboard" button: system text entry for JuggleSaver notation (also gets Continuity Keyboard)
  * Recent / favorite siteswaps list beside the keypad (same actions as patterns)
  * Engine indicator + warning when preferred ≠ actual
* [x] **Settings** (panel, tvOS `List`)
  * Mode 2D/3D, Engine JM/JS (with warning), Style (Picker; disabled under JuggleSaver), Autorotate,
    Show siteswap
  * Speed: − / + buttons around the value (one decimal); SwiftUI `Slider` is unavailable on tvOS
  * Mode and Engine are rows that show their value and switch on click; the style picker is a menu
  * Objects: horizontal focusable icon buttons (`ObjectTypePicker`)
* [x] **Share**: QR code of the web URL for the current state + the URL as text
* [x] Siteswap overlay sized for 10-foot viewing (~36–40 pt monospaced), inside the safe area
* [x] Focus styling (hover/lift effects), large type, consistent focus sections so the remote never
  lands on an invisible element
* [ ] App icon (tvOS layered image stack), launch screen — needs artwork; the generated launch screen is used
* [x] UI tests with `XCUIRemote` for the main flows (open browser, preview, commit, back-revert, keypad)

## Phase 4 — iPad and macOS

Closest to the web desktop layout.

* [x] `NavigationSplitView`: sidebar = web menu content, detail = juggler + overlays; sidebar toggle
  replaces the web's menu button
* [x] Sidebar content (shared view, also used by the iPhone sheet in Phase 5)
  * Segmented Siteswap | Pattern tab
  * Siteswap tab: text field (green/red, JM/JS badges), Style picker, Engine toggle with warning,
    recents with inline favorite/remove buttons (as on the web), a context menu, and swipe-to-remove on iOS
  * Pattern tab: search field, All | JM | JS filter, category picker + pattern list, search results,
    recents
  * Mode, Autorotate, Show siteswap toggles; Speed slider (value with one decimal; persist on commit);
    Objects picker
* [x] Juggler overlays at web positions: siteswap top center, presets bottom center, zoom + share top
  right
* [x] Gestures: drag → orbit; double tap/click → autorotate; pinch (`MagnifyGesture`) → zoom; Mac
  scroll wheel / trackpad scroll and pinch → zoom (handled by the AppKit view: SwiftUI gestures don't
  reach an `NSViewRepresentable` reliably)
* [x] Share: `ShareLink` (web URL) + QR popover
* [x] macOS commands / menu bar (Juggler menu): 2D/3D (⌘2 / ⌘3), camera presets (⌥⌘1–5), Zoom In/Out/
  Reset (⌘+ / ⌘− / ⌘0), Pause (⌥⌘P, and Space while the juggler has focus), Auto-rotate (⌥⌘R); single
  window with a minimum size; full-screen support. `MacUITests` covers typing a siteswap
* [x] iPad pointer hover effects; keyboard shortcuts shared with macOS
* [x] Stage Manager / Split View: below regular width switch to the compact (iPhone) layout via
  `horizontalSizeClass`
* [x] Deep link handling (custom scheme `jugglemaster://?<web query>`) using the AppState codec
  (`JugglerModel.load`)
* [x] The juggler respects the floating sidebar's leading safe-area inset, so it centers in what is visible
* [x] UI tests (`AppUITests`, iPad and iPhone simulators): typed siteswap, library pattern, share QR, deep
  link, iPhone landscape panel

## Phase 5 — iPhone

* [x] Compact layout: full-screen juggler; floating menu button (bottom left, like the web on mobile),
  presets bar bottom center, share bottom right; zoom by pinch with a small reset button instead of the
  three zoom buttons
* [x] Menu as a bottom sheet with detents (`.fraction(~0.45)`, `.large`) and
  `presentationBackgroundInteraction(.enabled(upThrough:))`, so the juggler stays visible and
  interactive while changing settings
  * Shrink the render view to the area above the sheet at the medium detent so the juggler stays
    centered in the visible part
  * Applying a pattern at the large detent closes the sheet (web mobile behaviour); at the medium
    detent it stays open
* [x] Siteswap field: ASCII keyboard, no autocorrect/capitalization, plus a keyboard accessory row with
  `[ ] ( ) , x` (characters the iOS keyboards bury) that inserts at the cursor / replaces the selection
* [x] One scene on iPad (`UIApplicationSupportsMultipleScenes = false`): the engine renders one view
* [x] Landscape: sheet becomes a leading side panel over the juggler
* [x] Safe areas (Dynamic Island, home indicator) for overlays
* [x] Haptics (selection) on camera preset, object type and 2D/3D changes

## Phase 6 — Polish and extras

* [ ] **Ambient mode** (all platforms, tvOS first): cycles random library patterns every N seconds,
  slow autorotate, no chrome, optional cross-fade; disables the idle timer only while active.
  Same feature as the "Screensaver / ambient mode" item in FEATURES.md
* [ ] tvOS **Top Shelf** extension (featured patterns / recents, deep links)
* [ ] iCloud key-value sync of favorites and recents (`NSUbiquitousKeyValueStore`), e.g. favorite on
  iPhone → available on Apple TV
* [ ] Game controller support (extended gamepad): sticks orbit/zoom, shoulder buttons cycle presets,
  A/B mapping consistent with the remote
* [ ] Pause + frame-step controls (FEATURES.md "nearly free" item; `jm_step_frames` exists)
* [ ] Universal links (static `/.well-known/apple-app-site-association` on jugglemaster.net) so shared web URLs open the
  native app when installed
* [ ] App Store preparation: icons for all platforms, screenshots, export compliance (no encryption),
  privacy manifest review

## Phase 7 — Metal renderer (follow-up)

Goal: native Metal rendering on all Apple platforms, visually identical to the GL paths, removing the
dependency on deprecated OpenGL / OpenGL ES.

* [ ] **Backend abstraction in jmlib** (benefits all platforms)
  * The GL surface of the shader path is small: buffers (gen/bind/data/subdata), three programs
    (phong, flat color, line), `glDrawArrays` / `glDrawElements`, clear, viewport, depth/blend state,
    1 px lines
  * Introduce a `RenderBackend` interface (C++): create/update mesh buffers, select pipeline, set
    uniforms (plain structs), draw, begin/end frame (clear color, viewport)
  * Move the existing GL ES / GL 2.1 code behind it; Playwright baselines must stay unchanged
  * Move the `render_shader.cpp` file-level statics into a renderer/context object (also lifts the
    one-engine-per-process limit, enabling e.g. pattern thumbnails)
* [ ] **Metal backend** (`jmlib/metal/`, Objective-C++ or metal-cpp)
  * MSL ports of the shaders in `shaders.h`
  * Clip-space depth: Metal z ∈ [0,1] vs GL [-1,1] → depth remap on the projection matrix
  * Winding: set `.counterClockwise` front face to match GL
  * Uniform layout: MSL `float3` is 16-byte aligned; use explicit padded structs shared via a header
  * Color: `CAMetalLayer` `.bgra8Unorm` (not `_srgb`) to match GL output; same MSAA sample count as
    the GL path
  * Lines: Metal line primitives are 1 px (same as GLES), no `glLineWidth` equivalent needed
* [ ] C API additions: `jm_init_metal(device)`, `jm_render_metal(commandBuffer, renderPassDescriptor)`;
  Swift owns `CAMetalLayer` / drawables / command queue
* [ ] `MetalRenderView` for iOS / tvOS / macOS replacing `GLRenderView`
* [ ] Parity: render the same pinned frames with GL and Metal offscreen; pixel diff within tolerance;
  also compare against the Playwright baselines
* [ ] Rollout: Metal behind a build setting → default once parity is verified → remove GL from the
  Apple targets

---

## Risks and open items

* Apple removing OpenGL ES from a future SDK → mitigated by Phase 7.
* tvOS remote input: swipe-to-orbit vs arrow clicks vs focus movement can conflict — resolved by the
  Phase 1 spike before building Phase 3.
* 2D mode on Retina / 4K (1 px lines, sizes tuned for 1× web canvas) — Phase 1 parity check.
* Antialiasing parity between web, jmdlx and native — Phase 1.
* The wrapper/engine unification refactor (FEATURES.md) is independent; the C API keeps it invisible to
  the apps, and Phase 7's backend abstraction can be combined with it.
