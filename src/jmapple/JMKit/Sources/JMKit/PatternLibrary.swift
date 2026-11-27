import Foundation

/// Which pattern file a pattern or category came from.
public enum PatternSource: String, Codable, Sendable, CaseIterable {
    case jm
    case js
}

/// Source filter of the pattern browser; `nil` source means all.
public enum SourceFilter: String, Sendable, CaseIterable {
    case all
    case jm
    case js

    var source: PatternSource? {
        switch self {
        case .all: nil
        case .jm: .jm
        case .js: .js
        }
    }
}

public struct Pattern: Codable, Hashable, Identifiable, Sendable {
    public var id: String
    public var name: String
    public var data: String
    public var style: String
    public var categoryName: String
    public var categoryId: String
    public var hr: Double
    public var dr: Double
    public var source: PatternSource

    public init(
        id: String, name: String, data: String, style: String, categoryName: String, categoryId: String,
        hr: Double = PatternDefaults.hr, dr: Double = PatternDefaults.dr, source: PatternSource
    ) {
        self.id = id
        self.name = name
        self.data = data
        self.style = style
        self.categoryName = categoryName
        self.categoryId = categoryId
        self.hr = hr
        self.dr = dr
        self.source = source
    }

    /// JuggleSaver patterns are applied with the "JuggleSaver" style.
    public var isJuggleSaver: Bool {
        source == .js || categoryName.contains("JuggleSaver")
    }

    /// Style to apply after setting the pattern.
    public var effectiveStyle: String {
        isJuggleSaver ? PatternDefaults.juggleSaverStyle : style
    }
}

public struct Category: Codable, Hashable, Identifiable, Sendable {
    public var id: String
    public var name: String
    public var source: PatternSource

    public init(id: String, name: String, source: PatternSource) {
        self.id = id
        self.name = name
        self.source = source
    }
}

/// Limits and defaults from src/jmlib/jmlib.h.
public enum PatternDefaults {
    public static let hr = 0.17
    public static let hrRange = 0.04...1.0
    public static let dr = 0.5
    public static let drRange = 0.1...0.9
    public static let style = "Normal"
    public static let juggleSaverStyle = "JuggleSaver"
}

public struct PatternLibrary: Codable, Sendable {
    public var categories: [Category]
    public var patterns: [Pattern]

    public init(categories: [Category], patterns: [Pattern]) {
        self.categories = categories
        self.patterns = patterns
    }

    /// Parses both pattern files; JuggleMaster entries come first.
    public init(juggleMasterText: String, juggleSaverText: String) {
        let jm = Self.parseJuggleMaster(juggleMasterText)
        let js = Self.parseJuggleSaver(juggleSaverText)
        self.init(categories: jm.categories + js.categories, patterns: jm.patterns + js.patterns)
    }

    /// Loads `patterns.jm` and `moresites.txt` from `bundle`.
    public static func load(from bundle: Bundle = .main) throws -> PatternLibrary {
        func text(_ name: String, _ ext: String) throws -> String {
            guard let url = bundle.url(forResource: name, withExtension: ext) else {
                throw CocoaError(.fileNoSuchFile, userInfo: [NSFilePathErrorKey: "\(name).\(ext)"])
            }
            return decodeText(try Data(contentsOf: url))
        }
        return PatternLibrary(
            juggleMasterText: try text("patterns", "jm"), juggleSaverText: try text("moresites", "txt"))
    }

    /// UTF-8 with replacement characters and without a BOM, like a browser's `Response.text()`.
    public static func decodeText(_ data: Data) -> String {
        var text = String(decoding: data, as: UTF8.self)
        if text.hasPrefix("\u{FEFF}") {
            text.removeFirst()
        }
        return text
    }

    public func categories(for filter: SourceFilter) -> [Category] {
        guard let source = filter.source else { return categories }
        return categories.filter { $0.source == source }
    }

    public func patterns(in categoryId: String) -> [Pattern] {
        patterns.filter { $0.categoryId == categoryId }
    }

    public func category(id: String) -> Category? {
        categories.first { $0.id == id }
    }

    /// Partial, case-insensitive match on name and data across the library.
    public func search(_ text: String, source: SourceFilter = .all, categoryId: String? = nil) -> [Pattern] {
        let query = trimJS(text).lowercased()
        return patterns.filter { pattern in
            (source.source == nil || pattern.source == source.source)
                && (categoryId == nil || pattern.categoryId == categoryId)
                && (query.isEmpty || pattern.name.lowercased().contains(query)
                    || pattern.data.lowercased().contains(query))
        }
    }
}

// MARK: - Shared parsing helpers

/// `String.prototype.trim()`: strips whitespace and line terminators (incl. BOM).
func trimJS(_ text: String) -> String {
    text.trimmingCharacters(in: jsWhitespace)
}

private let jsWhitespace = CharacterSet.whitespacesAndNewlines.union(CharacterSet(charactersIn: "\u{FEFF}"))

/// Lowercase, runs of anything but a–z/0–9 become "-", no leading/trailing "-".
func slug(_ text: String) -> String {
    var result = ""
    var pendingDash = false
    for scalar in trimJS(text).lowercased().unicodeScalars {
        if ("a"..."z").contains(scalar) || ("0"..."9").contains(scalar) {
            if pendingDash, !result.isEmpty {
                result.append("-")
            }
            pendingDash = false
            result.unicodeScalars.append(scalar)
        } else {
            pendingDash = true
        }
    }
    return result
}

/// `String.prototype.slice(0, length)`, which counts UTF-16 code units.
func prefixUTF16(_ text: String, _ length: Int) -> String {
    String(decoding: Array(text.utf16.prefix(length)), as: UTF16.self)
}

/// Lines split on `\n` (minus a trailing `\r`), scanning scalars since "\r\n" is a single Character.
func splitLines(_ text: String) -> [String] {
    text.unicodeScalars.split(separator: "\n", omittingEmptySubsequences: false).map { line in
        var line = String.UnicodeScalarView(line)
        if line.last == "\r" {
            line.removeLast()
        }
        return String(line)
    }
}
