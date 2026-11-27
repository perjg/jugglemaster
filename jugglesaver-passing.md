# JuggleSaver: multi-juggler passing — spec

## Decisions

| Topic | Decision |
|---|---|
| Four-handed syntax | `J<n>:<site>`, e.g. `J2:966`. JuggleMaster rejects it, so the wrapper routes it to JuggleSaver. |
| 2D mode with J > 1 | World side view: profile stick figures, objects in world x/y. |
| Speed | Same personal tempo: `Time += speed · dt · J`. |
| Object colours | Per object, as today. |
| Scope | Phases 1+2, 3 and 4. Phase 5 (multiplex, walking) is out of scope. |
| Library category | `Passing (JuggleSaver)`, using the existing suffix rule. |
| Body colours | Juggler A keeps today's colour, the others get muted tints. A "Colour jugglers" setting turns them off; share links carry `jcolors=0` when off. |
| Formations | Automatic only: facing pair for J = 2, circle for J ≥ 3, line for feeds. No formation picker and no `formation` URL parameter. |

Notation conventions: in Prechac, `p` passes to the next juggler and `pN` to juggler N (1-based,
JugglingLab style). `<a b|c d>` is synchronous passing, one bar per juggler. A throw's object letter
and other Juggle Saver throw syntax come after the pass (`3p2c`, `4.5pc*2`). Every juggler starts its
sequence with the right hand.

As implemented, all passing notations and solo sync share one model: the interleaved `J<n>:` site
(slot s belongs to juggler s mod J) plus an integer time shift per hand, so Prechac with any period and
synchronous patterns use the same engine path (Phases 3 and 4 below describe the original two-step
plan).

Where the sections below list options that these decisions settle (formation picker, `formation` URL
parameter, open questions), the decisions above take precedence.


Scope: the JuggleSaver engine (`src/jmlib/jugglesaver/`) plus everything that has to change for a user to
see and share passing patterns: the wrapper, the C API (`src/jmlib/jm_capi.{h,cpp}`), the web UI
(`src/jmwasm/ui`), and the Apple apps (`src/jmapple`). The JuggleMaster engine stays solo. The wrapper sends a
pattern to whichever engine accepts it (`JMLibWrapper::setPattern`, `src/jmlib/jmlib_wrapper.cpp:317-411`).

Ground rules from `AGENTS.md` and `src/jmwasm/ui/DEVELOPMENT_RULES.md`, which apply to every phase:

* The legacy renderer (`render_legacy.cpp`) and the shader renderer (`render_shader.cpp`) must stay
  visually IDENTICAL. Every rendering change lands in both files in the same commit, with the same math in
  the same order.
* Run `make` in `src/jmwasm` after every C++ change. For web UI changes, also run `npm run lint`,
  `npm run build` and, after string changes, `npm run typesafe-i18n`.
* Every user-facing string goes through i18n: web `en` + `no` (`src/jmwasm/ui/src/i18n/{en,no}/index.ts`),
  Apple `en` + `nb` (`src/jmapple/JugglerKit/Resources/Localizable.xcstrings`). Ask the user before
  adding any new shadcn component.
* With J = 1 (every existing pattern), the output must be unchanged.

---

## 0. Verification of the briefing claims (against the code)

