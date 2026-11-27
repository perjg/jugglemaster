import Foundation
import JMKit
import Observation

/// Validity of typed siteswap text: per engine syntax, and whether an engine will juggle it.
public struct SiteswapValidation: Equatable, Sendable {
    public var juggleMaster: Bool
    public var juggleSaver: Bool
    public var isValid: Bool
}

/// The current siteswap with the throw being animated, `site[range]`.
public struct SiteswapDisplay: Equatable, Sendable {
    public var site = ""
    public var range = 0..<0

    public var before: Substring { site.prefix(range.lowerBound) }
    public var active: Substring { site.dropFirst(range.lowerBound).prefix(range.count) }
    public var after: Substring { site.dropFirst(range.upperBound) }
}

/// Single source of truth for the UI, driving `Engine.shared`. Mirrors the web UI's state and rules
/// (Menu.tsx, PatternTabContent.tsx, Canvas.tsx); the C calls are synchronous, so state is re-read
/// from the engine right after each change.
@MainActor
@Observable
public final class JugglerModel {
    public static let styles = ["Normal", "Reverse", "Shower", "Mills Mess", "Center", "Windmill"]
    public static let speedStep = 0.1
    static let speedCommitDelay: Duration = .milliseconds(500)
    public static let zoomStep: Float = 0.05
    public static let zoomRange: ClosedRange<Float> = 0.01...0.9

    /// Engine the siteswap tab prefers; the actual engine may differ when a pattern only suits the other.
    public private(set) var preferredEngine = EngineType.juggleMaster
    public private(set) var actualEngine = EngineType.juggleMaster
    /// JuggleMaster style chosen in the style picker.
    public private(set) var selectedStyle = PatternDefaults.style
    public private(set) var renderMode = RenderMode.threeD
    public private(set) var objectType = ObjectType.default
    public private(set) var speed = AppState.defaultSpeed
    public private(set) var autoRotate = false
    public private(set) var isPaused = false
    public private(set) var zoom: Float = 0
    public private(set) var siteswap = SiteswapDisplay()
    /// The last camera preset chosen, nil once the camera was orbited.
    public private(set) var cameraPreset: CameraPreset? = .front
    public var showSiteswap = false {
        didSet {
            guard showSiteswap != oldValue else { return }
            state.showSiteswap = showSiteswap
            persist()
        }
    }

    /// What a share link or the next launch restores.
    public private(set) var state: AppState
    public private(set) var library: PatternLibrary?
    public private(set) var libraryFailed = false

    public let recentSiteswaps: RecentStore<RecentSiteswap>
    public let recentPatterns: RecentStore<RecentPattern>

    @ObservationIgnored private let storage: any KeyValueStore
    @ObservationIgnored private let engine = Engine.shared
    @ObservationIgnored private var isApplied = false
    @ObservationIgnored private var previewOrigin: (state: AppState, objectType: ObjectType, style: String)?
    @ObservationIgnored private var previewedPatternId: Pattern.ID?
    @ObservationIgnored private var speedCommit: Task<Void, Never>?

    /// Query string of the last state; `-jm_last_state <query>` on the command line overrides it.
    static let stateKey = "jm_last_state"

    /// `initialState` (e.g. from a deep link) wins over the state saved by the previous launch.
    public init(initialState: AppState? = nil, storage: any KeyValueStore = UserDefaults.standard) {
        self.storage = storage
        recentSiteswaps = .siteswaps(storage: storage)
        recentPatterns = .patterns(storage: storage)
        let saved = storage.string(forKey: Self.stateKey).map { AppState(query: $0) }
        state = initialState ?? saved ?? AppState()
        adoptSettings(of: state)
    }

    private func adoptSettings(of state: AppState) {
        let style = state.style
        preferredEngine = style == PatternDefaults.juggleSaverStyle ? .juggleSaver : .juggleMaster
        selectedStyle = Self.styles.contains(style) ? style : PatternDefaults.style
        renderMode = state.mode == .twoD ? .twoD : .threeD
        objectType = ObjectType(rawValue: state.object.engineValue) ?? .default
        speed = state.speed
        autoRotate = state.autoRotate
        showSiteswap = state.showSiteswap
    }

    // MARK: Engine lifecycle

