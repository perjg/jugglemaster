import JugglerKit
import SwiftUI

/// iPhone (and narrow iPad windows): full-screen juggler with a floating menu button. The menu is a
/// sheet in portrait, keeping the juggler visible and interactive above its medium detent, and a side
/// panel in landscape.
struct CompactRootView: View {
    let model: JugglerModel

    static let mediumDetent = PresentationDetent.fraction(0.45)

    @Environment(\.verticalSizeClass) private var verticalSizeClass
    @State private var isMenuOpen = false
    @State private var detent = CompactRootView.mediumDetent

    private var isLandscape: Bool { verticalSizeClass == .compact }

    var body: some View {
        GeometryReader { geometry in
            HStack(spacing: 0) {
                if isLandscape && isMenuOpen {
                    NavigationStack {
                        MenuView(model: model)
                            .navigationTitle(jmLocalized("title"))
                            .toolbar {
                                Button(jmLocalized("menu.close"), systemImage: "xmark") { isMenuOpen = false }
                            }
                    }
                    .frame(width: min(360, geometry.size.width * 0.45))
                    .transition(.move(edge: .leading))
                }
                JugglerStage(model: model, compact: true)
                    .frame(height: stageHeight(in: geometry), alignment: .top)
                    .frame(maxHeight: .infinity, alignment: .top)
                    .overlay(alignment: .bottomLeading) {
                        if !(isLandscape && isMenuOpen) {
                            OverlayIconButton("line.3.horizontal", label: jmLocalized("menu.menu")) {
                                isMenuOpen = true
                            }
                            .padding(10)
                            .accessibilityIdentifier("menuButton")
                        }
                    }
            }
        }
        .background(.black)
        .animation(.easeInOut(duration: 0.2), value: isMenuOpen)
        // Rotating to landscape dismisses the sheet; the menu stays open as the side panel
        .sheet(isPresented: Binding(get: { isMenuOpen && !isLandscape }, set: { if !isLandscape { isMenuOpen = $0 } }))
        {
            MenuView(model: model) {
                if detent == .large { isMenuOpen = false }
            }
            .presentationDetents([Self.mediumDetent, .large], selection: $detent)
            .presentationBackgroundInteraction(.enabled(upThrough: Self.mediumDetent))
            .presentationDragIndicator(.visible)
        }
        .sensoryFeedback(.selection, trigger: model.objectType)
        .sensoryFeedback(.selection, trigger: model.cameraPreset) { _, preset in preset != nil }
        .sensoryFeedback(.selection, trigger: model.renderMode)
    }

    /// Above the sheet at its medium detent, so the juggler stays centered in what is visible.
    private func stageHeight(in geometry: GeometryProxy) -> CGFloat {
        guard isMenuOpen, !isLandscape, detent == Self.mediumDetent else { return geometry.size.height }
        return (geometry.size.height + geometry.safeAreaInsets.bottom) * 0.55
    }
}
