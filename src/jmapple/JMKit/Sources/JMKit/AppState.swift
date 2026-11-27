import Foundation

/// The settings the web app keeps in its query string (port of `useUrlParams.ts`). Used for share links,
/// deep links and restoring the last state on launch.
public struct AppState: Equatable, Sendable {
    public enum Mode: String, Sendable {
        case twoD = "2d"
        case threeD = "3d"
    }

    /// The `object` query parameter; raw values of the engine's object types.
    public enum Object: String, Sendable, CaseIterable {
        case `default` = ""
        case ball, club, ring, fish

        public var engineValue: Int32 {
            switch self {
            case .default: 0
            case .ball: 1
            case .club: 2
            case .ring: 3
            case .fish: 6
            }
        }

        public init(engineValue: Int32) {
            self = Self.allCases.first { $0.engineValue == engineValue } ?? .default
        }
    }

    public static let webURL = URL(string: "https://jugglemaster.net/")!
    public static let speedRange = 0.1...10.0
    public static let defaultSpeed = 3.0

    public var pattern = "3"
    public var name = "3 Cascade"
    public var style = PatternDefaults.style
    public var mode = Mode.threeD
    public var showSiteswap = false
    public var autoRotate = false
    public var fromLibrary = false
    public var categoryId = ""
    public var object = Object.default
    public var speed = AppState.defaultSpeed

    public init() {}

    /// Reads a query string (with or without the leading "?"); missing or invalid values take the defaults.
    public init(query: String) {
        let params = Self.decodeQuery(query)
        func value(_ key: String) -> String? { params.first { $0.key == key }?.value }

        if let pattern = value("pattern") { self.pattern = pattern }
        name = value("name") ?? value("pattern") ?? name
        if let style = value("style") { self.style = style }
        mode = value("mode") == "2d" ? .twoD : .threeD
        showSiteswap = value("siteswap") == "1"
        autoRotate = value("autorotate") == "1"
        fromLibrary = value("lib") == "1"
        categoryId = value("cat") ?? ""
        object = value("object").flatMap { $0.isEmpty ? nil : Object(rawValue: $0) } ?? .default
        if let raw = value("speed"), let speed = Double(raw.trimmingCharacters(in: .whitespaces)),
            Self.speedRange.contains(speed)
        {
            self.speed = speed
        }
    }

    /// Reads the query of a web share link or a `jugglemaster://` deep link.
    public init(url: URL) {
        self.init(query: URLComponents(url: url, resolvingAgainstBaseURL: false)?.percentEncodedQuery ?? "")
    }

    /// Query string without "?", omitting defaults exactly like the web app.
    public var query: String {
        let defaults = AppState()
        var params: [(String, String)] = []
        if pattern != defaults.pattern { params.append(("pattern", pattern)) }
        if name != pattern && name != defaults.name { params.append(("name", name)) }
        if style != defaults.style { params.append(("style", style)) }
        if mode != defaults.mode { params.append(("mode", mode.rawValue)) }
        if showSiteswap { params.append(("siteswap", "1")) }
        if autoRotate { params.append(("autorotate", "1")) }
        if fromLibrary { params.append(("lib", "1")) }
        if !categoryId.isEmpty { params.append(("cat", categoryId)) }
        if object != .default { params.append(("object", object.rawValue)) }
        if speed != defaults.speed { params.append(("speed", Self.formatNumber(speed))) }
        return params.map { "\(Self.formEncode($0.0))=\(Self.formEncode($0.1))" }.joined(separator: "&")
    }

    /// Link to the web app with this state.
    public var webLink: URL {
        let query = query
        return query.isEmpty ? Self.webURL : URL(string: "\(Self.webURL.absoluteString)?\(query)")!
    }

    /// `String(number)` in JavaScript for the speed range: no trailing ".0".
    static func formatNumber(_ value: Double) -> String {
        value == value.rounded() && abs(value) < 1e15 ? String(Int(value)) : String(value)
    }

    /// `application/x-www-form-urlencoded`, as `URLSearchParams.toString()` writes it.
    static func formEncode(_ text: String) -> String {
        var result = ""
        for byte in text.utf8 {
            switch byte {
            case UInt8(ascii: "a")...UInt8(ascii: "z"), UInt8(ascii: "A")...UInt8(ascii: "Z"),
                UInt8(ascii: "0")...UInt8(ascii: "9"), UInt8(ascii: "*"), UInt8(ascii: "-"), UInt8(ascii: "."),
                UInt8(ascii: "_"):
                result.unicodeScalars.append(Unicode.Scalar(byte))
            case UInt8(ascii: " "):
                result.append("+")
            default:
                result += String(format: "%%%02X", byte)
            }
        }
        return result
    }

    /// Pairs in order, decoded like `URLSearchParams`: "+" is a space, then percent-decoding.
    static func decodeQuery(_ query: String) -> [(key: String, value: String)] {
        let trimmed = query.hasPrefix("?") ? String(query.dropFirst()) : query
        return trimmed.split(separator: "&", omittingEmptySubsequences: true).map { pair in
            let parts = pair.split(separator: "=", maxSplits: 1, omittingEmptySubsequences: false)
            let decode = { (part: Substring) -> String in
                let spaced = part.replacingOccurrences(of: "+", with: " ")
                return spaced.removingPercentEncoding ?? spaced
            }
            return (decode(parts[0]), parts.count > 1 ? decode(parts[1]) : "")
        }
    }
}