    /// Applies the state to the engine; call once the engine has started (in Canvas.tsx order).
    public func engineDidStart() {
        guard !isApplied, engine.isStarted else { return }
        isApplied = true
        applyToEngine()
    }

    /// Switches to the settings of a share or deep link.
    public func load(_ newState: AppState) {
        endPreview()
        state = newState
        adoptSettings(of: newState)
        if isApplied {
            applyToEngine()
            persist()
        }
    }

    private func applyToEngine() {
        cameraPreset = .front
        engine.objectType = objectType
        if !engine.setPattern(name: state.name, site: state.pattern) {
            let defaults = AppState()
            state.pattern = defaults.pattern
            state.name = defaults.name
            state.style = defaults.style
            state.fromLibrary = false
            state.categoryId = ""
            preferredEngine = .juggleMaster
            selectedStyle = defaults.style
            engine.setPattern(name: state.name, site: state.pattern)
            persist()
        }
        engine.setStyle(state.style)
        engine.setRenderMode(renderMode)
        engine.autoRotate = autoRotate
        engine.speed = Float(speed)
        syncFromEngine()
    }

    /// Polls what changes while animating; publishes only on change.
    public func frameDidRender() {
        setIfChanged(\.siteswap, SiteswapDisplay(site: engine.site, range: engine.siteRange))
        setIfChanged(\.zoom, engine.zoom)
    }

    private func syncFromEngine() {
        setIfChanged(\.actualEngine, engine.engineType)
        setIfChanged(\.autoRotate, engine.autoRotate)
        setIfChanged(\.isPaused, engine.isPaused)
        frameDidRender()
    }

    /// Observation notifies on every assignment; per-frame and per-drag updates skip unchanged values.
    private func setIfChanged<Value: Equatable>(
        _ keyPath: ReferenceWritableKeyPath<JugglerModel, Value>, _ value: Value
    ) {
        if self[keyPath: keyPath] != value {
            self[keyPath: keyPath] = value
        }
    }

    // MARK: Library

    /// Parses the bundled pattern files off the main actor.
    public func loadLibrary(from bundle: Bundle = .main) async {
        let result = await Task.detached(priority: .userInitiated) {
            Result { try PatternLibrary.load(from: bundle) }
        }.value
        switch result {
        case .success(let library): self.library = library
        case .failure: libraryFailed = true
        }
    }

    public func setLibrary(_ library: PatternLibrary) {
        self.library = library
        libraryFailed = false
    }

    // MARK: Patterns

    public func validate(_ text: String) -> SiteswapValidation? {
        let site = text.trimmingCharacters(in: .whitespacesAndNewlines)
        guard !site.isEmpty else { return nil }
        return SiteswapValidation(
            juggleMaster: engine.isValidJuggleMasterPattern(site), juggleSaver: engine.isValidJuggleSaverPattern(site),
            isValid: engine.isValidPattern(site))
    }

    /// Juggles typed text if either engine accepts it, falling back to the preferred engine's style.
    @discardableResult
    public func submitSiteswap(_ text: String) -> Bool {
        let site = text.trimmingCharacters(in: .whitespacesAndNewlines)
        endPreview()
        guard !site.isEmpty, engine.setPattern(name: site, site: site) else { return false }
        let style = preferredEngine == .juggleSaver ? PatternDefaults.juggleSaverStyle : selectedStyle
        engine.setStyle(style)
        patternDidChange(name: site, site: site, style: style, fromLibrary: false, categoryId: "")
        recentSiteswaps.push(RecentSiteswap(site: site, style: style))
        return true
    }

    @discardableResult
    public func applyRecent(_ item: RecentSiteswap) -> Bool {
        endPreview()
        guard engine.setPattern(name: item.site, site: item.site) else { return false }
        engine.setStyle(item.style)
        if item.style == PatternDefaults.juggleSaverStyle {
            preferredEngine = .juggleSaver
        } else {
            if Self.styles.contains(item.style) { selectedStyle = item.style }
            preferredEngine = .juggleMaster
        }
        patternDidChange(name: item.site, site: item.site, style: item.style, fromLibrary: false, categoryId: "")
        recentSiteswaps.push(item)
        return true
    }