| Claim | Verdict | Evidence / correction |
|---|---|---|
| The hand cycle is hard-coded to 2 | **True** | Several places assume 2. Hold detection is `TimeInAir == 2` (`engine.cpp:204, 299, 614, 617, 634, 643`). Holds are coalesced with `Next += 2` (`:619`). `NextForHand` starts at `Wait = 2` and steps `Wait += 2` (`:642-646`), and the fallback is `NextForHand = 2` (`:658`). The default side is `i % 2` (`:539, :544`). The parser's default side is `Len % 2` (`:969, :988`). |
| Odd-length doubling | **True** | `InfoLen = Len % 2 == 1 ? Len * 2 : Len` (`engine.cpp:507`). For J jugglers this becomes `lcm(Len, 2J)`. |
| `LeftHand`/`RightHand` → hand array | **True, but the names are misleading** | `PATTERN_INFO.LeftHand` starts at slot 0 and `RightHand` at slot 1 (`jugglesaver.h:195-196`, `engine.cpp:701-702`). Slot 0 is side 0 and gets x < 0 (`GetDefaultFromPosition`, `engine.cpp:451-466`). The renderer draws `GetHandPosition(…, 0)` on the `-ShoulderPos[0]` shoulder (`render_legacy.cpp:898, 910`). The juggler faces +z, towards the default camera, so −x is the juggler's **anatomical right**. So slot 0 = anatomical right hand, and "LeftHand" means *viewer's* left. The shader comment "Left=1 is the right arm" (`render_shader.cpp:1036`) is also from the viewer's side. The four-handed convention "slot i mod 4: A right, B right, A left, B left" is therefore consistent with today's slot 0 = right. |
| CARRY_TIME/gravity must scale with J | **True** | `CARRY_TIME` is a macro (`jugglesaver.h:63`). It is used in `InterpolatePosition` (`engine.cpp:100`, the length cap of the spline reparameterisation), `InterpolateCarry` (`:172`), `GetHandPosition` (`:229`), `GetObjectPosition` (`:306`), `SetHeightAndAlpha` (`:430-431`) and the velocity setup (`:572`). All of these need a per-pattern `CarryTime`. |
| The parser ignores z | **True** | `ParsePositionText` maps the numbers to `x, y, Rot, Elev` (`engine.cpp:817-820`). z is only ever set to 0 by the defaults (`:465, :491`). Every existing pattern therefore has z ≡ 0 everywhere, including in the velocities. |
| Arm IK must run in the owner's local frame | **True, and there is a second, unlisted issue** | The IK in `DrawArm` (`render_legacy.cpp:888-926`) and `drawArmES` (`render_shader.cpp:1035-1068`) **ignores `Pos.z`**: it always uses `ShoulderPos[2]` as the hand-to-shoulder depth (`:898-899` / `:1040-1041`). Any hand that moves in z (passes) would detach from the forearm. The fix is to use `ShoulderPos[2] + localHand.z`. That is bit-identical when z = 0 (`1.7f + 0.0f == 1.7f`). |
| `render_legacy`'s `DrawArm`/`JMDrawGLScene` are also used by JuggleMaster 3D | **Partly** | JuggleMaster 3D uses `JMDrawGLScene` → `JMDrawJSJuggler` → `JMDrawArm` (`render_legacy.cpp:1123-1170, 1294+`), which is a *copy* of the IK, not `DrawArm` itself. It also shares the display lists and `SetCamera`: the wrapper calls `js->setPattern(high)` to frame JuggleMaster patterns (`jmlib_wrapper.cpp:372-382`). So `SetCamera` and the display lists must stay byte-identical for J = 1. The shader path has the equivalent `drawJMJugglerES` (`render_shader.cpp:1310`). |
| Library: "data/patterns.jm — check how JuggleSaver patterns are stored there" | **Wrong file** | JuggleSaver patterns are in **`data/moresites.txt`**, in the format `$ Group` / `"Name (Author)" pattern;` (see the header and `$ Club patterns` at line 329). It is parsed by `js_patterns.cpp:ParseJSPatterns`, web `src/jmwasm/ui/src/lib/patterns/parseJs.ts`, and Apple `JMKit/Sources/JMKit/PatternParsers.swift:83`, and bundled by `src/jmapple/project.yml:124,155` and `src/jmwasm/ui/public/moresites.txt`. `patterns.jm` holds JuggleMaster patterns only. |
| "JuggleMaster refuses passing patterns; the wrapper routes to whichever accepts" | **True with a trap** | **Four-handed siteswap such as `966` or `786` is also a valid solo vanilla siteswap.** The wrapper checks `validateVSS` first and gives VSS patterns to JuggleMaster (`jmlib_wrapper.cpp:318-331`, `jm_capi.cpp:jm_is_valid_pattern`). Four-handed input therefore **needs an explicit marker** that JuggleMaster rejects (see 1+2.1). |
| `transSyncMSS` converts sync to async | **Partly** | `transSyncMSS` (`validator.cpp:394-448`) only rewrites the throw values (`x` crossing ±1). The beat shift of the second hand is done by the JuggleMaster engine at juggle time, not by the validator. The JuggleSaver equivalent is a per-hand time shift (Phase 4). Note that `isSSS` routes `(…,…)` to JuggleMaster (`jmlib_wrapper.cpp:320-331`). Solo sync in JuggleSaver therefore only applies to patterns JuggleMaster rejects, such as ones with JS object or position syntax, or when the style is set to `JuggleSaver`. |
| The overlay uses `jm_get_site` + sitepos start/stop | **True** | Web: `hooks/useSiteswapInfo.ts`, `components/SiteswapOverlay.tsx`. Apple: `Engine.swift:94-98` (`siteRange`). JuggleSaver reports `(int)Time % strlen(siteswap)` (`jmlib_jsaver.cpp:252-266`), which assumes **one character per beat**. That holds for four-handed siteswap but not for Prechac (`3.5p`) or `<…|…>`. |
| JuggleSaver also renders in 2D | **Unlisted** | In 2D mode the wrapper uses `JMFlatOpenGL::renderJS` (`render_jm_flat.cpp:524-631`). It calls `GetHandPosition(…, is_left)` and draws a single frontal stick figure. The Playwright and Apple parity baselines include **2D** captures of every JS pattern. 2D must be decided for J > 1 (see 1+2.5). |
| Visual baselines guard J = 1 | **True, but only for the shader path** | Seven patterns are captured in 3D and 2D (`tests/visual/visual.spec.ts:28-34`, mirrored in `src/jmapple/JugglerKit/Tests/RenderParityTests.swift:31-39`). Both use the wasm/ES (shader) renderer. **Nothing automated covers the legacy renderer.** A J = 1 engine-level golden test (Phase 1+2) closes that gap for the engine. Legacy rendering still needs a manual jmdlx check (`src/jmdlx/run-legacy.sh` vs `run-new.sh`). |
| Pre-existing quirks to keep in mind | — | `PrevThrow` is first written with `% Len`, not `% InfoLen` (`engine.cpp:533`); pass 2 repairs it (`:625`). The validator and the parser disagree on lowercase throw values: `siteDigit('a') = 35` (`js_validator.cpp:28-29`) but `Weight = 'a' + 10 - 'A' = 42` (`engine.cpp:915`). The web camera preset labels are hard-coded English (`components/CameraPresets.tsx:9-15`, an existing i18n violation). There is no C++ unit-test harness, only `*_STANDALONE` mains. |

---

## Data-model decisions shared by all phases

1. **Interleaved slot model (Phase 1+2).**
   * The pattern is one int array of slots. Each slot is `1/J` of a juggler beat.
   * Slot `s` belongs to hand `h = s mod 2J`, juggler `j = h mod J`, and side `side = (h / J) mod 2` (0 = right, 1 = left).
   * A throw of value `t` from slot `s` lands at slot `s + t`.
   * A throw is a pass iff `t mod J != 0`. Otherwise it is a self with solo-equivalent weight `t / J`.
   * J = 1 reduces exactly to today's `i % 2` side.
2. **Time unit = slot.**
   * `CarryTime = CARRY_TIME * J`.
   * Gravity is computed from solo-equivalent weights, so `Alpha` (in slot units) = `Alpha_solo / J²`.
   * The wrapper advances `state.Time += JuggleSpeed * dt * J`, so each juggler keeps the same personal rhythm as a solo juggler at the same speed setting.
