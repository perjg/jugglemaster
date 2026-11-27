import Foundation
import Testing

@testable import JugglerKit

final class MemoryStore: KeyValueStore {
    var values: [String: Any] = [:]
    func data(forKey key: String) -> Data? { values[key] as? Data }
    func string(forKey key: String) -> String? { values[key] as? String }
    func set(_ value: Any?, forKey key: String) { values[key] = value }
}

extension EngineSuites {
    @MainActor
    @Suite struct JugglerModelRules {
        let storage = MemoryStore()
        let target = EngineSuites.prepare()
        let engine = Engine.shared

        static let juggleSaverOnly = "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)"

        func makeModel(_ query: String = "") -> JugglerModel {
            makeModel(AppState(query: query))
        }

        func makeModel(_ state: AppState) -> JugglerModel {
            let model = JugglerModel(initialState: state, storage: storage)
            model.engineDidStart()
            return model
        }

        @Test func initialStateComesFromQuery() {
            let model = makeModel("pattern=531&style=JuggleSaver&mode=2d&object=club&speed=4.5&autorotate=1&siteswap=1")
            #expect(model.preferredEngine == .juggleSaver)
            #expect(model.selectedStyle == "Normal")
            #expect(model.renderMode == .twoD)
            #expect(model.objectType == .club)
            #expect(model.speed == 4.5)
            #expect(model.autoRotate)
            #expect(model.showSiteswap)
            #expect(engine.patternName == "531")
            #expect(engine.speed == 4.5)
            #expect(model.actualEngine == engine.engineType)
        }

        @Test func loadingADeepLinkAppliesAllSettings() {
            let model = makeModel()
            model.load(AppState(query: "pattern=441&style=Shower&mode=2d&object=ring&speed=5&siteswap=1"))
            #expect(engine.patternName == "441" && engine.style == "Shower")
            #expect(model.renderMode == .twoD && model.objectType == .ring && engine.objectType == .ring)
            #expect(model.speed == 5 && engine.speed == 5)
            #expect(model.showSiteswap && model.selectedStyle == "Shower")
            #expect(JugglerModel(storage: storage).state.pattern == "441")
        }

        @Test func rejectedInitialPatternFallsBackToDefault() {
            let model = makeModel("pattern=54&style=JuggleSaver&cat=x&lib=1")
            #expect(engine.patternName == "3 Cascade")
            #expect(model.state.pattern == "3" && model.state.style == "Normal" && !model.state.fromLibrary)
            #expect(model.preferredEngine == .juggleMaster)
            #expect(model.actualEngine == .juggleMaster)
        }

        @Test func juggleSaverPatternFromQuerySelectsJuggleSaver() {
            var state = AppState()
            state.pattern = Self.juggleSaverOnly
            state.style = "JuggleSaver"
            let model = makeModel(state)
            #expect(model.actualEngine == .juggleSaver)
            #expect(!model.engineMismatch)
            #expect(!model.isStyleEnabled)
        }

        @Test func invalidSiteswapIsIgnored() {
            let model = makeModel()
            #expect(!model.submitSiteswap("54"))
            #expect(!model.submitSiteswap("   "))
            #expect(engine.patternName == "3 Cascade")
            #expect(model.recentSiteswaps.items.isEmpty)
        }

        @Test func siteswapFallsBackToTheOtherEngine() {
            let model = makeModel("style=Reverse")
            #expect(model.submitSiteswap(" \(Self.juggleSaverOnly) "))
            #expect(model.actualEngine == .juggleSaver)
            #expect(model.preferredEngine == .juggleMaster)
            #expect(model.engineMismatch)
            #expect(model.objectType == .default)
            #expect(model.state.pattern == Self.juggleSaverOnly)
            #expect(model.state.style == "Reverse")
            #expect(!model.state.fromLibrary)
            #expect(model.recentSiteswaps.items.first == RecentSiteswap(site: Self.juggleSaverOnly, style: "Reverse"))
        }

        @Test func juggleSaverPreferenceAppliesToSiteswaps() {
            let model = makeModel()
            model.setPreferredEngine(.juggleSaver)
            #expect(model.actualEngine == .juggleSaver)
            #expect(model.state.style == "JuggleSaver")
            #expect(model.submitSiteswap("531"))
            #expect(model.actualEngine == .juggleSaver)
            #expect(model.recentSiteswaps.items.first?.style == "JuggleSaver")
            model.setPreferredEngine(.juggleMaster)
            #expect(model.actualEngine == .juggleMaster)
            #expect(engine.style == "Normal")
        }

        @Test func styleOnlyMattersForJuggleMaster() {
            let model = makeModel()
            model.setStyle("Mills Mess")
            #expect(engine.style == "Mills Mess")
            #expect(model.state.style == "Mills Mess")
            #expect(model.isStyleEnabled)
        }

