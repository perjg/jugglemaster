import SwiftUI

/// Pointer and touch input on the juggler, in the web's terms: drag deltas are normalized to the view size.
public struct PointerHandlers {
    public var drag: (Float, Float) -> Void
    public var doubleTap: () -> Void
    public var zoom: (Float) -> Void
}

/// The juggler, rendered by `GLRenderView` and kept in sync with `model`. With `interactive`, dragging
/// orbits, a double tap/click toggles auto-rotation and pinching (or the scroll wheel on macOS) zooms.
public struct JugglerView: View {
    private let model: JugglerModel
    private let interactive: Bool

    public init(model: JugglerModel, interactive: Bool = false) {
        self.model = model
        self.interactive = interactive
    }

    public var body: some View {
        #if os(iOS)
        if interactive {
            GeometryReader { geometry in
                RenderView(model: model, pointer: nil)
                    .gesture(dragGesture(size: geometry.size).simultaneously(with: pinchGesture))
                    .onTapGesture(count: 2) { model.toggleAutoRotate() }
            }
        } else {
            RenderView(model: model, pointer: nil)
        }
        #else
        RenderView(model: model, pointer: interactive ? handlers : nil)
        #endif
    }

    private var handlers: PointerHandlers {
        PointerHandlers(
            drag: { [model] dx, dy in model.orbit(dx: dx, dy: dy) },
            doubleTap: { [model] in model.toggleAutoRotate() },
            zoom: { [model] delta in model.zoom(by: delta) })
    }

    #if os(iOS)
    @State private var lastDrag = CGSize.zero
    @State private var lastScale: CGFloat = 1

    private func dragGesture(size: CGSize) -> some Gesture {
        DragGesture(minimumDistance: 1)
            .onChanged { value in
                guard size.width > 0, size.height > 0 else { return }
                let dx = (value.translation.width - lastDrag.width) / size.width
                let dy = (value.translation.height - lastDrag.height) / size.height
                lastDrag = value.translation
                model.orbit(dx: Float(dx), dy: Float(dy))
            }
            .onEnded { _ in lastDrag = .zero }
    }

    private var pinchGesture: some Gesture {
        MagnifyGesture()
            .onChanged { value in
                model.zoom(by: Float(value.magnification - lastScale) * 0.5)
                lastScale = value.magnification
            }
            .onEnded { _ in lastScale = 1 }
    }
    #endif
}

@MainActor
private func configure(_ view: GLRenderView, model: JugglerModel) {
    view.onStart = { [model] in model.engineDidStart() }
    view.onFrame = { [model] in model.frameDidRender() }
}

#if canImport(UIKit)
private struct RenderView: UIViewRepresentable {
    let model: JugglerModel
    let pointer: PointerHandlers?

    func makeUIView(context: Context) -> GLRenderView {
        let view = GLRenderView(frame: .zero)
        configure(view, model: model)
        return view
    }

    func updateUIView(_ view: GLRenderView, context: Context) {
        configure(view, model: model)
    }
}
#else
private struct RenderView: NSViewRepresentable {
    let model: JugglerModel
    let pointer: PointerHandlers?

    func makeNSView(context: Context) -> GLRenderView {
        let view = GLRenderView(frame: .zero)
        updateNSView(view, context: context)
        return view
    }

    func updateNSView(_ view: GLRenderView, context: Context) {
        configure(view, model: model)
        view.pointer = pointer
    }
}
#endif
