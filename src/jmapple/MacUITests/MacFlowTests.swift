import XCTest

/// Main flow of the Mac app: sidebar menu next to the juggler.
@MainActor
final class MacFlowTests: XCTestCase {
    func testTypedSiteswapIsJuggled() {
        let app = XCUIApplication()
        app.launchArguments = ["-jm_last_state", "siteswap=1"]
        app.launch()
        let field = app.descendants(matching: .any)["siteswapField"]
        XCTAssertTrue(field.waitForExistence(timeout: 5))
        field.click()
        field.typeText("441\r")
        let overlay = app.descendants(matching: .any)["siteswapOverlay"]
        expectation(for: NSPredicate(format: "label == '441'"), evaluatedWith: overlay)
        waitForExpectations(timeout: 5)
    }
}
