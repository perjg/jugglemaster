internal import CJMLib

public enum RenderMode: Int32, Sendable, CaseIterable {
    case twoD = 1
    case threeD = 2
}

public enum EngineType: Int32, Sendable {
    case juggleMaster = 0
    case juggleSaver = 1
}

public enum ObjectType: Int32, Sendable, CaseIterable {
    case `default` = 0
    case ball = 1
    case club = 2
    case ring = 3
    case fish = 6
}

public enum CameraPreset: Int32, Sendable, CaseIterable {
    case front = 0
    case left = 1
    case back = 2
    case right = 3
    case high = 4
}

/// Swift face of `jm_capi.h`. Every call makes `GLContext.shared` current
/// first, because setters may create or delete GL objects.
@MainActor
public final class Engine {
    public static let shared = Engine()

    public static let speedRange: ClosedRange<Float> = 0.1...10
    public static let defaultSpeed: Float = 3

    public private(set) var isStarted = false

    private init() {}

    private func gl<T>(_ body: () -> T) -> T {
        GLContext.shared.makeCurrent()
        return body()
    }

    /// Creates the engine on first use, resizes afterwards.
    @discardableResult
    public func start(width: Int, height: Int) -> Bool {
        isStarted = gl { jm_capi_init(Int32(width), Int32(height)) } == 1
        return isStarted
    }

    public func resize(width: Int, height: Int) {
        gl { jm_resize(Int32(width), Int32(height)) }
    }

    // MARK: Frames

    /// Advances and draws, or only draws while paused.
    public func frame() { gl { jm_frame() } }
    public func draw() { gl { jm_draw() } }
    public func render() { gl { jm_render() } }
    public func step(frames: Int) { gl { jm_step_frames(Int32(frames)) } }

    public var isPaused: Bool {
        get { jm_get_paused() != 0 }
        set { jm_set_paused(newValue ? 1 : 0) }
    }

    /// Pins the animation clock (fps > 0) for reproducible frames; 0 restores wall-clock time.
    public func setFixedFrameRate(_ fps: Float) { gl { jm_set_fixed_framerate(fps) } }

    // MARK: Pattern

    /// False (keeping the current pattern) if no engine juggles `site`.
    @discardableResult
    public func setPattern(name: String, site: String) -> Bool { gl { jm_set_pattern(name, site) } == 1 }
    public func setStyle(_ style: String) { gl { jm_set_style(style) } }
    public var patternName: String { String(cString: jm_get_pattern_name()) }
    public var style: String { String(cString: jm_get_style()) }
    public var engineType: EngineType { EngineType(rawValue: jm_get_engine()) ?? .juggleMaster }

    public var speed: Float {
        get { jm_get_speed() }
        set { gl { jm_set_speed(newValue) } }
    }

    /// Whether `setPattern` would accept `pattern`.
    public func isValidPattern(_ pattern: String) -> Bool { jm_is_valid_pattern(pattern) == 1 }
    public func isValidJuggleMasterPattern(_ pattern: String) -> Bool { jm_is_valid_jm_pattern(pattern) == 1 }
    public func isValidJuggleSaverPattern(_ pattern: String) -> Bool { jm_is_valid_js_pattern(pattern) == 1 }

    public var site: String { String(cString: jm_get_site()) }
    /// Always inside `site`, whatever the engine reports.
    public var siteRange: Range<Int> {
        let count = site.count
        let start = min(max(0, Int(jm_get_sitepos_start())), count)
        return start..<min(max(start, Int(jm_get_sitepos_stop())), count)
    }

    // MARK: Rendering

    public func setRenderMode(_ mode: RenderMode) { gl { jm_set_render_mode(mode.rawValue) } }

    public var objectType: ObjectType {
        get { ObjectType(rawValue: jm_get_object_type()) ?? .default }
        set { gl { jm_set_object_type(newValue.rawValue) } }
    }

    // MARK: Camera

    /// Rotates the camera by a drag delta normalized to the view size.
    public func orbit(dx: Float, dy: Float) { gl { jm_orbit_camera(dx, dy) } }
    public func setCameraPreset(_ preset: CameraPreset) { gl { jm_set_camera_preset(preset.rawValue) } }
    public func resetCamera() { gl { jm_reset_camera() } }

    public var autoRotate: Bool {
        get { jm_get_autorotate() != 0 }
        set { gl { jm_set_autorotate(newValue ? 1 : 0) } }
    }

    public func zoom(by delta: Float) { gl { jm_zoom(delta) } }
    public func resetZoom() { gl { jm_reset_zoom() } }
    public var zoom: Float { jm_get_zoom() }
}
