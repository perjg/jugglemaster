import XCTest

/// Main flows on iPad (sidebar) and iPhone (menu sheet).
@MainActor
final class AppFlowTests: XCTestCase {
    private var app: XCUIApplication!

    override func setUp() async throws {
        continueAfterFailure = false
        app = XCUIApplication()
        app.launchArguments = ["-jm_last_state", ""]
        app.launch()
    }

    private var isPhone: Bool { UIDevice.current.userInterfaceIdiom == .phone }

    private func element(_ id: String) -> XCUIElement {
        app.descendants(matching: .any)[id]
    }

    /// Saves a screenshot when JM_SNAPSHOT_DIR is set, for looking at the UI.
    private func snapshot(_ name: String) {
        guard let directory = ProcessInfo.processInfo.environment["JM_SNAPSHOT_DIR"] else { return }
        let device = isPhone ? "iphone" : "ipad"
        let url = URL(fileURLWithPath: directory).appendingPathComponent("\(device)-\(name).png")
        try? XCUIScreen.main.screenshot().pngRepresentation.write(to: url)
    }

    private func openMenu() {
        if isPhone {
            element("menuButton").tap()
        }
        XCTAssertTrue(element("siteswapField").waitForExistence(timeout: 3))
    }

    /// Relaunches with the siteswap overlay on, so tests can read the juggled pattern.
    private func relaunchShowingSiteswap() {
        app.terminate()
        app.launchArguments = ["-jm_last_state", "siteswap=1"]
        app.launch()
    }

    func testTypedSiteswapIsJuggled() {
        relaunchShowingSiteswap()
        openMenu()
        let field = element("siteswapField")
        field.tap()
        field.typeText("531\n")
        let overlay = element("siteswapOverlay")
        XCTAssertTrue(overlay.waitForExistence(timeout: 3))
        expectation(for: NSPredicate(format: "label == '531'"), evaluatedWith: overlay)
        waitForExpectations(timeout: 8)
        snapshot("menu")
    }

    func testLibraryPatternIsJuggled() {
        relaunchShowingSiteswap()
        openMenu()
        app.buttons["Pattern"].tap()
        let category = element("category")
        XCTAssertTrue(category.waitForExistence(timeout: 5))
        category.tap()
        app.buttons["3-Cascade Step By Step"].tap()
        let row = app.buttons.containing(NSPredicate(format: "label CONTAINS 'Throw Twice'")).firstMatch
        XCTAssertTrue(row.waitForExistence(timeout: 3))
        row.tap()
        expectation(for: NSPredicate(format: "label == '33022'"), evaluatedWith: element("siteswapOverlay"))
        waitForExpectations(timeout: 8)
        snapshot("library")
    }

    func testShareShowsQRCode() {
        element("share").tap()
        XCTAssertTrue(element("shareQR").waitForExistence(timeout: 3))
        snapshot("share")
    }

    func testDeepLinkLoadsState() {
        app.open(URL(string: "jugglemaster://open?pattern=441&siteswap=1&mode=2d")!)
        let overlay = element("siteswapOverlay")
        XCTAssertTrue(overlay.waitForExistence(timeout: 5))
        expectation(for: NSPredicate(format: "label == '441'"), evaluatedWith: overlay)
        waitForExpectations(timeout: 8)
        snapshot("deeplink")
    }

    func testLandscapeMenuIsASidePanel() throws {
        try XCTSkipUnless(isPhone, "compact layout")
        XCUIDevice.shared.orientation = .landscapeLeft
        defer { XCUIDevice.shared.orientation = .portrait }
        element("menuButton").tap()
        XCTAssertTrue(element("siteswapField").waitForExistence(timeout: 3))
        snapshot("landscape")
        XCTAssertFalse(element("menuButton").exists)
        app.buttons["Close"].tap()
        XCTAssertTrue(element("menuButton").waitForExistence(timeout: 3))
    }
}
