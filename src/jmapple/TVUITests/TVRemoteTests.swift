import XCTest

/// Drives the tvOS app with the Siri Remote in the simulator.
@MainActor
final class TVRemoteTests: XCTestCase {
    private var app: XCUIApplication!
    private let remote = XCUIRemote.shared

    override func setUp() async throws {
        continueAfterFailure = false
        app = XCUIApplication()
        app.launchArguments = ["-jm_last_state", ""]
        app.launchEnvironment["JM_CONTROLS_TIMEOUT"] = "5"
        app.launch()
    }

    private var focused: XCUIElement {
        app.descendants(matching: .any).element(matching: NSPredicate(format: "hasFocus == true"))
    }

    /// Saves a screenshot when JM_SNAPSHOT_DIR is set, for looking at the UI.
    private func snapshot(_ name: String) {
        guard let directory = ProcessInfo.processInfo.environment["JM_SNAPSHOT_DIR"] else { return }
        let url = URL(fileURLWithPath: directory).appendingPathComponent("\(name).png")
        try? XCUIScreen.main.screenshot().pngRepresentation.write(to: url)
    }

    private func element(_ id: String) -> XCUIElement {
        app.descendants(matching: .any)[id]
    }

    /// Moves focus with `direction` until `id` is focused.
    private func focus(_ id: String, moving direction: XCUIRemote.Button, maxPresses: Int = 15) {
        for _ in 0..<maxPresses where focused.identifier != id {
            remote.press(direction)
        }
        XCTAssertEqual(focused.identifier, id)
    }

    /// Moves focus with `direction` until an element labelled `label` is focused (list rows).
    private func focus(label: String, moving direction: XCUIRemote.Button, maxPresses: Int = 30) {
        for _ in 0..<maxPresses where focused.label != label {
            remote.press(direction)
        }
        XCTAssertEqual(focused.label, label)
    }

    func testClickShowsControlsWithFrontPresetFocusedAndBackHidesThem() {
        XCTAssertFalse(element("settings").exists)
        remote.press(.select)
        XCTAssertTrue(element("settings").waitForExistence(timeout: 2))
        XCTAssertEqual(focused.identifier, "patterns")
        snapshot("tv-controls")
        remote.press(.menu)
        XCTAssertTrue(element("settings").waitForNonExistence(timeout: 2))
    }

    func testControlsHideAfterTimeout() {
        remote.press(.select)
        XCTAssertTrue(element("settings").waitForExistence(timeout: 2))
        XCTAssertTrue(element("settings").waitForNonExistence(timeout: 9))
    }

    func testPlayPauseTogglesPause() {
        remote.press(.playPause)
        XCTAssertTrue(element("pausedIndicator").waitForExistence(timeout: 2))
        remote.press(.playPause)
        XCTAssertTrue(element("pausedIndicator").waitForNonExistence(timeout: 2))
    }

    func testSettingsPanelOpensAndBackReturnsToControls() {
        remote.press(.select)
        XCTAssertTrue(element("settings").waitForExistence(timeout: 2))
        focus("settings", moving: .right)
        remote.press(.select)
        XCTAssertTrue(element("mode").waitForExistence(timeout: 2))

        snapshot("tv-settings")
        focus("speedDown", moving: .down)
        focus("speedUp", moving: .right)
        let before = element("speedValue").label
        remote.press(.select)
        XCTAssertNotEqual(element("speedValue").label, before)

        remote.press(.menu)
        XCTAssertTrue(element("mode").waitForNonExistence(timeout: 2))
        XCTAssertTrue(element("settings").exists)
    }

    func testSharePanelShowsQRCode() {
        remote.press(.select)
        XCTAssertTrue(element("share").waitForExistence(timeout: 2))
        focus("share", moving: .right)
        remote.press(.select)
        XCTAssertTrue(element("shareQR").waitForExistence(timeout: 2))
        snapshot("tv-share")
    }

    /// Relaunches with the siteswap overlay on and the controls shown, so tests can read the juggled pattern.
    private func showSiteswapOverlay() {
        app.terminate()
        app.launchArguments = ["-jm_last_state", "siteswap=1"]
        app.launch()
        remote.press(.select)
        XCTAssertTrue(element("settings").waitForExistence(timeout: 3))
    }

    private var overlaySite: String {
        element("siteswapOverlay").label
    }

