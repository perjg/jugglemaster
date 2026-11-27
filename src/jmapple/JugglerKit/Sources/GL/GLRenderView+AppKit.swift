#if os(macOS)
import AppKit
import OpenGL.GL
import OpenGL.GL3

/// Hosts the engine in an NSOpenGLView sharing `GLContext.shared` and drives it from the display link.
/// Only one render view should be on screen at a time: the engine is a singleton.
public final class GLRenderView: NSOpenGLView {
    /// Called before the first frame of a new drawable size, once the engine is running.
    public var onStart: (() -> Void)?
    /// Called after every rendered frame, e.g. to poll engine state.
    public var onFrame: (() -> Void)?
    /// Pointer input; nil handlers leave the events to the responder chain.
    public var pointer: PointerHandlers?

    private var target: MultisampleTarget?
    private var pixelSize: (width: GLsizei, height: GLsizei) = (0, 0)
    private var displayLink: CADisplayLink?
    private var advanceOnDraw = false

    override public init(frame frameRect: NSRect) {
        super.init(frame: frameRect, pixelFormat: GLContext.shared.pixelFormat)!
        openGLContext = GLContext.shared.context
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) is not supported")
    }

    isolated deinit {
        displayLink?.invalidate()
        target?.release()
    }

    override public var isOpaque: Bool { true }

    /// The context is shared with any later render view, so a view going away must not detach it.
    override public func clearGLContext() {}

    override public func viewDidMoveToWindow() {
        super.viewDidMoveToWindow()
        displayLink?.invalidate()
        displayLink = nil
        guard window != nil else { return }

        let link = displayLink(target: self, selector: #selector(tick))
        link.add(to: .main, forMode: .common)
        displayLink = link
    }

    override public func reshape() {
        super.reshape()
        updateTarget()
    }

    override public func draw(_ dirtyRect: NSRect) {
        updateTarget()
        renderFrame(advance: advanceOnDraw)
        advanceOnDraw = false
    }

    override public func mouseDown(with event: NSEvent) {
        guard let pointer else { return super.mouseDown(with: event) }
        if event.clickCount == 2 {
            pointer.doubleTap()
        }
    }

    override public func mouseDragged(with event: NSEvent) {
        guard let pointer, bounds.width > 0, bounds.height > 0 else { return super.mouseDragged(with: event) }
        pointer.drag(Float(event.deltaX / bounds.width), Float(event.deltaY / bounds.height))
    }

    override public func scrollWheel(with event: NSEvent) {
        guard let pointer else { return super.scrollWheel(with: event) }
        pointer.zoom(Float(event.scrollingDeltaY) * (event.hasPreciseScrollingDeltas ? 0.002 : 0.02))
    }

    override public func magnify(with event: NSEvent) {
        guard let pointer else { return super.magnify(with: event) }
        pointer.zoom(Float(event.magnification) * 0.5)
    }

    @objc private func tick() {
        advanceOnDraw = true
        needsDisplay = true
    }

    private func updateTarget() {
        let backing = convertToBacking(bounds)
        let width = GLsizei(backing.width.rounded())
        let height = GLsizei(backing.height.rounded())
        guard width > 0, height > 0, (width, height) != pixelSize else { return }

        GLContext.shared.makeCurrent()
        target?.release()
        target = MultisampleTarget(width: width, height: height)
        pixelSize = (width, height)

        let engine = Engine.shared
        if engine.isStarted {
            engine.resize(width: Int(width), height: Int(height))
        } else {
            target?.bind()
            engine.start(width: Int(width), height: Int(height))
        }
        if engine.isStarted {
            onStart?()
        }
    }

    private func renderFrame(advance: Bool) {
        guard let target, Engine.shared.isStarted else { return }
        GLContext.shared.makeCurrent()
        target.bind()
        if advance {
            Engine.shared.frame()
        } else {
            Engine.shared.draw()
        }
        target.resolve(into: 0)
        GLContext.shared.context.flushBuffer()
        onFrame?()
    }
}
#endif