        @Test func libraryPatternsApplyStyleAfterPattern() {
            let model = makeModel()
            let jsPattern = Pattern(
                id: "js-x-1", name: "Bounce", data: Self.juggleSaverOnly, style: "JuggleSaver",
                categoryName: "X (JuggleSaver)", categoryId: "js-x", source: .js)
            let jmPattern = Pattern(
                id: "m-1", name: "Mess", data: "3", style: "Mills Mess", categoryName: "Basics", categoryId: "basics",
                source: .jm)

            model.setObjectType(.ring)
            model.apply(jsPattern)
            #expect(model.actualEngine == .juggleSaver)
            #expect(model.objectType == .default)
            #expect(model.state.fromLibrary && model.state.categoryId == "js-x" && model.state.name == "Bounce")
            #expect(model.recentPatterns.items.map(\.name) == ["Bounce"])

            model.setObjectType(.ring)
            model.preview(jmPattern)
            #expect(model.actualEngine == .juggleMaster)
            #expect(engine.style == "Mills Mess")
            #expect(model.selectedStyle == "Mills Mess")
            #expect(model.objectType == .ring)
            #expect(model.recentPatterns.items.map(\.name) == ["Bounce"])

            model.applyRecent(RecentPattern(jsPattern))
            #expect(model.actualEngine == .juggleSaver)
        }

        @Test func previewRevertsOrCommits() {
            let model = makeModel("pattern=531&style=Reverse&object=club")
            let jsPattern = Pattern(
                id: "js-x-1", name: "Bounce", data: Self.juggleSaverOnly, style: "JuggleSaver",
                categoryName: "X (JuggleSaver)", categoryId: "js-x", source: .js)
            let jmPattern = Pattern(
                id: "m-1", name: "Mess", data: "3", style: "Mills Mess", categoryName: "Basics", categoryId: "basics",
                source: .jm)

            model.preview(jsPattern)
            model.preview(jmPattern)
            #expect(model.isPreviewing)
            #expect(engine.style == "Mills Mess")
            #expect(model.recentPatterns.items.isEmpty)
            model.cancelPreview()
            #expect(!model.isPreviewing)
            #expect(engine.patternName == "531")
            #expect(engine.style == "Reverse")
            #expect(model.selectedStyle == "Reverse")
            #expect(model.objectType == .club && engine.objectType == .club)
            #expect(model.state.pattern == "531" && !model.state.fromLibrary)

            model.preview(jsPattern)
            model.commitPreview(jsPattern)
            #expect(!model.isPreviewing)
            #expect(model.actualEngine == .juggleSaver)
            #expect(model.recentPatterns.items.map(\.name) == ["Bounce"])
            model.cancelPreview()
            #expect(model.state.pattern == Self.juggleSaverOnly)

            model.commitPreview(jmPattern)
            #expect(engine.style == "Mills Mess")
            #expect(model.recentPatterns.items.map(\.name) == ["Mess", "Bounce"])
        }

        @Test func previewIdentityAndObjectIsolation() {
            let model = makeModel("object=club")
            let mill = Pattern(
                id: "t-1", name: "Tims Multi Mill", data: "6464", style: "Windmill", categoryName: "T",
                categoryId: "t", source: .jm)
            let mess = Pattern(
                id: "t-2", name: "Tims Multi Mess", data: "6464", style: "Mills Mess", categoryName: "T",
                categoryId: "t", source: .jm)
            let saver = Pattern(
                id: "js-1", name: "Bounce", data: Self.juggleSaverOnly, style: "JuggleSaver", categoryName: "J",
                categoryId: "js", source: .js)

            model.preview(saver)
            #expect(model.objectType == .default)
            model.preview(mill)
            #expect(model.objectType == .club)
            model.commitPreview(mess)
            #expect(engine.style == "Mills Mess")
            #expect(model.state.name == "Tims Multi Mess")
            #expect(model.objectType == .club)
            #expect(!model.isPreviewing)
        }

        @Test func explicitChoicesEndAPreview() {
            let model = makeModel("pattern=531")
            let mill = Pattern(
                id: "t-1", name: "Mill", data: "6464", style: "Windmill", categoryName: "T", categoryId: "t",
                source: .jm)
            model.preview(mill)
            model.applyRecent(
                RecentPattern(name: "441", data: "441", style: "Normal", categoryId: "x", categoryName: "X"))
            #expect(!model.isPreviewing)
            model.cancelPreview()
            #expect(engine.patternName == "441")
        }

