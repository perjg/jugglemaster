import Testing

@testable import JugglerKit

#if canImport(UIKit)
import OpenGLES
#else
import OpenGL.GL
#endif

/// Engine is a process-wide singleton: every suite that touches it nests in here so they run serially.
@MainActor
@Suite(.serialized)
struct EngineSuites {
    static let width = 800
    static let height = 600

    /// One render target for the whole run, so tests don't accumulate GPU allocations.
    static let sharedTarget: MultisampleTarget? = {
        GLContext.shared.makeCurrent()
        return MultisampleTarget(width: GLsizei(width), height: GLsizei(height))
    }()

    /// Starts the engine with the shared 800×600 target bound and the harness defaults of tests/visual.
    @discardableResult
    static func prepare() -> MultisampleTarget? {
        GLContext.shared.makeCurrent()
        let target = sharedTarget
        target?.bind()
        let engine = Engine.shared
        engine.start(width: width, height: height)
        engine.resize(width: width, height: height)
        engine.isPaused = false
        engine.setFixedFrameRate(60)
        engine.speed = Engine.defaultSpeed
        engine.objectType = .default
        engine.autoRotate = false
        engine.setCameraPreset(.front)
        engine.setRenderMode(.twoD)
        engine.setPattern(name: "3 Cascade", site: "3")
        engine.setStyle("Normal")
        return target
    }
}

extension EngineSuites {
    @MainActor
    @Suite struct EngineAPI {
        let target = EngineSuites.prepare()
        let engine = Engine.shared

        @Test func startsAndReportsDefaults() throws {
            #expect(try #require(target).samples > 0)
            #expect(engine.isStarted)
            #expect(engine.engineType == .juggleMaster)
            #expect(engine.patternName == "3 Cascade")
            #expect(engine.style == "Normal")
        }

        @Test func siteswapPatternSelectsJuggleMaster() {
            engine.setPattern(name: "531", site: "531")
            #expect(engine.engineType == .juggleMaster)
            #expect(engine.patternName == "531")
            #expect(engine.site == "531")
        }

        @Test func juggleSaverOnlyPatternSelectsJuggleSaver() {
            engine.setPattern(name: "Bounce", site: "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)")
            #expect(engine.engineType == .juggleSaver)
            #expect(engine.style == "JuggleSaver")
            #expect(engine.patternName == "Bounce")
        }

        @Test func juggleSaverStyleSwitchesEngineBothWays() {
            engine.setPattern(name: "3", site: "3")
            engine.setStyle("JuggleSaver")
            #expect(engine.engineType == .juggleSaver)
            #expect(engine.patternName == "3")
            engine.setStyle("Reverse")
            #expect(engine.engineType == .juggleMaster)
            #expect(engine.style == "Reverse")
        }

        @Test func juggleSaverStyleKeepsJuggleSaver() {
            engine.setPattern(name: "441", site: "441")
            engine.setStyle("JuggleSaver")
            engine.setStyle("JuggleSaver")
            #expect(engine.engineType == .juggleSaver)
        }

        @Test func longestSiteswapAndRandomStyle() {
            let longest = String(repeating: "3", count: 56)
            #expect(engine.setPattern(name: String(repeating: "n", count: 56), site: longest))
            #expect(engine.site == longest)
            engine.setStyle("Random")
            #expect(engine.style == "Random")
            #expect(engine.site == longest)
        }

        @Test(arguments: [
            ("531", true, true),
            ("(4,2x)(2x,4)", true, false),
            ("3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)", false, true),
            ("54", false, false),
            (String(repeating: "3", count: 57), false, true),
            (String(repeating: "(4,4)", count: 260), false, false),
            ("", false, false),
        ])
        func validators(pattern: String, juggleMaster: Bool, juggleSaver: Bool) {
            #expect(engine.isValidJuggleMasterPattern(pattern) == juggleMaster)
            #expect(engine.isValidJuggleSaverPattern(pattern) == juggleSaver)
            #expect(engine.isValidPattern(pattern) == ((juggleMaster || juggleSaver) && pattern.count <= 56))
        }

