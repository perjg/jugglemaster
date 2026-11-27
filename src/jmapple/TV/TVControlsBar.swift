import JugglerKit
import SwiftUI

/// Bottom bar shown on click: panels, camera presets (behind the Camera button; the remote's arrows also
/// step through them), zoom, auto-rotate and pause.
struct TVControlsBar: View {
    let model: JugglerModel
    let onSelectPanel: (TVPanel) -> Void
    let onInteraction: () -> Void

    @FocusState private var focused: String?
    @State private var showPresets = false

    var body: some View {
        VStack(spacing: 16) {
            if showPresets {
                presetsRow
            }
            mainRow
        }
        .font(.callout)
        .padding(.horizontal, 32)
        .padding(.vertical, 20)
        .background(.regularMaterial, in: RoundedRectangle(cornerRadius: 24))
        .focusSection()
        .defaultFocus($focused, "patterns")
        // Claims focus even if it is still held elsewhere (the remote input) when the bar appears
        .onAppear { focused = "patterns" }
        .onChange(of: focused) { onInteraction() }
    }

    /// Opened from the Camera button; picking a preset closes it again.
    private var presetsRow: some View {
        HStack(spacing: 18) {
            ForEach(model.cameraPresets, id: \.self) { preset in
                Button {
                    model.setCameraPreset(preset)
                    showPresets = false
                    focused = "camera"
                    onInteraction()
                } label: {
                    Text(preset.title).lineLimit(1).fixedSize()
                }
                .focused($focused, equals: "preset-\(preset.rawValue)")
                .accessibilityIdentifier("preset-\(preset.rawValue)")
            }
        }
        .focusSection()
    }

    private var mainRow: some View {
        HStack(spacing: 18) {
            iconButton("list.bullet", jmLocalized("menu.patterns"), id: "patterns") { onSelectPanel(.patterns) }
            iconButton("number", jmLocalized("menu.tabSite"), id: "siteswap") { onSelectPanel(.siteswap) }
            iconButton("gearshape", jmLocalized("menu.settings"), id: "settings") { onSelectPanel(.settings) }
            iconButton("qrcode", jmLocalized("share.share"), id: "share") { onSelectPanel(.share) }

            Spacer(minLength: 24)

            Button {
                showPresets.toggle()
                onInteraction()
            } label: {
                Label((model.cameraPreset ?? .front).title, systemImage: "video")
                    .lineLimit(1)
                    .fixedSize()
            }
            .accessibilityLabel("\(jmLocalized("menu.camera")), \((model.cameraPreset ?? .front).title)")
            .accessibilityIdentifier("camera")
            .focused($focused, equals: "camera")

            Spacer(minLength: 24)

            iconButton("minus.magnifyingglass", jmLocalized("zoom.zoomOut"), id: "zoomOut") { model.zoomOut() }
                .disabled(!model.canZoomOut)
            iconButton("plus.magnifyingglass", jmLocalized("zoom.zoomIn"), id: "zoomIn") { model.zoomIn() }
                .disabled(!model.canZoomIn)
            iconButton("arrow.counterclockwise", jmLocalized("zoom.zoomReset"), id: "zoomReset") { model.resetZoom() }
            iconButton(
                model.autoRotate ? "rotate.3d.fill" : "rotate.3d", jmLocalized("menu.autoRotate"),
                id: "autoRotateButton"
            ) { model.toggleAutoRotate() }
            iconButton(
                model.isPaused ? "play.fill" : "pause.fill",
                model.isPaused ? jmLocalized("control.play") : jmLocalized("control.pause"), id: "pause"
            ) { model.togglePause() }
        }
    }

    private func iconButton(_ symbol: String, _ label: String, id: String, action: @escaping () -> Void)
        -> some View
    {
        Button {
            action()
            onInteraction()
        } label: {
            Image(systemName: symbol)
                .frame(width: 44, height: 44)
        }
        .accessibilityLabel(label)
        .accessibilityIdentifier(id)
        .focused($focused, equals: id)
    }
}
