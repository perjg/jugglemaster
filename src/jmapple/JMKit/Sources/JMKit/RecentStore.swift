import Foundation
import Observation

/// An entry of a recent list; `isSameEntry` decides which entries replace each other.
public protocol RecentItem: Codable, Equatable, Sendable {
    var isFavorite: Bool { get set }
    func isSameEntry(as other: Self) -> Bool
}

public struct RecentSiteswap: RecentItem, Hashable {
    public var site: String
    public var style: String
    public var isFavorite: Bool

    public init(site: String, style: String, isFavorite: Bool = false) {
        self.site = site
        self.style = style
        self.isFavorite = isFavorite
    }

    public func isSameEntry(as other: RecentSiteswap) -> Bool {
        site == other.site
    }

    private enum CodingKeys: String, CodingKey {
        case site, style
        case isFavorite = "favorite"
    }

    public init(from decoder: any Decoder) throws {
        let container = try decoder.container(keyedBy: CodingKeys.self)
        site = try container.decode(String.self, forKey: .site)
        style = try container.decode(String.self, forKey: .style)
        isFavorite = try container.decodeIfPresent(Bool.self, forKey: .isFavorite) ?? false
    }
}

public struct RecentPattern: RecentItem, Hashable {
    public var name: String
    public var data: String
    public var style: String
    public var categoryId: String
    public var categoryName: String
    public var isFavorite: Bool

    public init(
        name: String, data: String, style: String, categoryId: String, categoryName: String, isFavorite: Bool = false
    ) {
        self.name = name
        self.data = data
        self.style = style
        self.categoryId = categoryId
        self.categoryName = categoryName
        self.isFavorite = isFavorite
    }

    public init(_ pattern: Pattern) {
        self.init(
            name: pattern.name, data: pattern.data, style: pattern.effectiveStyle, categoryId: pattern.categoryId,
            categoryName: pattern.categoryName)
    }

    public func isSameEntry(as other: RecentPattern) -> Bool {
        categoryId == other.categoryId && data == other.data
    }

    private enum CodingKeys: String, CodingKey {
        case name, data, style, categoryId, categoryName
        case isFavorite = "favorite"
    }

    public init(from decoder: any Decoder) throws {
        let container = try decoder.container(keyedBy: CodingKeys.self)
        name = try container.decode(String.self, forKey: .name)
        data = try container.decode(String.self, forKey: .data)
        style = try container.decode(String.self, forKey: .style)
        categoryId = try container.decode(String.self, forKey: .categoryId)
        categoryName = try container.decode(String.self, forKey: .categoryName)
        isFavorite = try container.decodeIfPresent(Bool.self, forKey: .isFavorite) ?? false
    }
}

/// Persistence for `RecentStore`; `UserDefaults` in the apps.
public protocol KeyValueStore: AnyObject {
    func data(forKey key: String) -> Data?
    func string(forKey key: String) -> String?
    func set(_ value: Any?, forKey key: String)
}

extension UserDefaults: KeyValueStore {}

/// Most recent first, favorites ahead of the rest, at most `maxItems` (port of the web's useRecentItems).
@MainActor
@Observable
public final class RecentStore<Item: RecentItem> {
    public static var defaultMaxItems: Int { 10 }

    public private(set) var items: [Item]

    private let key: String
    private let maxItems: Int
    @ObservationIgnored private let storage: any KeyValueStore

    public init(key: String, storage: any KeyValueStore = UserDefaults.standard, maxItems: Int = defaultMaxItems) {
        self.key = key
        self.storage = storage
        self.maxItems = maxItems
        items = storage.data(forKey: key).flatMap { try? JSONDecoder().decode([Item].self, from: $0) } ?? []
    }

    /// Moves `item` to the front; re-adding a favorite keeps it a favorite.
    public func push(_ item: Item) {
        var item = item
        if let existing = items.first(where: { $0.isSameEntry(as: item) }), existing.isFavorite {
            item.isFavorite = true
        }
        let rest = items.filter { !$0.isSameEntry(as: item) }
        update(Array(Self.favoritesFirst([item] + rest).prefix(maxItems)))
    }

    public func remove(_ item: Item) {
        update(items.filter { !$0.isSameEntry(as: item) })
    }

    public func toggleFavorite(_ item: Item) {
        update(
            Self.favoritesFirst(
                items.map { entry in
                    guard entry.isSameEntry(as: item) else { return entry }
                    var toggled = entry
                    toggled.isFavorite.toggle()
                    return toggled
                }))
    }

    private func update(_ next: [Item]) {
        items = next
        storage.set(try? JSONEncoder().encode(next), forKey: key)
    }

    private static func favoritesFirst(_ items: [Item]) -> [Item] {
        items.filter(\.isFavorite) + items.filter { !$0.isFavorite }
    }
}

extension RecentStore where Item == RecentSiteswap {
    /// Same storage key as the web app.
    public static func siteswaps(storage: any KeyValueStore = UserDefaults.standard) -> RecentStore {
        RecentStore(key: "jm_recent_siteswaps", storage: storage)
    }
}

extension RecentStore where Item == RecentPattern {
    /// Same storage key as the web app.
    public static func patterns(storage: any KeyValueStore = UserDefaults.standard) -> RecentStore {
        RecentStore(key: "jm_recent_patterns", storage: storage)
    }
}
