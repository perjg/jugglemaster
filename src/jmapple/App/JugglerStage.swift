import JugglerKit
import SwiftUI

/// The interactive juggler with the web's overlays: siteswap on top, camera presets at the bottom and
/// zoom + share in the top-right corner (pinch-only zoom with a reset button when `compact`).
struct JugglerStage: View {
    let model: JugglerModel
    var compact = false

    var body: some View {
        // Controls stay inside the safe area; the juggler extends under the bars (vertical only on iPad
        // and Mac, where the floating sidebar is a leading inset)
        Color.clear
            .background {
                JugglerView(model: model, interactive: true)
                    .ignoresSafeArea(edges: compact ? .all : .vertical)
            }
            .overlay(alignment: .top) {
                if model.showSiteswap {
                    SiteswapOverlay(display: model.siteswap)
                        .padding(.top, 12)
                        .allowsHitTesting(false)
                        .accessibilityIdentifier("siteswapOverlay")
                }
            }
            .overlay(alignment: .bottom) {
                CameraPresetsBar(model: model)
                    .padding(.bottom, 10)
            }
            .overlay(alignment: compact ? .bottomTrailing : .topTrailing) {
                VStack(alignment: .trailing, spacing: 8) {
                    if compact {
                        OverlayIconButton("arrow.counterclockwise", label: jmLocalized("zoom.zoomReset")) {
                            model.resetZoom()
                        }
                    } else {
                        ZoomControls(model: model)
                    }
                    ShareButton(model: model)
                }
                .padding(10)
                .padding(.bottom, compact ? 44 : 0)
            }
            .overlay(alignment: .topLeading) {
                if model.isPaused {
                    Image(systemName: "pause.fill")
                        .foregroundStyle(.white)
                        .padding(8)
                        .background(overlayBackground, in: RoundedRectangle(cornerRadius: 4))
                        .padding(10)
                        .padding(.leading, compact ? 0 : 44)
                }
            }
            .focusable()
            .focusEffectDisabled()
            .onKeyPress(.space) {
                model.togglePause()
                return .handled
            }
    }
}

/// A single icon button styled like the web overlay controls.
struct OverlayIconButton: View {
    let symbol: String
    let label: String
    let action: () -> Void

    init(_ symbol: String, label: String, action: @escaping () -> Void) {
        self.symbol = symbol
        self.label = label
        self.action = action
    }

    var body: some View {
        Button(action: action) {
            Image(systemName: symbol)
                .font(.system(size: 16))
                .frame(width: 26, height: 26)
                .padding(3)
                .background(overlayBackground, in: RoundedRectangle(cornerRadius: 4))
                .foregroundStyle(.white)
        }
        .buttonStyle(.plain)
        .accessibilityLabel(label)
        .help(label)
        #if os(iOS)
        .hoverEffect()
        #endif
    }
}

/// Share link to the web app, QR code and copy button (web: ShareButton.tsx).
struct ShareButton: View {
    let model: JugglerModel

    @State private var isPresented = false
    @State private var copied = false

    var body: some View {
        OverlayIconButton("square.and.arrow.up", label: jmLocalized("share.share")) {
            copied = false
            isPresented = true
        }
        .accessibilityIdentifier("share")
        .popover(isPresented: $isPresented) {
            let url = model.shareURL
            VStack(spacing: 12) {
                ShareQRCode(url: url)
                    .frame(width: 176, height: 176)
                    .accessibilityIdentifier("shareQR")
                Text(url.absoluteString)
                    .font(.caption2.monospaced())
                    .foregroundStyle(.secondary)
                    .textSelection(.enabled)
                    .frame(maxWidth: 240)
                HStack {
                    Button {
                        copy(url)
                        copied = true
                    } label: {
                        Label(
                            copied ? jmLocalized("share.copied") : jmLocalized("share.copyLink"),
                            systemImage: copied ? "checkmark" : "doc.on.doc")
                    }
                    ShareLink(item: url) {
                        Label(jmLocalized("share.share"), systemImage: "square.and.arrow.up")
                    }
                }
                .buttonStyle(.bordered)
            }
            .padding()
            .presentationCompactAdaptation(.popover)
        }
    }

    private func copy(_ url: URL) {
        #if os(iOS)
        UIPasteboard.general.url = url
        #else
        NSPasteboard.general.clearContents()
        NSPasteboard.general.setString(url.absoluteString, forType: .string)
        #endif
    }
}