3. **World space in the engine.**
   * Throw and catch positions are authored in the juggler's local frame (today's frame: faces +z, hand plane at z = 0, torso at z = −1.7).
   * `InitPatternInfo` converts them to world space with the owner's placement: `world = T(px, 0, pz) · R_y(facing) · local`, and `Rot_world = Rot_local + facing`.
   * Ball `Rot` is a twist angle about a fixed axis, not a yaw, so facing is **not** added to balls (`render_legacy.cpp:1013`, `GetObjectPosition` overrides `Rot` for balls, `engine.cpp:326-327, 381-382`).
   * All flight and carry math then stays unchanged. Renderers draw objects in world space exactly as today and transform only the bodies.
4. **J = 1 guard.** Every new branch keys on `pPattern->Jugglers > 1` or an identity placement, so existing patterns take the existing code path. In addition, a golden-data test proves bit-equality (see tests).

---

## Phase 1+2 — Generalise the engine to J jugglers + four-handed siteswap

**Goal.** Make `J2:966`-style four-handed siteswap juggle, with two (or more) stick figures passing in both
renderers, while every existing pattern stays pixel-identical.

### 1+2.1 Notation, parsing, validation

* **Marker syntax.** Input looks like `J<n>:<site>`, for example `J2:966`, `J2:7`, `J2:786`, `J3:a` (letters
  as today, `A`=10). It accepts the full existing JS token syntax after the colon (object letters,
  `@(…)>(…)`, `/`, `*n`).
  * JuggleMaster rejects it, because `siteDigit(':') == -1` (`validator.cpp:178-190`), so the wrapper and
    `jm_is_valid_pattern` route it to JuggleSaver.
  * Positions in a passing pattern are interpreted as **thrower-local** for `@(…)` and **catcher-local**
    for `>(…)`.
  * Final syntax is an open question; see the end of the document.
* **New file `src/jmlib/jugglesaver/js_passing.{h,cpp}`** holds the passing front-end. It must be added to
  `src/jmlib/Makefile`, `src/jmwasm/Makefile:22-29`, `src/jmapple/project.yml:44-51`, and the jmdlx Xcode
  project (`src/jmdlx/xcode-3d/…pbxproj`) if it is still used.
  * `int ParsePassingPrefix(const char* in, const char** rest)` returns J (1 if there is no prefix) and
    rejects J outside `[1, JS_MAX_JUGGLERS]` (`JS_MAX_JUGGLERS = 8`).
* **`engine.cpp:ParsePattern` (`:889-1057`)** gets a new signature:
  `EXT_SITE_INFO* ParsePatternEx(const char* Site, int* pLen, int* pJugglers)`. `ParsePattern` stays as a
  J = 1 wrapper that rejects a prefix.
  * It strips the prefix and stores the bare throws in `vss` (`:885-892`), so `GetCurrentSite()`, the
    overlay, and `jm_flat->resetJS` see `966`.
  * Default positions are no longer filled at parse time with `Len % 2` (`:969, :988`) when J > 1. Store
    the partial position plus a "which fields were given" mask, and let `InitPatternInfo` fill the rest
    with the slot's real side. For J = 1, keep the current behaviour unchanged, including its existing
    odd-length quirk.
  * Default spins (`:1031`) become `GetDefaultSpins(Weight / J)` for selfs and `GetDefaultPassSpins` for
    passes. J = 1 is unchanged.
* **`js_validator.cpp`.**
  * Add `static bool validateWeights(const int* w, int n)`, an int-array version of `validateVSS`
    (`:59-85`) for values > 61.
  * `validateJSPattern` (`:92-106`) calls `ParsePatternEx` and validates the weights instead of the char
    string.
  * Extra J > 1 rules:
    * Reject `0 < t <= CarryTime` with `CarryTime = 0.56·J`. With J = 2 that rejects `1`, whose flight
      time `t − CarryTime` would be negative (`engine.cpp:572`).
    * Reject patterns where some juggler never throws, such as `J2:6 0`. That is allowed but confusing; at
      least warn about it.
* **`jmlib_wrapper.cpp:setPattern`.** The JuggleSaver branch currently calls `jm->setPattern(GetCurrentSite())`
  (`:387-392`). For J > 1 it must not feed `966` to JuggleMaster (a different, solo pattern). Skip that call
  and keep `jm` on its previous pattern. `setStyle` already refuses to switch to JuggleMaster when
  `validateSite(currentPattern)` fails (`:443-451`). Verify this for `J2:…`.
* **`jm_capi.cpp:jm_is_valid_pattern`** needs no change, provided `isMSS` and `isSSS` reject `:`. Add a
  test for that.

### 1+2.2 Engine (`jugglesaver.h`, `engine.cpp`)

**Structs (`jugglesaver.h`):**

* `#define JS_MAX_JUGGLERS 8`
* `typedef struct { float x, z, Facing; } JUGGLER_PLACEMENT;` Facing is the yaw in degrees, with 0 = facing +z.
* `PATTERN_INFO` (`:187-202`):
  * Replace `LeftHand`/`RightHand` with `OBJECT_POSITION Hands[2 * JS_MAX_JUGGLERS]`.
  * Add `int Jugglers; float CarryTime; JUGGLER_PLACEMENT Placement[JS_MAX_JUGGLERS]; int Formation;`.
* `THROW_INFO` (`:133-145`): add `int ToHand; unsigned char IsPass; float TwistSign;`.

**Functions (`engine.cpp`):**

* **`InterpolatePosition` (`:87`) / `InterpolateCarry` (`:169`).** Add a `float CarryTime` parameter that
  replaces the macro at `:100` and `:172`.
