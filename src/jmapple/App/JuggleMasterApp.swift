import JugglerKit
import SwiftUI

@main
struct JuggleMasterApp: App {
    @State private var model = JugglerModel()

    var body: some Scene {
        // One window: the engine and its GL state are process-wide
        #if os(macOS)
        Window(jmLocalized("title"), id: "main") {
            content
                .frame(minWidth: 820, minHeight: 520)
        }
        .windowResizability(.contentMinSize)
        .commands { JugglerCommands(model: model) }
        #else
        WindowGroup {
            content
        }
        .commands { JugglerCommands(model: model) }
        #endif
    }

    private var content: some View {
        RootView(model: model)
            .task { await model.loadLibrary() }
            .onOpenURL { url in model.load(AppState(url: url)) }
    }
}

/// Menu bar (Mac) and hardware keyboard (iPad) commands.
struct JugglerCommands: Commands {
    let model: JugglerModel

    var body: some Commands {
        CommandMenu(jmLocalized("commands.juggler")) {
            Button(model.isPaused ? jmLocalized("control.play") : jmLocalized("control.pause")) {
                model.togglePause()
            }
            .keyboardShortcut("p", modifiers: [.command, .option])
            Button(jmLocalized("menu.autoRotate")) { model.toggleAutoRotate() }
                .keyboardShortcut("r", modifiers: [.command, .option])
            Divider()
            Button("2D") { model.setRenderMode(.twoD) }
                .keyboardShortcut("2", modifiers: .command)
            Button("3D") { model.setRenderMode(.threeD) }
                .keyboardShortcut("3", modifiers: .command)
            Divider()
            ForEach(Array(CameraPreset.displayOrder.enumerated()), id: \.element) { index, preset in
                Button(preset.title) { model.setCameraPreset(preset) }
                    .keyboardShortcut(KeyEquivalent(Character("\(index + 1)")), modifiers: [.command, .option])
            }
            Divider()
            Button(jmLocalized("zoom.zoomIn")) { model.zoomIn() }
                .keyboardShortcut("+", modifiers: .command)
            Button(jmLocalized("zoom.zoomOut")) { model.zoomOut() }
                .keyboardShortcut("-", modifiers: .command)
            Button(jmLocalized("zoom.zoomReset")) { model.resetZoom() }
                .keyboardShortcut("0", modifiers: .command)
        }
    }
}
