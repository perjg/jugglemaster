import Foundation
import Testing

@testable import JMKit

struct AppStateTests {
    @Test func defaultsProduceEmptyQueryAndBareWebLink() {
        #expect(AppState().query == "")
        #expect(AppState().webLink.absoluteString == "https://jugglemaster.net/")
    }

    @Test func writesOnlyNonDefaultsInWebOrder() {
        var state = AppState()
        state.pattern = "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)"
        state.name = "Bounce, mine"
        state.style = "JuggleSaver"
        state.mode = .twoD
        state.showSiteswap = true
        state.autoRotate = true
        state.fromLibrary = true
        state.categoryId = "js-bounce"
        state.object = .club
        state.speed = 2.5
        let pattern = "3b%40%28-0.5%2C0%29%3E%281.5%2C0%29+3b%40%280.5%2C0%29%3E%28-1.5%2C0%29"
        #expect(
            state.query
                == "pattern=\(pattern)&name=Bounce%2C+mine&style=JuggleSaver&mode=2d&siteswap=1&autorotate=1"
                + "&lib=1&cat=js-bounce&object=club&speed=2.5")
    }

    @Test func jugglerColoursAreOnUnlessTurnedOff() {
        #expect(AppState(query: "pattern=J2%3A966").colorJugglers)
        #expect(!AppState(query: "pattern=J2%3A966&jcolors=0").colorJugglers)
        var state = AppState()
        state.pattern = "4.5p 3 3"
        #expect(state.query == "pattern=4.5p+3+3")
        state.colorJugglers = false
        #expect(state.query == "pattern=4.5p+3+3&jcolors=0")
        #expect(!AppState(query: state.query).colorJugglers)
    }

    @Test func feedWidthsAreSharedAndRead() {
        var state = AppState()
        #expect(state.query.isEmpty)
        state.feedWidth = 2
        #expect(state.query == "feed=2")
        #expect(AppState(query: "feed=2").feedWidth == 2)
        #expect(AppState(query: "feed=9").feedWidth == AppState.defaultFeedWidth)
        #expect(AppState(query: "").feedWidth == AppState.defaultFeedWidth)
        state.feedWidth = AppState.defaultFeedWidth
        #expect(state.query.isEmpty)
    }

    @Test func nameEqualToPatternIsOmitted() {
        var state = AppState()
        state.pattern = "531"
        state.name = "531"
        #expect(state.query == "pattern=531")
    }

    @Test func roundTrips() {
        var state = AppState()
        state.pattern = "(4,2x)(2x,4)"
        state.name = "Box & co"
        state.style = "Mills Mess"
        state.object = .fish
        state.speed = 10
        state.categoryId = "x"
        #expect(AppState(query: state.query) == state)
        #expect(AppState(url: state.webLink) == state)
    }

    @Test func readsWebURLs() throws {
        let url = try #require(
            URL(string: "https://jugglemaster.net/?pattern=531&style=Reverse&mode=2d&object=ring&speed=0.5&siteswap=1"))
        let state = AppState(url: url)
        #expect(state.pattern == "531")
        #expect(state.name == "531")
        #expect(state.style == "Reverse")
        #expect(state.mode == .twoD)
        #expect(state.object == .ring)
        #expect(state.object.engineValue == 3)
        #expect(state.speed == 0.5)
        #expect(state.showSiteswap)
        #expect(!state.autoRotate)
    }

    @Test func invalidValuesFallBackToDefaults() {
        let state = AppState(query: "?mode=4d&object=knife&speed=12&siteswap=yes&autorotate=0")
        #expect(state == AppState())
        #expect(AppState(query: "speed=abc").speed == AppState.defaultSpeed)
        #expect(AppState(query: "speed=").speed == AppState.defaultSpeed)
    }

    @Test func deepLinkAndPlusDecoding() throws {
        let url = try #require(URL(string: "jugglemaster://open?pattern=3b+3b&name=Two%20words"))
        let state = AppState(url: url)
        #expect(state.pattern == "3b 3b")
        #expect(state.name == "Two words")
    }

    @Test func objectEngineValues() {
        #expect(AppState.Object.allCases.map(\.engineValue) == [0, 1, 2, 3, 6])
        #expect(AppState.Object(engineValue: 6) == .fish)
        #expect(AppState.Object(engineValue: 4) == .default)
    }

    @Test func formatsSpeedLikeJavaScript() {
        #expect(AppState.formatNumber(3) == "3")
        #expect(AppState.formatNumber(0.1) == "0.1")
        #expect(AppState.formatNumber(2.5) == "2.5")
    }
}
