import JugglerKit
import SwiftUI

/// What is shown over the juggler. `.watching` shows nothing and gives the remote to `RemoteInput`.
enum TVPanel: Equatable {
    case watching
    case controls
    case patterns
    case siteswap
    case search
    case settings
    case share

    /// Panels on the leading side; the juggler renders in the space to their right.
    var isSidePanel: Bool {
        switch self {
        case .patterns, .siteswap, .settings, .share: true
        case .watching, .controls, .search: false
        }
    }
}

struct TVRootView: View {
    @Bindable var model: JugglerModel
    @State private var panel = TVPanel.watching
    @State private var lastInteraction = Date.now
    @State private var browserFilter = SourceFilter.all

    static let panelWidth: CGFloat = 760
    static let panelGap: CGFloat = 40

    /// JM_CONTROLS_TIMEOUT (seconds) shortens it for UI tests.
    static let controlsTimeout: Duration = .seconds(
        Double(ProcessInfo.processInfo.environment["JM_CONTROLS_TIMEOUT"] ?? "") ?? 8)

    var body: some View {
        GeometryReader { geometry in
            let reserved =
                panel.isSidePanel ? geometry.safeAreaInsets.leading + Self.panelWidth + Self.panelGap : 0
            ZStack {
                stage(reserved: reserved, safeArea: geometry.safeAreaInsets)
                remoteInput
                overlays
            }
        }
        .onPlayPauseCommand { model.togglePause() }
        .onExitCommand(perform: exitAction)
        .task(id: lastInteraction) {
            guard panel == .controls else { return }
            try? await Task.sleep(for: Self.controlsTimeout)
            if !Task.isCancelled, panel == .controls {
                panel = .watching
            }
        }
    }

    /// The juggler beside any side panel, so a preview is never hidden behind it. The resize is not
    /// animated: every intermediate size would rebuild the render buffers.
    private func stage(reserved: CGFloat, safeArea: EdgeInsets) -> some View {
        HStack(spacing: 0) {
            Color.black.frame(width: reserved)
            JugglerView(model: model)
                .overlay(alignment: .top) {
                    if model.showSiteswap {
                        SiteswapOverlay(rows: model.siteswap, fontSize: 38)
                            .padding(.top, safeArea.top)
                            .accessibilityIdentifier("siteswapOverlay")
                    }
                }
                .overlay(alignment: .topTrailing) {
                    if model.isPaused {
                        Image(systemName: "pause.fill")
                            .font(.system(size: 36))
                            .foregroundStyle(.white)
                            .padding(16)
                            .background(overlayBackground, in: RoundedRectangle(cornerRadius: 8))
                            .padding(.top, safeArea.top)
                            .padding(.trailing, safeArea.trailing)
                            .accessibilityIdentifier("pausedIndicator")
                    }
                }
        }
        .ignoresSafeArea()
        .transaction { $0.animation = nil }
    }

    private var remoteInput: some View {
        RemoteInput(
            isActive: panel == .watching,
            onSelect: { show(.controls) },
            onLongSelect: { model.toggleAutoRotate() },
            onPlayPause: { model.togglePause() },
            onArrow: { handleArrow($0) },
            onOrbit: { dx, dy in model.orbit(dx: dx, dy: dy) }
        )
        .ignoresSafeArea()
        .accessibilityIdentifier("remote")
    }

    /// Nil while watching, so Back reaches the system and returns to the Home screen.
    private var exitAction: (() -> Void)? {
        panel == .watching ? nil : { goBack() }
    }

    private var overlays: some View {
        VStack(spacing: 0) {
            HStack(alignment: .top, spacing: 0) {
                switch panel {
                case .settings:
                    TVPanelContainer(title: jmLocalized("menu.settings")) {
                        TVSettingsPanel(model: model, onInteraction: touch)
                    }
                case .share:
                    TVPanelContainer(title: jmLocalized("share.share")) {
                        TVSharePanel(url: model.shareURL)
                    }
                case .patterns:
                    TVPanelContainer(title: jmLocalized("menu.patterns")) {
                        TVPatternBrowser(
                            model: model, filter: $browserFilter, onSearch: { show(.search) },
                            onCommit: { panel = .watching })
                    }
                case .siteswap:
                    TVPanelContainer(title: jmLocalized("menu.tabSite")) {
                        TVSiteswapPanel(model: model) { panel = .watching }
                    }
                case .search:
                    TVPatternSearch(model: model, filter: browserFilter) { panel = .watching }
                        .background(.regularMaterial, in: RoundedRectangle(cornerRadius: 24))
                case .watching, .controls:
                    EmptyView()
                }
                Spacer(minLength: 0)
            }
            .frame(maxHeight: .infinity, alignment: .top)
            .padding(.top, 24)

            if panel == .controls {
                TVControlsBar(model: model, onSelectPanel: show, onInteraction: touch)
                    .transition(.move(edge: .bottom).combined(with: .opacity))
            }
        }
        .animation(.easeInOut(duration: 0.2), value: panel)
    }

    private func show(_ next: TVPanel) {
        panel = next
        touch()
    }

    private func touch() {
        lastInteraction = .now
    }

    private func goBack() {
        switch panel {
        case .patterns:
            model.cancelPreview()
            show(.controls)
        case .search: show(.patterns)
        case .siteswap, .settings, .share: show(.controls)
        case .controls, .watching: panel = .watching
        }
    }

    private func handleArrow(_ type: UIPress.PressType) {
        let presets = model.cameraPresets
        let index = presets.firstIndex(of: model.cameraPreset ?? .front) ?? 0
        switch type {
        case .leftArrow:
            model.setCameraPreset(presets[max(index - 1, 0)])
        case .rightArrow:
            model.setCameraPreset(presets[min(index + 1, presets.count - 1)])
        case .upArrow:
            model.zoomIn()
        case .downArrow:
            model.zoomOut()
        default:
            break
        }
    }
}

/// Side panel over the juggler, which stays visible on the right.
struct TVPanelContainer<Content: View>: View {
    let title: String
    @ViewBuilder var content: Content

    var body: some View {
        VStack(alignment: .leading, spacing: 24) {
            Text(title).font(.title2.bold())
            content
        }
        .padding(40)
        .frame(width: TVRootView.panelWidth, alignment: .topLeading)
        .frame(maxHeight: .infinity, alignment: .top)
        .background(.regularMaterial, in: RoundedRectangle(cornerRadius: 24))
    }
}