    func testBrowserPreviewsOnFocusAndBackReverts() {
        showSiteswapOverlay()
        XCTAssertEqual(overlaySite, "3")
        focus("patterns", moving: .left)
        remote.press(.select)
        let category = app.buttons["3-Cascade Step By Step"]
        XCTAssertTrue(category.waitForExistence(timeout: 5))
        focus(label: "3-Cascade Step By Step", moving: .down)
        remote.press(.select)
        XCTAssertTrue(
            app.buttons.matching(NSPredicate(format: "identifier BEGINSWITH 'pattern-'")).firstMatch
                .waitForExistence(timeout: 3))
        remote.press(.down)
        let previewed = NSPredicate(format: "label != '3'")
        expectation(for: previewed, evaluatedWith: element("siteswapOverlay"))
        waitForExpectations(timeout: 3)
        snapshot("tv-browser")

        remote.press(.menu)
        remote.press(.menu)
        XCTAssertTrue(category.waitForNonExistence(timeout: 2))
        XCTAssertEqual(overlaySite, "3")
    }

    func testBrowserSelectCommitsPattern() {
        showSiteswapOverlay()
        focus("patterns", moving: .left)
        remote.press(.select)
        XCTAssertTrue(app.buttons["3-Cascade Step By Step"].waitForExistence(timeout: 5))
        focus(label: "3-Cascade Step By Step", moving: .down)
        remote.press(.select)
        XCTAssertTrue(
            app.buttons.matching(NSPredicate(format: "identifier BEGINSWITH 'pattern-'")).firstMatch
                .waitForExistence(timeout: 3))
        remote.press(.down)
        remote.press(.select)
        XCTAssertTrue(app.buttons["3-Cascade Step By Step"].waitForNonExistence(timeout: 2))
        XCTAssertFalse(element("settings").exists)
        XCTAssertNotEqual(overlaySite, "3")
    }

    func testKeypadEntersAndJugglesSiteswap() {
        showSiteswapOverlay()
        focus("siteswap", moving: .right)
        remote.press(.select)
        XCTAssertTrue(element("key-5").waitForExistence(timeout: 2))
        snapshot("tv-keypad")
        for key in ["5", "3", "1"] {
            focus("key-\(key)", moving: key == "5" ? .right : .left)
            remote.press(.select)
        }
        XCTAssertTrue(element("juggle").isEnabled)
        for _ in 0..<6 where !["delete", "clear", "keyboard", "juggle"].contains(focused.identifier) {
            remote.press(.down)
        }
        focus("juggle", moving: .right)
        remote.press(.select)
        XCTAssertTrue(element("key-5").waitForNonExistence(timeout: 2))
        XCTAssertEqual(overlaySite, "531")
    }

    func testCameraMenuSelectsPreset() {
        remote.press(.select)
        XCTAssertTrue(element("share").waitForExistence(timeout: 2))
        focus("camera", moving: .right)
        remote.press(.select)
        let left = element("preset-1")
        XCTAssertTrue(left.waitForExistence(timeout: 2))
        snapshot("tv-camera-menu")
        remote.press(.up)
        focus("preset-1", moving: .left, maxPresses: 6)
        remote.press(.select)
        XCTAssertTrue(left.waitForNonExistence(timeout: 3))
        XCTAssertEqual(focused.identifier, "camera")
        XCTAssertEqual(focused.label, "Camera, Left")
    }

    func testJugglerMovesBesideTheSidePanel() {
        showSiteswapOverlay()
        let overlay = element("siteswapOverlay")
        let centeredMidX = overlay.frame.midX
        focus("patterns", moving: .left)
        remote.press(.select)
        XCTAssertTrue(app.buttons["3-Cascade Step By Step"].waitForExistence(timeout: 5))
        let besideMidX = overlay.frame.midX
        XCTAssertGreaterThan(besideMidX, centeredMidX + 200)
        snapshot("tv-beside-panel")
    }

    func testSearchIsFullScreenAndJugglesAResult() {
        showSiteswapOverlay()
        focus("patterns", moving: .left)
        remote.press(.select)
        XCTAssertTrue(element("search").waitForExistence(timeout: 5))
        focus("search", moving: .right)
        remote.press(.select)
        let field = app.searchFields.firstMatch
        XCTAssertTrue(field.waitForExistence(timeout: 3))
        XCTAssertGreaterThan(field.frame.width, app.frame.width * 0.6)
        field.typeText("33022")
        let result = app.buttons.matching(NSPredicate(format: "identifier BEGINSWITH 'result-'")).firstMatch
        XCTAssertTrue(result.waitForExistence(timeout: 5))
        snapshot("tv-search")
        // Typing through XCUITest into the tvOS search keyboard can drop characters, so find the row
        for _ in 0..<12 where !focused.label.contains("33022") {
            remote.press(.down)
        }
        XCTAssertTrue(focused.label.contains("33022"))
        remote.press(.select)
        expectation(for: NSPredicate(format: "label == '33022'"), evaluatedWith: element("siteswapOverlay"))
        waitForExpectations(timeout: 5)
    }
}