        @Test func patternsTheEngineRefusesAreRejected() {
            let model = makeModel("pattern=531")
            #expect(model.validate("[03]")?.isValid == false)
            #expect(!model.submitSiteswap("[03]"))
            #expect(engine.patternName == "531")
            #expect(model.validate("3b@(" + String(repeating: "1", count: 40) + ",0)>(1,0)")?.isValid == false)
        }

        @Test func rejectedLibraryPatternsChangeNothing() {
            let model = makeModel("pattern=531")
            let rejected = Pattern(
                id: "js-r", name: "Circle", data: "3c*-1", style: "JuggleSaver", categoryName: "J", categoryId: "js",
                source: .js)
            #expect(!model.apply(rejected))
            #expect(!model.applyRecent(RecentPattern(rejected)))
            #expect(model.recentPatterns.items.isEmpty)
            #expect(model.state.pattern == "531" && engine.site == "531")

            let mill = Pattern(
                id: "t-1", name: "Mill", data: "6464", style: "Windmill", categoryName: "T", categoryId: "t",
                source: .jm)
            model.preview(mill)
            model.preview(rejected)
            #expect(!model.commitPreview(rejected))
            #expect(model.isPreviewing)
            #expect(model.recentPatterns.items.isEmpty)
            model.cancelPreview()
            #expect(engine.site == "531")
        }

        @Test func previewsAreNotSaved() {
            let model = makeModel("pattern=531&object=club")
            let saver = Pattern(
                id: "js-1", name: "Bounce", data: Self.juggleSaverOnly, style: "JuggleSaver", categoryName: "J",
                categoryId: "js", source: .js)
            model.preview(saver)
            model.setSpeed(2)
            let relaunched = JugglerModel(storage: storage)
            #expect(relaunched.state.pattern == "531")
            #expect(relaunched.objectType == .club)
            #expect(relaunched.speed == 2)
            model.commitPreview(saver)
            #expect(JugglerModel(storage: storage).state.pattern == Self.juggleSaverOnly)
        }

        @Test func juggleSaverPreferenceKeepsAJuggleSaverLibraryPattern() {
            let model = makeModel()
            let pattern = Pattern(
                id: "js-441", name: "441", data: "441", style: "JuggleSaver", categoryName: "J", categoryId: "js",
                source: .js)
            model.apply(pattern)
            #expect(model.actualEngine == .juggleSaver)
            model.setPreferredEngine(.juggleSaver)
            #expect(model.actualEngine == .juggleSaver && engine.engineType == .juggleSaver)
        }

        @Test func cameraPresetIsTracked() {
            let model = makeModel()
            model.setCameraPreset(.left)
            #expect(model.cameraPreset == .left)
            model.orbit(dx: 0.1, dy: 0)
            #expect(model.cameraPreset == nil)
            model.setCameraPreset(.left)
            model.submitSiteswap("531")
            #expect(model.cameraPreset == .front)
        }

        @Test func recentSiteswapRestoresEnginePreference() {
            let model = makeModel()
            model.applyRecent(RecentSiteswap(site: "3", style: "JuggleSaver"))
            #expect(model.preferredEngine == .juggleSaver)
            #expect(model.actualEngine == .juggleSaver)
            model.applyRecent(RecentSiteswap(site: "441", style: "Shower"))
            #expect(model.preferredEngine == .juggleMaster)
            #expect(model.selectedStyle == "Shower")
            #expect(model.actualEngine == .juggleMaster)
            #expect(model.recentSiteswaps.items.map(\.site) == ["441", "3"])
        }

        @Test func objectTypesPerEngine() {
            let model = makeModel()
            #expect(model.availableObjectTypes == [.ball, .club, .ring, .fish])
            #expect(model.displayedObjectType == .ball)
            model.setObjectType(.fish)
            #expect(engine.objectType == .fish)
            #expect(model.state.object == .fish)
            model.submitSiteswap(Self.juggleSaverOnly)
            #expect(model.availableObjectTypes.first == .default)
            #expect(model.displayedObjectType == .default)
        }

        @Test func cameraInteractionStopsAutoRotation() {
            let model = makeModel("autorotate=1")
            #expect(model.autoRotate)
            model.orbit(dx: 0.1, dy: 0)
            #expect(!model.autoRotate)
            #expect(!engine.autoRotate)
            model.toggleAutoRotate()
            #expect(model.autoRotate && model.state.autoRotate)
            model.setCameraPreset(.high)
            #expect(!model.autoRotate && !model.state.autoRotate)
        }

        @Test func speedRoundsClampsAndCommits() {
            let model = makeModel()
            model.setSpeed(2.34, commit: false)
            #expect(abs(model.speed - 2.3) < 1e-9)
            #expect(model.state.speed == AppState.defaultSpeed)
            #expect(model.shareURL.absoluteString.contains("speed=2.3"))
            model.setSpeed(42)
            #expect(model.speed == 10)
            #expect(model.state.speed == 10)
            model.setSpeed(0)
            #expect(model.speed == 0.1)
        }

