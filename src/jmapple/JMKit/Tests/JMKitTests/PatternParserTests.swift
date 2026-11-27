import Foundation
import Testing

@testable import JMKit

struct PatternParserTests {
    @Test func juggleMasterCategoriesStylesAndRates() {
        let library = PatternLibrary.parseJuggleMaster(
            """
            ; comment
            531 Ignored before any category
            /[ Basics ]
            #HR=2.5
            #DR=.05
            3\tCascade
            %Mills Mess
            {ignored}
            441
            /[ Basics ]
            #HR=0.3
            (4,4) Sync fountain
            !bad
            """)
        #expect(library.categories == [Category(id: "basics", name: "Basics", source: .jm)])
        #expect(library.patterns.map(\.name) == ["Cascade", "441", "Sync fountain"])
        #expect(library.patterns.map(\.style) == ["Normal", "Mills Mess", "Normal"])
        #expect(library.patterns[0].hr == PatternDefaults.hrRange.upperBound)
        #expect(library.patterns[0].dr == PatternDefaults.drRange.lowerBound)
        #expect(library.patterns[2].hr == 0.3)
        #expect(library.patterns.map(\.id) == ["basics-cascade-3-0", "basics-441-441-1", "basics-sync-fountain-4-4-2"])
    }

    @Test func juggleMasterHandlesCRLFAndEmptyNames() {
        let library = PatternLibrary.parseJuggleMaster("/[ !!! ]\r\n3  \r\n")
        #expect(library.categories.first?.id == "category-0")
        #expect(library.patterns.first?.name == "3")
        #expect(library.patterns.first?.data == "3")
    }

    @Test func juggleSaverGroupsAuthorsAndMultilinePatterns() {
        let library = PatternLibrary.parseJuggleSaver(
            """

            $ 3 Object patterns
            $ ------------------
            "Shower (Someone)" 51;
            "Split" 3b@(0,0)
              >(1,0);
            $ (5 entries)
            $ Clubs
            "" 3c;
            """)
        #expect(
            library.categories.map(\.name) == [
                "3 Object patterns (JuggleSaver)", "3 Object patterns - Clubs (JuggleSaver)",
            ])
        #expect(library.patterns.map(\.name) == ["Shower", "Split", "3c"])
        #expect(library.patterns[1].data == "3b@(0,0)   >(1,0)")
        #expect(library.patterns.allSatisfy { $0.style == "JuggleSaver" && $0.source == .js })
        #expect(library.patterns[2].id == "js-3-object-patterns-clubs-jugglesaver-p-2")
    }

    @Test func slugMatchesWebRules() {
        #expect(slug("  3-Cascade Step By Step ") == "3-cascade-step-by-step")
        #expect(slug("(4,4)(4x,0)") == "4-4-4x-0")
        #expect(slug("!!!") == "")
    }

    @Test func juggleSaverDetectionAndEffectiveStyle() {
        let jm = Pattern(
            id: "a", name: "a", data: "3", style: "Reverse", categoryName: "Basics", categoryId: "b", source: .jm)
        var tagged = jm
        tagged.categoryName = "Imported JuggleSaver"
        #expect(!jm.isJuggleSaver)
        #expect(jm.effectiveStyle == "Reverse")
        #expect(tagged.isJuggleSaver)
        #expect(tagged.effectiveStyle == "JuggleSaver")
    }

    @Test func searchFiltersBySourceAndCategory() {
        let library = PatternLibrary(
            juggleMasterText: "/[ A ]\n3 Cascade\n531\n/[ B ]\n3 Cascade too",
            juggleSaverText: "$ G\n\"Cascade\" 3b;")
        #expect(library.search(" CASCADE ").count == 3)
        #expect(library.search("cascade", source: .js).count == 1)
        #expect(library.search("cascade", categoryId: "a").map(\.data) == ["3"])
        #expect(library.search("53").map(\.data) == ["531"])
        #expect(library.categories(for: .jm).map(\.id) == ["a", "b"])
        #expect(library.patterns(in: "b").count == 1)
        #expect(library.category(id: "js-g-jugglesaver")?.name == "G (JuggleSaver)")
    }

    @Test func decodeTextDropsBOM() {
        #expect(PatternLibrary.decodeText(Data([0xEF, 0xBB, 0xBF, 0x33])) == "3")
    }
}