        @Test func pausedFramesDoNotAdvance() {
            engine.setPattern(name: "97531", site: "97531")
            engine.isPaused = true
            let before = engine.siteRange
            for _ in 0..<120 { engine.frame() }
            #expect(engine.siteRange == before)
            engine.isPaused = false
            var seen: Set<Int> = [before.lowerBound]
            for _ in 0..<120 {
                engine.frame()
                seen.insert(engine.siteRange.lowerBound)
            }
            #expect(seen.count > 1)
        }

        @Test func siteRangeStaysInsideSite() {
            engine.setPattern(name: "(4,2x)(2x,4)", site: "(4,2x)(2x,4)")
            for _ in 0..<60 {
                engine.frame()
                let range = engine.siteRange
                #expect(range.lowerBound >= 0 && range.upperBound <= engine.site.count)
            }
        }

        @Test func juggleSaverSiteRangeFollowsEachThrow() {
            engine.setPattern(name: "531", site: "531")
            engine.setStyle("JuggleSaver")
            #expect(engine.engineType == .juggleSaver)
            var seen: Set<Int> = []
            for _ in 0..<240 {
                engine.frame()
                let range = engine.siteRange
                #expect(range.count == 1 && range.upperBound <= engine.site.count)
                seen.insert(range.lowerBound)
            }
            #expect(seen == [0, 1, 2])
        }

        @Test func longJuggleSaverSiteRangeStaysInsideSite() {
            let site = String(repeating: "3c", count: 140)
            #expect(engine.setPattern(name: site, site: site))
            #expect(engine.engineType == .juggleSaver)
            engine.speed = 10
            var highest = 0
            for _ in 0..<600 {
                engine.frame()
                let range = engine.siteRange
                #expect(range.count == 1 && range.upperBound <= engine.site.count)
                highest = max(highest, range.lowerBound)
            }
            #expect(highest > 127)
        }

        @Test func rejectedPatternsKeepTheCurrentOne() {
            #expect(engine.setPattern(name: "531", site: "531"))
            #expect(!engine.setPattern(name: "bad", site: "54"))
            #expect(!engine.setPattern(name: "long", site: String(repeating: "3", count: 57)))
            #expect(engine.patternName == "531" && engine.site == "531")
            let longName = String(repeating: "n", count: 80)
            #expect(engine.setPattern(name: longName, site: "441"))
            #expect(engine.patternName == longName && engine.site == "441")
        }

        @Test func speedIsClamped() {
            engine.speed = 20
            #expect(engine.speed == Engine.speedRange.upperBound)
            engine.speed = 0
            #expect(engine.speed == Engine.speedRange.lowerBound)
            engine.speed = 2.5
            #expect(engine.speed == 2.5)
        }

        @Test func zoomIsClampedAndResets() {
            engine.setRenderMode(.threeD)
            engine.resetZoom()
            let initial = engine.zoom
            engine.zoom(by: 10)
            #expect(engine.zoom <= 0.9 + 0.0001)
            engine.zoom(by: -10)
            #expect(engine.zoom >= 0.01 - 0.0001)
            engine.resetZoom()
            #expect(abs(engine.zoom - initial) < 0.0001)
        }

        @Test func objectTypeRoundTrips() {
            for type in ObjectType.allCases {
                engine.objectType = type
                #expect(engine.objectType == type)
            }
        }

        @Test func autoRotateRoundTripsAndPresetsDraw() {
            engine.setRenderMode(.threeD)
            engine.autoRotate = true
            #expect(engine.autoRotate)
            engine.autoRotate = false
            #expect(!engine.autoRotate)
            for preset in CameraPreset.allCases {
                engine.setCameraPreset(preset)
                engine.orbit(dx: 0.1, dy: -0.05)
                engine.draw()
            }
            #expect(!engine.autoRotate)
        }
    }
}