        @Test(arguments: [(2.9, "2.9"), (0.7, "0.7"), (2.34, "2.3")])
        func speedIsSharedWithoutFloatNoise(value: Double, query: String) {
            let model = makeModel()
            model.setSpeed(value)
            let items = URLComponents(url: model.shareURL, resolvingAgainstBaseURL: false)?.queryItems ?? []
            #expect(items.first { $0.name == "speed" }?.value == query)
        }

        @Test func rejectedRestoredStateIsReplaced() {
            _ = makeModel("pattern=54")
            #expect(JugglerModel(storage: storage).state.pattern == "3")
        }

        @Test func customLibraryStylesKeepThePickerOnAKnownStyle() {
            let model = makeModel("style=Reverse")
            let tennis = Pattern(
                id: "t", name: "Tennis", data: "3", style: "Tennis", categoryName: "T", categoryId: "t", source: .jm)
            model.apply(tennis)
            #expect(model.selectedStyle == "Reverse")
            #expect(model.state.style == "Tennis")
            model.submitSiteswap("441")
            #expect(engine.style == "Reverse")
            #expect(model.recentSiteswaps.items.first?.style == "Reverse")
            #expect(makeModel("style=Tennis").selectedStyle == "Normal")
        }

        @Test func uncommittedSpeedIsSavedOnceItSettles() async throws {
            let model = makeModel()
            model.setSpeed(4.2, commit: false)
            #expect(model.state.speed == AppState.defaultSpeed)
            try await Task.sleep(for: .milliseconds(800))
            #expect(abs(model.state.speed - 4.2) < 1e-9)
            #expect(JugglerModel(storage: storage).speed == model.state.speed)
        }

        @Test func zoomLimits() {
            let model = makeModel()
            model.setRenderMode(.threeD)
            for _ in 0..<40 { model.zoomIn() }
            #expect(!model.canZoomIn)
            for _ in 0..<40 { model.zoomOut() }
            #expect(!model.canZoomOut)
            model.resetZoom()
            #expect(model.canZoomIn && model.canZoomOut)
        }

        @Test func stateIsSavedAndRestored() {
            let model = makeModel()
            model.submitSiteswap("531")
            model.setRenderMode(.twoD)
            model.showSiteswap = true
            let restored = JugglerModel(storage: storage)
            #expect(restored.state.pattern == "531")
            #expect(restored.renderMode == .twoD)
            #expect(restored.showSiteswap)
            #expect(restored.recentSiteswaps.items.map(\.site) == ["531"])
            #expect(JugglerModel(initialState: AppState(), storage: storage).state == AppState())
        }

        @Test func framesPublishTheActiveThrow() {
            let model = makeModel()
            model.submitSiteswap("97531")
            target?.bind()
            var ranges: Set<Int> = []
            for _ in 0..<90 {
                engine.frame()
                model.frameDidRender()
                ranges.insert(model.siteswap.range.lowerBound)
                #expect(model.siteswap.site == "97531")
            }
            #expect(ranges.count > 1)
        }

        @Test func pauseAndValidation() {
            let model = makeModel()
            model.togglePause()
            #expect(model.isPaused && engine.isPaused)
            model.togglePause()
            #expect(!model.isPaused)
            #expect(model.validate("  ") == nil)
            #expect(model.validate("531") == SiteswapValidation(juggleMaster: true, juggleSaver: true, isValid: true))
            #expect(
                model.validate(Self.juggleSaverOnly)
                    == SiteswapValidation(juggleMaster: false, juggleSaver: true, isValid: true))
            let tooLong = String(repeating: "3", count: 57)
            #expect(
                model.validate(tooLong) == SiteswapValidation(juggleMaster: false, juggleSaver: true, isValid: false))
            #expect(!model.submitSiteswap(tooLong))
            #expect(model.validate("54")?.isValid == false)
        }

        @Test func libraryLoadsFromBundle() async throws {
            let model = makeModel()
            await model.loadLibrary(from: Bundle(for: MemoryStore.self))
            #expect(model.libraryFailed)
            model.setLibrary(PatternLibrary(categories: [], patterns: []))
            #expect(!model.libraryFailed && model.library != nil)
        }
    }
}

struct SiteswapDisplayTests {
    @Test func splitsAroundTheActiveThrow() {
        let display = SiteswapDisplay(site: "(4,2x)(2x,4)", range: 1..<2)
        #expect(display.before == "(")
        #expect(display.active == "4")
        #expect(display.after == ",2x)(2x,4)")
        #expect(SiteswapDisplay().active.isEmpty)
    }
}
