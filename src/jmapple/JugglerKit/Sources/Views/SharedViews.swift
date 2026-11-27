import CoreImage
import CoreImage.CIFilterBuiltins
import JMKit
import SwiftUI

/// Translucent backing used by the overlays on top of the juggler (web: rgba(0,0,0,0.45)).
public let overlayBackground = Color.black.opacity(0.45)
/// The highlighted throw in the siteswap overlay (web: #ff4444).
public let activeThrowColor = Color(red: 1, green: 0x44 / 255, blue: 0x44 / 255)

/// The current siteswap with the throw being animated in red.
public struct SiteswapOverlay: View {
    let display: SiteswapDisplay
    let fontSize: CGFloat

    public init(display: SiteswapDisplay, fontSize: CGFloat = 16) {
        self.display = display
        self.fontSize = fontSize
    }

    public var body: some View {
        if !display.site.isEmpty {
            (Text(display.before) + Text(display.active).foregroundStyle(activeThrowColor) + Text(display.after))
                .font(.system(size: fontSize, design: .monospaced))
                .foregroundStyle(.white)
                .padding(.horizontal, fontSize * 0.75)
                .padding(.vertical, fontSize * 0.25)
                .background(overlayBackground, in: RoundedRectangle(cornerRadius: 4))
                .accessibilityElement(children: .ignore)
                .accessibilityLabel(display.site)
        }
    }
}

/// Left, Back, Front, High, Right.
public struct CameraPresetsBar: View {
    let model: JugglerModel

    public init(model: JugglerModel) {
        self.model = model
    }

    public var body: some View {
        HStack(spacing: 2) {
            ForEach(CameraPreset.displayOrder, id: \.self) { preset in
                Button(preset.title) { model.setCameraPreset(preset) }
                    .font(.system(size: 12, design: .monospaced))
                    .padding(.horizontal, 8)
                    .padding(.vertical, 2)
            }
        }
        .buttonStyle(.plain)
        .foregroundStyle(.white)
        .padding(3)
        .background(overlayBackground, in: RoundedRectangle(cornerRadius: 4))
    }
}

/// Zoom in, out and reset, disabled at the engine's limits.
public struct ZoomControls: View {
    let model: JugglerModel

    public init(model: JugglerModel) {
        self.model = model
    }

    public var body: some View {
        VStack(spacing: 2) {
            iconButton("plus.magnifyingglass", label: jmLocalized("zoom.zoomIn"), enabled: model.canZoomIn) {
                model.zoomIn()
            }
            iconButton("minus.magnifyingglass", label: jmLocalized("zoom.zoomOut"), enabled: model.canZoomOut) {
                model.zoomOut()
            }
            iconButton("arrow.counterclockwise", label: jmLocalized("zoom.zoomReset"), enabled: true) {
                model.resetZoom()
            }
        }
        .padding(3)
        .background(overlayBackground, in: RoundedRectangle(cornerRadius: 4))
    }

    private func iconButton(_ symbol: String, label: String, enabled: Bool, action: @escaping () -> Void) -> some View {
        Button(action: action) {
            Image(systemName: symbol)
                .font(.system(size: 16))
                .frame(width: 26, height: 26)
        }
        .buttonStyle(.plain)
        .foregroundStyle(.white)
        .opacity(enabled ? 1 : 0.35)
        .disabled(!enabled)
        .accessibilityLabel(label)
        .help(label)
    }
}

/// Object type buttons; Default only appears for JuggleSaver patterns.
public struct ObjectTypePicker: View {
    let model: JugglerModel
    let iconSize: CGFloat

    public init(model: JugglerModel, iconSize: CGFloat = 36) {
        self.model = model
        self.iconSize = iconSize
    }

    public var body: some View {
        HStack(spacing: 4) {
            ForEach(model.availableObjectTypes, id: \.self) { type in
                let selected = model.displayedObjectType == type
                Button {
                    model.setObjectType(type)
                } label: {
                    ObjectIcon(type)
                        .frame(width: iconSize, height: iconSize)
                        .frame(maxWidth: .infinity)
                        .overlay(alignment: .bottom) {
                            if selected {
                                Capsule().frame(width: iconSize * 0.6, height: 3).offset(y: 6)
                            }
                        }
                }
                .accessibilityLabel(type.title)
                .accessibilityAddTraits(selected ? .isSelected : [])
                .modifier(SelectableButtonStyle(selected: selected))
            }
        }
    }
}

/// Prominent when selected, bordered otherwise (the web's default/outline buttons).
struct SelectableButtonStyle: ViewModifier {
    let selected: Bool

    func body(content: Content) -> some View {
        if selected {
            content.buttonStyle(.borderedProminent)
        } else {
            content.buttonStyle(.bordered)
        }
    }
}

/// Title and detail of a recent siteswap.
public struct RecentSiteswapLabel: View {
    let item: RecentSiteswap

    public init(_ item: RecentSiteswap) {
        self.item = item
    }

    public var body: some View {
        HStack {
            Text(item.site).font(.body.monospaced()).lineLimit(1)
            Spacer(minLength: 8)
            Text(item.style).font(.caption).foregroundStyle(.secondary)
        }
    }
}

/// Title and category of a recent library pattern.
public struct RecentPatternLabel: View {
    let item: RecentPattern

    public init(_ item: RecentPattern) {
        self.item = item
    }

    public var body: some View {
        VStack(alignment: .leading, spacing: 2) {
            Text(item.name == item.data ? item.name : "\(item.name) (\(item.data))").lineLimit(1)
            Text(item.categoryName).font(.caption).foregroundStyle(.secondary).lineLimit(1)
        }
    }
}

/// QR code for `url`, black on white.
public struct ShareQRCode: View {
    let url: URL

    @State private var image: CGImage?

    public init(url: URL) {
        self.url = url
    }

    public var body: some View {
        Group {
            if let image {
                Image(decorative: image, scale: 1)
                    .interpolation(.none)
                    .resizable()
                    .aspectRatio(1, contentMode: .fit)
                    .padding(8)
                    .background(.white)
                    .accessibilityLabel(jmLocalized("share.scanHint"))
            } else {
                Color.clear
            }
        }
        .task(id: url) { image = Self.makeImage(for: url) }
    }

    private static let context = CIContext()

    static func makeImage(for url: URL) -> CGImage? {
        let filter = CIFilter.qrCodeGenerator()
        filter.message = Data(url.absoluteString.utf8)
        filter.correctionLevel = "M"
        guard let output = filter.outputImage else { return nil }
        return context.createCGImage(output, from: output.extent)
    }
}
