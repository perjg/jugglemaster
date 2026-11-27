import Foundation
import Testing

@testable import JMKit

final class MemoryStore: KeyValueStore {
    var values: [String: Any] = [:]
    func data(forKey key: String) -> Data? { values[key] as? Data }
    func string(forKey key: String) -> String? { values[key] as? String }
    func set(_ value: Any?, forKey key: String) { values[key] = value }
}

@MainActor
struct RecentStoreTests {
    let storage = MemoryStore()

    func siteswap(_ site: String, favorite: Bool = false) -> RecentSiteswap {
        RecentSiteswap(site: site, style: "Normal", isFavorite: favorite)
    }

    @Test func pushMovesToFrontDedupesAndCaps() {
        let store = RecentStore<RecentSiteswap>(key: "k", storage: storage, maxItems: 3)
        for site in ["3", "441", "531", "3", "51"] {
            store.push(siteswap(site))
        }
        #expect(store.items.map(\.site) == ["51", "3", "531"])
    }

    @Test func favoritesStayFirstAndSurviveRepush() {
        let store = RecentStore<RecentSiteswap>(key: "k", storage: storage)
        store.push(siteswap("3"))
        store.push(siteswap("441"))
        store.toggleFavorite(siteswap("3"))
        #expect(store.items.map(\.site) == ["3", "441"])
        store.push(siteswap("531"))
        #expect(store.items.map(\.site) == ["3", "531", "441"])
        store.push(siteswap("3"))
        #expect(store.items.first?.isFavorite == true)
        store.toggleFavorite(siteswap("3"))
        #expect(store.items.map(\.site) == ["3", "531", "441"])
        #expect(store.items.allSatisfy { !$0.isFavorite })
    }

    @Test func removeAndPersistence() {
        let store = RecentStore<RecentSiteswap>(key: "k", storage: storage)
        store.push(siteswap("3"))
        store.push(siteswap("441", favorite: true))
        store.remove(siteswap("3"))
        let reloaded = RecentStore<RecentSiteswap>(key: "k", storage: storage)
        #expect(reloaded.items == [siteswap("441", favorite: true)])
    }

    @Test func patternsAreKeyedByCategoryAndData() {
        let store = RecentStore<RecentPattern>.patterns(storage: storage)
        store.push(RecentPattern(name: "A", data: "3", style: "Normal", categoryId: "x", categoryName: "X"))
        store.push(RecentPattern(name: "B", data: "3", style: "Normal", categoryId: "y", categoryName: "Y"))
        store.push(RecentPattern(name: "A again", data: "3", style: "Normal", categoryId: "x", categoryName: "X"))
        #expect(store.items.map(\.name) == ["A again", "B"])
        #expect(storage.data(forKey: "jm_recent_patterns") != nil)
    }

    @Test func readsWebShapedJSONWithoutFavoriteFlag() throws {
        storage.values["jm_recent_siteswaps"] = Data(
            #"[{"site":"531","style":"Normal"},{"site":"3","style":"JuggleSaver","favorite":true}]"#.utf8)
        let store = RecentStore<RecentSiteswap>.siteswaps(storage: storage)
        #expect(store.items == [siteswap("531"), RecentSiteswap(site: "3", style: "JuggleSaver", isFavorite: true)])
    }

    @Test func corruptStorageStartsEmpty() {
        storage.values["k"] = Data("not json".utf8)
        #expect(RecentStore<RecentSiteswap>(key: "k", storage: storage).items.isEmpty)
    }

    @Test func recentPatternFromLibraryPattern() {
        let pattern = Pattern(
            id: "i", name: "Box", data: "(4,2x)(2x,4)", style: "Normal", categoryName: "Moresites (JuggleSaver)",
            categoryId: "c", source: .jm)
        let recent = RecentPattern(pattern)
        #expect(recent.style == "JuggleSaver")
        #expect(recent.categoryName == "Moresites (JuggleSaver)")
    }
}
