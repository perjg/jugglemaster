import Foundation
import Testing

@testable import JMKit

/// Compares the Swift parsers with the TypeScript ones on the real pattern files.
/// Regenerate the fixture with `node scripts/dump-patterns.mjs <path>` in src/jmwasm/ui.
struct PatternLibraryParityTests {
    struct Fixture: Decodable {
        struct Search: Decodable {
            let query: String
            let source: String
            let ids: [String]
        }

        let library: PatternLibrary
        let filters: [Search]
    }

    static let dataDirectory = URL(fileURLWithPath: #filePath)
        .deletingLastPathComponent().appendingPathComponent("../../../../../data").standardized

    static func loadFixture() throws -> Fixture {
        let url = try #require(
            Bundle.module.url(forResource: "library", withExtension: "json", subdirectory: "Fixtures"))
        return try JSONDecoder().decode(Fixture.self, from: Data(contentsOf: url))
    }

    static func loadLibrary() throws -> PatternLibrary {
        let jm = try Data(contentsOf: dataDirectory.appendingPathComponent("patterns.jm"))
        let js = try Data(contentsOf: dataDirectory.appendingPathComponent("moresites.txt"))
        return PatternLibrary(
            juggleMasterText: PatternLibrary.decodeText(jm), juggleSaverText: PatternLibrary.decodeText(js))
    }

    @Test func categoriesMatchTypeScript() throws {
        let expected = try Self.loadFixture().library.categories
        let actual = try Self.loadLibrary().categories
        #expect(actual.count == expected.count)
        for (a, e) in zip(actual, expected) {
            #expect(a == e)
        }
    }

    @Test func patternsMatchTypeScript() throws {
        let expected = try Self.loadFixture().library.patterns
        let actual = try Self.loadLibrary().patterns
        #expect(actual.count == expected.count)
        for (a, e) in zip(actual, expected) where a != e {
            Issue.record("\(a) != \(e)")
        }
    }

    @Test func searchMatchesTypeScript() throws {
        let fixture = try Self.loadFixture()
        let library = try Self.loadLibrary()
        for search in fixture.filters {
            let source = try #require(SourceFilter(rawValue: search.source))
            let ids = library.search(search.query, source: source).map(\.id)
            #expect(ids == search.ids, "query '\(search.query)' source \(search.source)")
        }
    }
}