    @discardableResult
    public func applyRecent(_ item: RecentPattern) -> Bool {
        guard endingPreview({ apply(name: item.name, data: item.data, style: item.style, categoryId: item.categoryId) })
        else { return false }
        recentPatterns.push(item)
        return true
    }

    /// Juggles a library pattern and adds it to recents; false (and nothing changes) if no engine accepts it.
    @discardableResult
    public func apply(_ pattern: Pattern) -> Bool {
        guard
            endingPreview({
                apply(
                    name: pattern.name, data: pattern.data, style: pattern.effectiveStyle,
                    categoryId: pattern.categoryId)
            })
        else { return false }
        recentPatterns.push(RecentPattern(pattern))
        return true
    }

    /// Runs `change` outside of any preview; a rejected change leaves the preview as it was.
    private func endingPreview(_ change: () -> Bool) -> Bool {
        let origin = previewOrigin
        let previewed = previewedPatternId
        endPreview()
        if change() { return true }
        previewOrigin = origin
        previewedPatternId = previewed
        return false
    }

    // MARK: Preview

    /// Juggles `pattern` while browsing, without touching recents. Each preview starts from the object
    /// type chosen before browsing, which `cancelPreview()` restores along with the pattern and style.
    public func preview(_ pattern: Pattern) {
        if previewOrigin == nil {
            previewOrigin = (state, objectType, selectedStyle)
        }
        restoreOriginObjectType()
        let accepted = apply(
            name: pattern.name, data: pattern.data, style: pattern.effectiveStyle, categoryId: pattern.categoryId)
        previewedPatternId = accepted ? pattern.id : nil
    }

    /// Keeps `pattern` (re-applied unless it is the one being previewed) and adds it to recents; false if no
    /// engine accepts it, in which case the preview continues.
    @discardableResult
    public func commitPreview(_ pattern: Pattern) -> Bool {
        if previewedPatternId == pattern.id {
            endPreview()
            recentPatterns.push(RecentPattern(pattern))
            persist()
            return true
        }
        restoreOriginObjectType()
        return apply(pattern)
    }

    /// Returns to the pattern, style and object type from before the first preview.
    public func cancelPreview() {
        guard let origin = previewOrigin else { return }
        endPreview()
        engine.setPattern(name: origin.state.name, site: origin.state.pattern)
        engine.setStyle(origin.state.style)
        engine.objectType = origin.objectType
        objectType = origin.objectType
        selectedStyle = origin.style
        cameraPreset = .front
        state = origin.state
        syncFromEngine()
        persist()
    }

    public var isPreviewing: Bool { previewOrigin != nil }

    private func endPreview() {
        previewOrigin = nil
        previewedPatternId = nil
    }

    private func restoreOriginObjectType() {
        guard let origin = previewOrigin, objectType != origin.objectType else { return }
        objectType = origin.objectType
        engine.objectType = origin.objectType
        state.object = AppState.Object(engineValue: origin.objectType.rawValue)
    }

    private func apply(name: String, data: String, style: String, categoryId: String) -> Bool {
        // setPattern re-selects the engine per pattern, so the style must follow it
        guard engine.setPattern(name: name, site: data) else { return false }
        engine.setStyle(style)
        // Library styles outside the picker's list (e.g. "Tennis") render as Normal in the engine
        if Self.styles.contains(style) {
            selectedStyle = style
        }
        patternDidChange(name: name, site: data, style: style, fromLibrary: true, categoryId: categoryId)
        return true
    }

    private func patternDidChange(name: String, site: String, style: String, fromLibrary: Bool, categoryId: String) {
        actualEngine = engine.engineType
        // Setting a pattern re-centers the camera
        cameraPreset = .front
        // JuggleSaver patterns carry their own object types
        if actualEngine == .juggleSaver {
            objectType = .default
            engine.objectType = .default
            state.object = .default
        }
        state.name = name
        state.pattern = site
        state.style = style
        state.fromLibrary = fromLibrary
        state.categoryId = categoryId
        syncFromEngine()
        persist()
    }

    // MARK: Settings

    public var isStyleEnabled: Bool { actualEngine == .juggleMaster }
    public var engineMismatch: Bool { preferredEngine != actualEngine }

    public func setStyle(_ style: String) {
        selectedStyle = style
        engine.setStyle(style)
        actualEngine = engine.engineType
        state.style = style
        persist()
    }

