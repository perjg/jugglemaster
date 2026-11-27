import JugglerKit
import SwiftUI

/// Mode, engine, style, auto-rotate, siteswap display, speed and objects.
struct TVSettingsPanel: View {
    @Bindable var model: JugglerModel
    let onInteraction: () -> Void

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 16) {
                valueRow(jmLocalized("menu.mode"), value: model.renderMode == .threeD ? "3D" : "2D", id: "mode") {
                    model.setRenderMode(model.renderMode == .threeD ? .twoD : .threeD)
                }

                valueRow(
                    jmLocalized("menu.engine"), value: model.preferredEngine.title, warning: model.engineMismatch,
                    id: "engine"
                ) {
                    model.setPreferredEngine(model.preferredEngine == .juggleSaver ? .juggleMaster : .juggleSaver)
                }

                Menu {
                    Picker(jmLocalized("menu.style"), selection: binding(model.selectedStyle) { model.setStyle($0) }) {
                        ForEach(JugglerModel.styles, id: \.self) { Text($0).tag($0) }
                    }
                } label: {
                    settingLabel(jmLocalized("menu.style"), value: model.selectedStyle)
                }
                .disabled(!model.isStyleEnabled)
                .accessibilityIdentifier("style")

                Toggle(jmLocalized("menu.autoRotate"), isOn: binding(model.autoRotate) { model.setAutoRotate($0) })
                    .accessibilityIdentifier("autoRotate")

                Toggle(jmLocalized("menu.showSiteswap"), isOn: $model.showSiteswap)
                    .accessibilityIdentifier("showSiteswap")

                HStack(spacing: 16) {
                    Text(jmLocalized("menu.speed"))
                    Spacer()
                    Button {
                        model.setSpeed(model.speed - JugglerModel.speedStep)
                    } label: {
                        Image(systemName: "minus")
                    }
                    .accessibilityLabel("\(jmLocalized("menu.speed")) −")
                    .accessibilityIdentifier("speedDown")
                    Text(model.speed, format: .number.precision(.fractionLength(1)))
                        .monospacedDigit()
                        .frame(minWidth: 70)
                        .accessibilityIdentifier("speedValue")
                    Button {
                        model.setSpeed(model.speed + JugglerModel.speedStep)
                    } label: {
                        Image(systemName: "plus")
                    }
                    .accessibilityLabel("\(jmLocalized("menu.speed")) +")
                    .accessibilityIdentifier("speedUp")
                }

                Text(jmLocalized("menu.objects"))
                ObjectTypePicker(model: model, iconSize: 48)
            }
            .padding(.vertical, 8)
        }
        .focusSection()
    }

    private func binding<Value>(_ value: Value, set: @escaping (Value) -> Void) -> Binding<Value> {
        Binding(
            get: { value },
            set: {
                set($0)
                onInteraction()
            })
    }

    private func settingLabel(_ title: String, value: String) -> some View {
        HStack {
            Text(title)
            Spacer()
            Text(value).foregroundStyle(.secondary)
        }
    }

    /// A row that shows its current value and switches to the next one on click.
    private func valueRow(
        _ title: String, value: String, warning: Bool = false, id: String, action: @escaping () -> Void
    ) -> some View {
        Button {
            action()
            onInteraction()
        } label: {
            HStack {
                settingLabel(title, value: value)
                if warning {
                    Image(systemName: "exclamationmark.triangle.fill")
                        .foregroundStyle(.yellow)
                        .accessibilityLabel(model.actualEngine.title)
                }
            }
        }
        .accessibilityIdentifier(id)
    }
}

/// QR code and link to the web app with the current settings.
struct TVSharePanel: View {
    let url: URL

    var body: some View {
        VStack(alignment: .leading, spacing: 24) {
            ShareQRCode(url: url)
                .frame(width: 400, height: 400)
                .accessibilityIdentifier("shareQR")
            Text(jmLocalized("share.scanHint"))
            Text(url.absoluteString)
                .font(.caption.monospaced())
                .foregroundStyle(.secondary)
                .lineLimit(4)
        }
    }
}
