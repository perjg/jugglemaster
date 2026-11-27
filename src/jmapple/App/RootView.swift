import JugglerKit
import SwiftUI

/// Sidebar + juggler on iPad and Mac; the compact layout on iPhone (also large iPhones in landscape, which
/// report a regular width) and narrow iPad windows.
struct RootView: View {
    let model: JugglerModel

    #if os(iOS)
    @Environment(\.horizontalSizeClass) private var horizontalSizeClass
    @Environment(\.verticalSizeClass) private var verticalSizeClass
    #endif
    @State private var columns = NavigationSplitViewVisibility.all

    var body: some View {
        #if os(iOS)
        if horizontalSizeClass == .compact || verticalSizeClass == .compact {
            CompactRootView(model: model)
        } else {
            splitView
        }
        #else
        splitView
        #endif
    }

    private var splitView: some View {
        NavigationSplitView(columnVisibility: $columns) {
            MenuView(model: model)
                .navigationTitle(jmLocalized("title"))
                .navigationSplitViewColumnWidth(min: 280, ideal: 320, max: 400)
        } detail: {
            JugglerStage(model: model)
                .background(.black)
                #if os(iOS)
            .toolbar(.hidden, for: .navigationBar)
                #endif
        }
        .navigationSplitViewStyle(.balanced)
    }
}