    public func setPreferredEngine(_ target: EngineType) {
        let style = target == .juggleSaver ? PatternDefaults.juggleSaverStyle : selectedStyle
        engine.setStyle(style)
        preferredEngine = target
        actualEngine = engine.engineType
        state.style = style
        persist()
    }

    public func setRenderMode(_ mode: RenderMode) {
        renderMode = mode
        engine.setRenderMode(mode)
        state.mode = mode == .twoD ? .twoD : .threeD
        persist()
    }

    public func setAutoRotate(_ on: Bool) {
        engine.autoRotate = on
        autoRotate = engine.autoRotate
        state.autoRotate = autoRotate
        persist()
    }

    public func toggleAutoRotate() {
        setAutoRotate(!engine.autoRotate)
    }

    /// Sets the speed, rounded to the 0.1 step and clamped. Without `commit` (slider drags) it is saved once
    /// it stops changing for `speedCommitDelay`.
    public func setSpeed(_ value: Double, commit: Bool = true) {
        let steps = 1 / Self.speedStep
        let clamped = ((value * steps).rounded() / steps).clamped(to: AppState.speedRange)
        speed = clamped
        engine.speed = Float(clamped)
        speedCommit?.cancel()
        if commit {
            state.speed = clamped
            persist()
        } else {
            speedCommit = Task { [weak self] in
                try? await Task.sleep(for: Self.speedCommitDelay)
                guard let self, !Task.isCancelled else { return }
                state.speed = speed
                persist()
            }
        }
    }

    public func setObjectType(_ type: ObjectType) {
        objectType = type
        engine.objectType = type
        state.object = AppState.Object(engineValue: type.rawValue)
        persist()
    }

    /// Object types offered for the actual engine; Default only exists for JuggleSaver patterns.
    public var availableObjectTypes: [ObjectType] {
        let types: [ObjectType] = [.ball, .club, .ring, .fish]
        return actualEngine == .juggleSaver ? [.default] + types : types
    }

    /// JuggleMaster has no pattern default, so Default shows as Balls.
    public var displayedObjectType: ObjectType {
        actualEngine == .juggleMaster && objectType == .default ? .ball : objectType
    }

    public func setPaused(_ paused: Bool) {
        engine.isPaused = paused
        isPaused = engine.isPaused
    }

    public func togglePause() {
        setPaused(!isPaused)
    }

    // MARK: Camera

    /// Rotates by a drag delta normalized to the view size; dragging stops auto-rotation.
    public func orbit(dx: Float, dy: Float) {
        setIfChanged(\.cameraPreset, nil)
        if autoRotate {
            setAutoRotate(false)
        }
        engine.orbit(dx: dx, dy: dy)
    }

    public func setCameraPreset(_ preset: CameraPreset) {
        cameraPreset = preset
        engine.setCameraPreset(preset)
        autoRotate = engine.autoRotate
        state.autoRotate = autoRotate
        persist()
    }

    public var canZoomIn: Bool { zoom < Self.zoomRange.upperBound - 0.005 }
    public var canZoomOut: Bool { zoom > Self.zoomRange.lowerBound + 0.005 }

    public func zoomIn() { zoom(by: Self.zoomStep) }
    public func zoomOut() { zoom(by: -Self.zoomStep) }

    public func zoom(by delta: Float) {
        engine.zoom(by: delta)
        zoom = engine.zoom
    }

    public func resetZoom() {
        engine.resetZoom()
        zoom = engine.zoom
    }

    // MARK: Persistence

    /// Saves the state; while previewing, the pattern from before browsing stands in for the preview.
    public func persist() {
        var saved = state
        if let origin = previewOrigin?.state {
            saved.name = origin.name
            saved.pattern = origin.pattern
            saved.style = origin.style
            saved.fromLibrary = origin.fromLibrary
            saved.categoryId = origin.categoryId
            saved.object = origin.object
        }
        storage.set(saved.query, forKey: Self.stateKey)
    }

    /// Link to the web app with the current settings, including an uncommitted speed.
    public var shareURL: URL {
        var current = state
        current.speed = speed
        return current.webLink
    }
}

extension Comparable {
    fileprivate func clamped(to range: ClosedRange<Self>) -> Self {
        min(max(self, range.lowerBound), range.upperBound)
    }
}