* **`GetBallTwistAmount` (`:54-60`).** Return `TwistSign * 18 * TimeInAir_solo^1.5`, using
  `TimeInAir / J` so a self 6 at J = 2 twists like a solo 3. `TwistSign` is computed once in
  `InitPatternInfo` from the **thrower-local** x of From and To (the catch point is transformed into the
  thrower's frame), so jugglers facing the other way twist correctly. For J = 1 the sign rule is exactly
  today's `FromPos.x > ToPos.x`.
* **`GetHandPosition` (`:180-249`).**
  * New signature: `GetHandPositionEx(PATTERN_INFO*, int Hand, float Time, POS*)`.
  * The old `GetHandPosition(p, RightHand, …)` becomes `GetHandPositionEx(p, RightHand ? 1 : 0, …)`, so
    `render_jm_flat.cpp:605` keeps working.
  * The hold test `TimeInAir == 2` becomes `== 2 * J` (`:204`). `CARRY_TIME` becomes `p->CarryTime` (`:229`).
  * The function returns **world** coordinates.
* **`GetObjectPosition` (`:277-387`).** The hold test becomes `== 2J` (`:299`), and `CARRY_TIME` becomes
  `CarryTime` (`:306`). The rest is unchanged, because positions are already in world space.
* **`SetHeightAndAlpha` (`:401-432`).** Compute `MaxW` over solo-equivalent weights (`w / J`, as a float
  for passes), then `Alpha_slot = Alpha / J²`. `Height` stays in world units.
* **`GetDefaultFromPosition` / `GetDefaultToPosition` (`:451-492`)** get the signature
  `(side, Weight, J, IsPass, POS*)`.
  * Selfs use solo weight `Weight / J`. That gives the same table as today and reproduces J = 1 exactly.
  * New pass defaults, in the thrower and catcher local frames:
    * From: x = side ? +0.5 : −0.5, y = 0, z = 0.3 (slightly forward), Rot = 0 (club points at the partner), Elev = 0.
    * To: x = catcher-side ? +0.9 : −0.9 (catch slightly outside), y = −0.5, z = 0.3, Rot = 0, Elev = spins·360 − 50 (see below).
    * The numbers are starting values for tuning (see Risks).
* **`GetDefaultPassSpins(w, J)`**: `floor(w / J)` → 3 → single, 4 → double, ≥ 5 → triple. This matches
  passing practice: 3.5p is a single and 4.5p a double. Tuned in Phase 3.
* **`InitPatternInfo` (`:501-705`).** New signature
  `InitPatternInfoEx(PATTERN_INFO*, const int* Site, const EXT_SITE_INFO*, int Len, int J, int Formation)`.
  The old one forwards with J = 1.
  * `InfoLen = lcm(Len, 2J)` (`:507`).
  * The side and juggler of every slot come from `s mod 2J`. They replace `i % 2` (`:539, :544`).
  * `PrevThrow` uses `% InfoLen` (`:533`). This is harmless for J = 1, because pass 2 overwrites it.
  * Fill placement from the formation (1+2.3). After the From/To positions are final, and before the
    velocities (`:572-585`):
    * transform FromPos by the thrower's placement and ToPos by the catcher's placement, adding Facing to Rot for clubs and rings;
    * compute `IsPass`, `ToHand`, `TwistSign`.
  * **Club orientation for passes.** The catcher's facing differs by about 180°. Store ToPos in the
    equivalent representation `(Rot + 180, 180 − Elev)` when `|NormaliseAngle(To.Rot − From.Rot)| > 90`,
    and compute `FromVelocity.Rot` and `ToVelocity.Rot` from that minimised pair. Otherwise:
    * the in-flight minimisation (`:355-366`) and the carry spline (`InterpolatePosition` normalises P0 and
      P1 separately, `:160-163`) disagree;
    * the club visibly yaws 180° inside the catcher's hand.
  * **Pass catch damping.** Scale the horizontal (x, z) part of `ToVelocity` by a constant, for example
    0.3, for passes only, so the catcher's carry spline does not shove the hand through the torso. Do the
    same with `FromVelocity` in the "going for next catch" spline (`GetHandPosition`, `:243-246`), which
    otherwise overshoots forward after a pass.
  * Holds: `Next += 2` becomes `+= 2J` (`:619`). `Wait = 2` / `+= 2` / fallback `2` become `2J`
    (`:642-658`). The hold value test `!= 2` becomes `!= 2J` (`:614, :617, :634, :643`).
  * Hands: `Hands[h].ThrowIndex = Hands[h].TimeOffset = h` for `h < 2J`. This replaces `:701-702`, with
    `Hands[0]`/`Hands[1]` equal to today's `LeftHand`/`RightHand`.
  * **Collision pass** (only when J > 1). Sample all pairs of objects in flight over one period
    (`InfoLen` slots, 16 samples per slot). If two objects come closer than `2 * BallRad` (or a club
    radius), shift that pass's catcher-local To x outward by 0.15, recompute, and repeat up to N times.
    Log if it does not converge. This deals with mirrored opposite-direction passes on the same line, as
    in `J2:7` (ultimates). Mirror-symmetric parabolas launched one slot apart meet at equal height at their
    lateral crossing point, so a pure table rule cannot avoid the collision.

### 1+2.3 Formation / placement

* **`ApplyFormation(PATTERN_INFO*, int Formation)`** in `engine.cpp`:
  * J = 1: `{0, 0, 0}`, which is exactly today.
  * J = 2, "facing": A at `(−D/2, 0)` with facing +90° (towards +x), B at `(+D/2, 0)` with facing −90°.
    The default front camera, looking along −z, then shows both passers in profile, the classic passing
    view.
  * J ≥ 3, "circle": all jugglers on a radius-R circle facing the centre.
  * `D` = hand-plane distance. Start at about 9 units; the shoulder width is 1.9 units ≈ 0.4 m, so 9
    units ≈ 2 m between hands. The value is a tunable constant.
* The formation centre is the scene origin. Auto-rotate (`SpinAngle`) therefore orbits the formation.

### 1+2.4 Rendering (both renderers, identical math)

* **Shared helper in `render_common.cpp`:**
  `void JugglerLocalHand(const PATTERN_INFO*, int Juggler, const POS* World, POS* Local)` (inverse
  `R_y(−facing)` and the translation).
* **Shared IK.** Factor the IK numbers of `DrawArm` / `drawArmES` into
  `void ArmIK(const POS* LocalHand, int Left, ARM_IK* out)` in `render_common.cpp`, returning the four
  angles and the shoulder translate. Change the depth to `dz = ShoulderPos[2] + LocalHand->z`, clamped to
  ≥ 0.2 so `asin` stays in its branch. Both renderers then consume identical numbers, which makes "visually
  identical" structural rather than by convention. Leave `JMDrawArm` and the shader JM juggler untouched.
* **Legacy `DrawGLScene` (`render_legacy.cpp:961-1029`).** Objects are unchanged (`:984-1025`). Replace
  `:1026-1028` with a loop over `j < J`:
  * when J > 1: `glPushMatrix(); glTranslatef(px, 0, pz); glRotatef(facing, 0, 1, 0);`
  * set the body material (per-juggler colour, below);
  * `glCallList(DL_TORSO)`, then `DrawArm(pState, Time, j, 1)` and `DrawArm(pState, Time, j, 0)`;
  * `glPopMatrix()`.
  * `DrawArm` (`:888`) takes `(pState, Time, Juggler, Left)` and calls `GetHandPositionEx` with
    `hand = Juggler + J * Left` (Left = 1 is side 1, as today), then `JugglerLocalHand` and `ArmIK`.
* **Shader `DrawGLScene` (`render_shader.cpp:892-1118`).** Use the same loop. `bodyBase` (`:1015`) gets
  `translate(px, 0, pz).rotate(facing, 0, 1, 0)` when J > 1. `drawArmES` (`:1035`) takes the juggler index
  and uses `ArmIK`. The torso, head and shoulder bar blocks (`:1070-1114`) move inside the loop unchanged.
* **Per-juggler body colours.** Add `extern const float JugglerBodyCols[JS_MAX_JUGGLERS][4]` in
  `render_common.cpp`. Entry 0 must equal `BodyCol` (`render_common.cpp:16`), and the others are muted
  hues. Use them only when J > 1 **and** the C API flag `jm_set_juggler_colors(1)` is set. Default on for
  J > 1, so A and B are distinguishable.
* **Camera (`SetCamera`, legacy `:104-221`, shader `:789-841`).** Extract today's eye/target computation
  into `render_common.cpp: ComputeCameraFit(const RENDER_STATE*, float fov, float* ey, float* ez, float* cy)`
  and call it from both renderers, which removes the duplication.
  * For J > 1:
    * replace `w = 3` (`:135`) by the formation's z half-extent + 3;
    * add a width fit: the formation x half-extent + 4 (arm reach plus the pass arc), divided by
      `tan(hfov/2)` with `hfov = 2·atan(tan(FOV/2)·Aspect)`;
    * take `ez = max(height-fit ez, width-fit ez)`;
    * H keeps the existing loop over `FromPos.y` plus `Height` (`:150-153`). Heights are world-space y and
      unaffected by the yaw.
  * For J = 1, the code path is the extracted original. Verify the extraction does not change the float
    operation order (compile both and run the golden and pixel tests).
* **2D (`render_jm_flat.cpp:renderJS`, `:524-631`).** Recommendation for Phase 1+2: for J > 1, draw a
  **world side view**: x horizontal, y vertical, the current zoom logic. Each juggler is drawn as a profile
  stick (vertical torso line at `px`, head circle, both arms as lines to the projected hands). Objects use
  world x/y. This keeps the 2D toggle meaningful and needs no new engine data. The alternative, disabling
  2D for J > 1, is an open question. J = 1 code is untouched.

### 1+2.5 Wrapper / C API / UI

* **`jmlib_jsaver.cpp`.**
  * `doJuggle` (`:201`): `state.Time += JuggleSpeed * dt * Jugglers`.
  * `getSiteposStart` (`:252-257`) already works for four-handed siteswap (one character per slot), as long
    as `siteswap` is the stripped `966`.
  * Add `int numJugglers()`.
* **`jmlib_wrapper` / `jmlib.h`.** Add `virtual JML_INT32 numJugglers() { return 1; }` and
  `setFormation(int)` / `getFormation()`. `JMLibWrapper` forwards these to `js` when it is active.
* **`jm_capi.h/.cpp`:**
  * `JM_API int jm_get_juggler_count(void);` returns 1 for JuggleMaster.
  * `enum { JM_FORMATION_AUTO = 0, JM_FORMATION_FACING = 1, JM_FORMATION_CIRCLE = 2, JM_FORMATION_LINE = 3 };`
    `jm_set_formation(int)` re-inits the current pattern (`SetPattern`), and `jm_get_formation(void)`.
  * `JM_API void jm_set_juggler_colors(int on); JM_API int jm_get_juggler_colors(void);`
  * Mirror these in `src/jmwasm/ui/src/types/wasm.d.ts`, `src/jmwasm/ui/src/constants/jmlib.ts` and
    `src/jmapple/JugglerKit/Sources/Engine.swift`. The wasm export list in `src/jmwasm/Makefile` needs
    updating if it uses `EXPORTED_FUNCTIONS`; check before relying on `EMSCRIPTEN_KEEPALIVE` alone.
* **Web UI (minimal for this phase).** Nothing new is strictly required: entering `J2:966` in the pattern
  field works. Show a small "2 jugglers" badge near the pattern name when `jm_get_juggler_count() > 1`
  (i18n key `passing.jugglerCount`, en + no).
* **Apple.** `JugglerModel` gets `jugglerCount` (polled with the site). The same badge goes in
  `SharedViews.swift` (en + nb).

### 1+2.6 Tests

* **J = 1 engine golden test (new; the strongest regression guard, and it also covers the legacy path's
  engine).**
  * Add `tests/engine/engine_golden.cpp` plus a Makefile target. It needs no GL; `engine.cpp` only uses
    headers.
  * **Before any engine change**, on master, record `GetObjectPosition` for every object and
    `GetHandPosition` for both hands at `Time = 50 + k·0.037` for k < 2000 into
    `tests/engine/golden/*.txt` (`%a` hex floats).
  * Patterns: the 7 visual patterns, all of `moresites.txt` (~340), and the `Generate` output with a fixed
    `srand`.
  * After the refactor, require **exact** equality.
* **J > 1 engine tests** in the same binary:
  * `J2:6`: no passes; each juggler's objects stay within its own half (x sign).
  * `J2:7`: every throw lands at the other juggler.
  * `J2:966`, `J2:786`: object count 7; the throw and catch hands match `s mod 4`.
  * Orientation continuity at the catch for club passes: compare the club axis vector at `tc − ε` and
    `tc + ε`, not the angles.
  * Hand position continuity across every throw and catch.
  * Minimum pair distance ≥ `2 * BallRad` over a period for every library passing pattern.
* **Validator tests.**
  * `J2:966` is valid; `J2:965` and `J2:1` are invalid.
  * `jm_is_valid_jm_pattern("J2:966") == 0` and `jm_is_valid_pattern("J2:966") == 1`.
  * `966` without the prefix still goes to JuggleMaster.
* **Playwright (`tests/visual/visual.spec.ts`).** Existing baselines unchanged; this is the no-regression
  proof for the shader path. Add `js-passing-966` and `js-passing-7` in 3D and 2D, with new baselines
  reviewed by eye.
* **Apple.**
  * `RenderParityTests.swift:31-39`: add the same two cases; they must match the web baselines.
  * `EngineTests`: `jugglerCount == 2` after `setPattern("J2:966")`.
* **Legacy renderer (manual, documented in the PR).** Run `USE_SHADERS=0` jmdlx (`run-legacy.sh`) and
  `USE_SHADERS=1` (`run-new.sh`) side by side on `J2:966`, `J2:7` and `J2:786c` with the same frame and
  camera. Also re-check a few solo JS patterns in legacy mode. Optional automation: a jmdlx
  `--screenshot <file> --frames 120 --fps 60` flag would allow a legacy-vs-shader pixel diff in CI.

### 1+2.7 What the user can visibly test

Type `J2:966` (7-object three-count, crossing doubles), `J2:7` (7-object ultimates, straight singles),
`J2:786`, `J2:6` (two side-by-side cascades with no passes) or `J2:7c`. Two jugglers face each other in
profile, objects fly between them, arms follow the hands, and the siteswap overlay steps through `966`.
Orbit, zoom, autorotate, the object-type override and 2D all work. Every existing solo pattern looks
exactly as before.

### 1+2.8 Risks

* Club pass orientation and spin count (the 180° yaw equivalence) is the trickiest part. It is mitigated by
  the continuity test, but expect visual tuning.
* Hand trajectories after passes (overshoot or going behind the body). Mitigated by the damping constants
  and the dz clamp.
* Camera framing at extreme aspect ratios (portrait phones). The width fit may push the camera far back.
* Float-order drift from extracting `ComputeCameraFit` and `ArmIK`. Mitigated by the exact golden test
  plus the existing pixel baselines.
* `render_shader.cpp` file-level statics (one engine per process, per `apple-port.md`) are unaffected,
  but per-juggler colours must not become new globals that leak between patterns.

---

## Phase 3 — Symmetric Prechac notation

**Goal.** Accept `3.5p`, `4.5p 3 3`, `3p 3 3` and similar, with every juggler doing the same sequence, by
converting to the interleaved model from Phase 1+2.

**Correction to the outline.** Converting by "multiply the values by J" is only half the rule.
* Symmetric Prechac offsets juggler `j` by `j·n/J` **beats**, where n is the period: a time offset *and* a
  sequence phase.
* Example: `4.5p 3 3` with J = 2 gives A = `9 6 6` and B = the same sequence starting 1.5 beats later. The
  interleaved slots are then `9 6 6 9 6 6…` read with B on the odd slots, i.e. four-handed `966` (verified
  by hand: A's slots 0, 2, 4 = 9, 6, 6 and B's slots 1, 3, 5 = 6, 9, 6). A naive per-value doubling gives
  `996666`, which collides.
* The interleaved representation exists only when `gcd(n, J) = 1`, so the jugglers fall on distinct
  slots. Otherwise (for example `4p 3`), jugglers throw at the same instant, which needs Phase 4. Whole-beat
  patterns that do not work with the staggered start (`3p 3`, `3p 3 3 3`, the chocolate bar `3p 3p 3 3`) are
  read with all jugglers throwing together, the way they are usually meant.

**Changes:**
* **`js_passing.cpp`.**
  * Tokeniser for Prechac tokens: `\d+(\.\d+)?(p\d*)?`, with an optional JS object/position suffix per
    token. `p` means pass to the next juggler (`pN` = N jugglers ahead, optional).
  * Juggler count: an optional prefix (`J3:`) or inferred from the fractional parts (default 2).
  * `bool PrechacToSlots(tokens, J, int** slots, int* len)`:
    1. build explicit events (thrower j, time, value, receiver);
    2. place them on slots `s = J·(time − offset_j·…)`, using `offset_j = j·n/J`;
    3. **check** that every event lands on a slot owned by the intended receiver.
    Reject the pattern if any of this fails, or if `gcd(n, J) != 1`, with a message that it needs sync
    support (Phase 4).
  * Keep a **token map**: slot ↔ (juggler, token index, char range) for the overlay.
* **`ParsePatternEx`** routes Prechac input (any token with `p` after a number, or a `.`) through
  `PrechacToSlots`. The ext info (object, spins) is copied per slot from its token.
* **Validator.** Same as before (`validateWeights` on the slots). It also needs a Prechac syntax check.
  Reject lowercase-letter throws in passing input; lowercase is already inconsistent (see section 0).
* **Default pass spins.** Tune `GetDefaultPassSpins` on library patterns (single at 3.5p, double at 4.5p
  where conventional). Club passes in particular may want the selfs as doubles (`3` at J = 2 → slot value
  6 → spins via `GetDefaultSpins(3) = 1`, as today).
* **Overlay per juggler.**
  * C API: `JM_API const char* jm_get_juggler_site(int j)` returns the line, for example `4.5p 3 3`.
    `JM_API int jm_get_juggler_sitepos_start(int j)` and `…_stop(int j)` return character offsets into
    that line, computed from the token map and `Time`. For four-handed input, line 0 is the whole site
    and only one line is returned, so the overlay is unchanged.
  * `jm_get_site` keeps returning the full user string.
  * Web: `useSiteswapInfo.ts` polls the count and the per-juggler lines. `SiteswapOverlay.tsx` renders one
    row per juggler with a juggler label (`A`, `B`, … i18n key `passing.jugglerLabel`) in the juggler's
    body colour.
  * Apple: `Engine.siteRange` (`Engine.swift:95-98`) gets a per-juggler counterpart, and the overlay views
    (`SharedViews.swift`, `TVRootView.swift`, `JugglerStage.swift`) render the rows.
* **Library.** Add a passing section to `data/moresites.txt`:
  * `$ Passing patterns` / `$ ----------------` / `$ (N entries)`, then `$ Two jugglers:`, with entries
    such as `"7 club 3-count" J2:9c6c6c;`, `"Ultimates (7)" J2:7c;`, `"3-count (Prechac)" 4.5p 3 3;`.
  * The three parsers (`js_patterns.cpp`, `parseJs.ts`, `PatternParsers.swift`) already handle the format.
    `parseJs.ts` and `PatternParsers.swift` append " (JuggleSaver)", so the category shows as "Passing
    patterns (JuggleSaver)". Decide whether to keep the suffix.
  * Remember the copy in `src/jmwasm/ui/public/moresites.txt`, and check whether the web build copies it
    from `data/`. `PatternLibraryParityTests.swift` will catch Apple/web drift.
  * Web filter (`lib/patterns/filter.ts`): an optional "Passing" quick filter on category, i18n.

**Tests.**
* Engine: `PrechacToSlots("4.5p 3 3", 2)` gives `966`; `3.5p` gives `7`; `3p 3` is rejected (needs sync);
  the J = 3 Prechac example `3.33p 3 3`, or whatever form is chosen, maps correctly.
* Token-map unit tests drive the overlay offsets.
* Playwright: overlay rows for `4.5p 3 3` (DOM text plus highlighted span); a baseline for `4.5p 3 3c`.
* Apple: parser parity tests for the new library section; `AppStateTests` for share URLs with `.` and `p`
  (form encoding, `AppState.swift:103-119`).

**Visible result.** Choose "Passing patterns" in the library, or type `4.5p 3 3`. The overlay shows one row
per juggler with the current throw highlighted, and A and B are colour-coded.

**Risks.**
* Prechac conventions vary: half-beat offsets versus `p` meaning next or previous juggler. Pick
  JugglingLab/Prechac-calculator conventions and document them.
* Collisions in denser patterns; the collision pass may need to adjust heights as well as lateral offsets.

---

## Phase 4 — Per-throw time / per-hand time shifts (sync passing, uneven Prechac, feeds, solo sync)

**Goal.** Decouple a slot's index from its throw time so that jugglers (and two hands of one juggler) can
throw at the same instant. This covers `<3p 3 3 3|3p 3 3 3>`, `4p 3` style symmetric patterns with
`gcd(n, J) > 1`, uneven Prechac and feeds (J = 3, feeder `3p 3p`, feedees `3p 3 3 3`), and solo sync in
JuggleSaver.

**Model.**
* Every hand h gets a constant `float HandShift[h]` (in slots). Throw time = slot index + shift of the
  throwing hand. Flight duration = `TimeInAir + HandShift[ToHand] − HandShift[FromHand]`.
* Because the shift is constant per hand, `NextForHand` spacing and the hand cycle are unchanged.
* Sync passing with J jugglers: juggler j's hands shift by `−j`, so all jugglers throw at slot multiples
  of J.
* Solo sync (J = 1): side-1 hand shift = −1, matching JuggleMaster's "shift on alternate beats".
* Uneven Prechac: a per-juggler rational offset as the shift.

**Engine changes (`engine.cpp`):**
* `PATTERN_INFO`: add `float HandShift[2 * JS_MAX_JUGGLERS]`. `THROW_INFO`: add `float Flight` (the
  effective flight time) and `int FromHand`.
* `GetObjectPosition` (`:286-295`) advances with `TotalTime` measured in shifted time. Replace
  `(int) Time` against an integer `TimeOffset` with a float `TimeOffset` plus the shift of the current
  throw's hand. Use `pThrow->Flight − CarryTime` instead of `TimeInAir − CARRY_TIME` (`:306`).
* `GetHandPosition` (`:190-196, :229-246`): the same float time base for the hand. The "carry" vs "going
  for catch" split uses the shifted catch time of the incoming object.
* `InitPatternInfo`: velocities use `Flight − CarryTime` (`:572`). Validate that `Flight > CarryTime` for
  every non-hold throw. The initial object placement (`:674-699`) uses shifted times.
* `OBJECT_POSITION.TimeOffset` becomes `float`. For J = 1 with no shift, all shifts are 0.0f, so the values
  are identical. The golden test re-runs here.

**Parsing.**
* `js_passing.cpp`: a `<a b c|d e f>` front-end gives J = the number of bars, all jugglers in sync. `p`
  passes to the next juggler and `pN` to juggler N (JugglingLab convention). Values are in beats, the slot
  value is `J·value` adjusted by the receiver offset, and the shifts are `−j`.
* Solo sync in JuggleSaver: accept `(4c,4c)`-style tokens through the JS parser when an object or position
  suffix is present. Otherwise the wrapper still routes plain SSS to JuggleMaster (`jmlib_wrapper.cpp:320-331`).
  Reuse the `x` rule from `transSyncMSS` (`validator.cpp:394-448`) for the crossing adjustment.
* Formations: "line" or "feed" (juggler 0 facing the others, who stand in a row), chosen automatically when
  one juggler's sequence passes to everyone. `JM_FORMATION_LINE` is already reserved in Phase 1+2.

**Overlay.** For `<…|…>`, each bar is a juggler line. The token map from Phase 3 gives the character
ranges, so no new API is needed.

**Tests.**
* `<3p 3 3 3|3p 3 3 3>` (6-club 4-count): both jugglers' throw times are identical, and objects
  conserve.
* `<3p|3p>` (6-club every-others) works.
* Feed `<3p 3p|3p 3|3 3p>`, or the chosen syntax, works.
* Solo `(4c,4c)` in JuggleSaver matches JuggleMaster's throw timing qualitatively.
* J = 1 golden equality, and the existing Playwright baselines.
* New baselines for one sync passing pattern and one feed.

**Visible result.** Synchronous 4-count and every-others, a 3-person feed in a line formation, and solo
sync club patterns in JuggleSaver.

**Risks.** This changes the core time base (integer → float offsets), so it carries the highest J = 1
regression risk; it is guarded by the golden test. The UI has to explain why `4p 3` is rejected before this
phase ships.

---

## Phase 5 — Out of scope / future

* Multiplex passing (`[3p3]`): multiple objects per slot, which the `THROW_INFO` model has no room for.
* Walking and choreography (JoePass `#jugglerPosition`, movement over time), with placement as a function
  of time. The per-juggler transform from 1+2 is the hook for it.
* Per-object colour by thrower and a club "tramline" visualisation.
* Juggler-specific skin and hat colours beyond body tint, and named jugglers.

---

## Cross-cutting: UI, share URLs, camera presets

**Camera presets (Phase 1+2, finished in Phase 3).**
* `jm_capi.h` enum: add `JM_CAMERA_OVERVIEW = 5`, `JM_CAMERA_PASSER_SIDE = 6`, `JM_CAMERA_PASSER_A = 7`.
* `jm_set_camera_preset` (`jm_capi.cpp:236-246`):
  * Overview: `setCameraAngle(0, 0, 35)`, elevated over the formation.
  * Side: `setCameraAngle(0)`, profile of the passing line, the default.
  * Passer's view: `setCameraAngle(−90, 0, 8)`, looking from behind A towards B, plus a zoom preset.
* The solo presets keep their indices.
* Web: `CameraPresets.tsx` shows the passing set when `jugglerCount > 1`. Fix the existing hard-coded
  labels at the same time (i18n `camera.left` … `camera.passerView`, en + no).
* Apple: `CameraPreset` (`Engine.swift:21-27`) gets new cases. The bars in `SharedViews.swift` and the tvOS
  bar switch sets the same way (Localizable en + nb). `JugglerModelTests.cameraPresetIsTracked` gets
  extended.

**Share URLs.**
* The pattern string already carries J (prefix or notation), so no `jugglers` parameter is needed.
* Add `formation` (`facing|circle|line`, omitted when auto) and `jcolors=0` (only when per-juggler
  colours are turned off).
* Web: `useUrlParams.ts` (`AppUrlParams`, `readParams`, `writeParams`, `urlParamsToCanvas`). Apple:
  `JMKit/Sources/JMKit/AppState.swift` (init(query:) `:51-62`, the encoder `:78-87`) plus `AppStateTests`.
  Keep both codecs byte-compatible; add a shared fixture test for `pattern=J2%3A966&formation=circle`.

**Settings UI.**
* Web: a "Passing" section in the settings menu (`components/Menu.tsx`), visible when J > 1, with a
  formation select and a "colour jugglers" switch. Use existing shadcn components only; ask the user
  before adding any.
* Apple: the equivalent in `MenuView.swift`, plus the tvOS panel.
* Strings: en + no (web) and en + nb (Apple).

---

## Overall risks and open questions

1. **Syntax for four-handed siteswap.** Candidates: `J2:966` (recommended: unambiguous, JuggleMaster
   rejects it, URL-safe after encoding), `966 %2`, or no four-handed input at all (Prechac only). This
   needs a user decision before 1+2.1.
2. **2D mode for J > 1.** World side view (recommended) or disable 2D for passing patterns.
3. **Pass distance and formation constants** (`D` ≈ 9 units, circle radius): what looks right to the user.
4. **Speed semantics.** `Time × J` keeps each juggler's personal tempo equal to solo at the same speed
   setting. The alternative keeps the global slot rate, so passing looks J× slower.
5. **Object colours.** Keep `Cols[i % n]` per object (today's behaviour) or colour by the first thrower.
6. **The " (JuggleSaver)" suffix** on the Passing category name in both UIs.
7. **Collision avoidance** may still need height tweaks for dense patterns (8+ objects). The automatic
   nudge is deterministic, but it is a heuristic.
8. **No legacy-renderer automation.** The golden engine test covers the engine. Pixel identity of legacy
   vs shader for multi-juggler bodies relies on the shared `ArmIK`/`ComputeCameraFit` helpers and a manual
   jmdlx check, unless the optional jmdlx screenshot flag is added.
9. **Phase 4 refactors the time base.** It is the most likely place for subtle J = 1 regressions. Do not
   start it without the golden test in place.
10. **`ParsePattern` global state** (`static char* vss`, `engine.cpp:885`) is shared by the validator and
    the engine. The new token map must follow the same lifecycle, or better, live in `PATTERN_INFO`/the
    wrapper rather than in another global.

---

## Rough relative effort

| Phase | Effort | Notes |
|---|---|---|
| 1+2 Engine generalisation, four-handed siteswap, both renderers, camera, 2D, golden tests, minimal API/UI | **L (~45%)** | Most of the risk: club pass orientation, hand dynamics, collisions, J = 1 bit-exactness. |
| 3 Symmetric Prechac, per-juggler overlay, Passing library, presets/i18n | **M (~20%)** | Mostly parser and UI. The engine is untouched. |
| 4 Per-hand time shifts (sync passing, feeds, uneven Prechac, solo sync), line formation | **M–L (~30%)** | Core time-base change. Re-runs all J = 1 regression guards. |
| Cross-cutting share URLs / settings UI (spread over 1–3) | **S (~5%)** | Two codecs plus tests. |
| 5 Multiplex / walking | — | Out of scope. |
