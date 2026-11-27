import JugglerKit
import SwiftUI

@main
struct JuggleMasterTVApp: App {
    @State private var model = JugglerModel()

    var body: some Scene {
        WindowGroup {
            TVRootView(model: model)
                .task { await model.loadLibrary() }
        }
    }
}
