import Foundation
import JMKit

private final class BundleToken {}

extension Bundle {
    /// Holds `Localizable.xcstrings` shared by all apps.
    public static let jugglerKit = Bundle(for: BundleToken.self)
}

/// Localized string from JugglerKit's catalog; keys mirror the web app's i18n keys.
public func jmLocalized(_ key: String.LocalizationValue) -> String {
    String(localized: key, bundle: .jugglerKit)
}

extension CameraPreset {
    /// Left to right as shown in the presets bar (web order).
    public static let displayOrder: [CameraPreset] = [.left, .back, .front, .high, .right]

    public var title: String {
        switch self {
        case .left: jmLocalized("camera.left")
        case .back: jmLocalized("camera.back")
        case .front: jmLocalized("camera.front")
        case .high: jmLocalized("camera.high")
        case .right: jmLocalized("camera.right")
        }
    }
}

extension ObjectType {
    public var title: String {
        switch self {
        case .default: jmLocalized("menu.objectDefault")
        case .ball: jmLocalized("menu.objectBalls")
        case .club: jmLocalized("menu.objectClubs")
        case .ring: jmLocalized("menu.objectRings")
        case .fish: jmLocalized("menu.objectFish")
        }
    }
}

extension EngineType {
    public var title: String {
        switch self {
        case .juggleMaster: jmLocalized("menu.engineJuggleMaster")
        case .juggleSaver: jmLocalized("menu.engineJuggleSaver")
        }
    }
}

extension SourceFilter {
    public var title: String {
        switch self {
        case .all: jmLocalized("menu.filterAll")
        case .jm: jmLocalized("menu.filterJm")
        case .js: jmLocalized("menu.filterJs")
        }
    }
}
