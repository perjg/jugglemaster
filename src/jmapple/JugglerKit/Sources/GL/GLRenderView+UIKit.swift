#if canImport(UIKit)
import OpenGLES
import QuartzCore
import UIKit

/// Hosts the engine in a CAEAGLLayer and drives it from the display link.
/// Only one render view should be on screen at a time: the engine is a singleton.
public final class GLRenderView: UIView {
    override public static var layerClass: AnyClass { CAEAGLLayer.self }

    /// Called before the first frame of a new drawable size, once the engine is running.
    public var onStart: (() -> Void)?
    /// Called after every rendered frame, e.g. to poll engine state.
    public var onFrame: (() -> Void)?

    private var target: MultisampleTarget?
    private var displayFramebuffer: GLuint = 0
    private var displayColorBuffer: GLuint = 0
    private var pixelSize: (width: GLsizei, height: GLsizei) = (0, 0)
    private var displayLink: CADisplayLink?
    private var isActive = true

    override public init(frame: CGRect) {
        super.init(frame: frame)
        isOpaque = true
        backgroundColor = .black
        if let layer = layer as? CAEAGLLayer {
            layer.isOpaque = true
            layer.drawableProperties = [
                kEAGLDrawablePropertyRetainedBacking: false,
                kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8,
            ]
        }

        let center = NotificationCenter.default
        center.addObserver(
            self, selector: #selector(willResignActive), name: UIApplication.willResignActiveNotification, object: nil)
        center.addObserver(
            self, selector: #selector(didBecomeActive), name: UIApplication.didBecomeActiveNotification, object: nil)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) is not supported")
    }

    isolated deinit {
        displayLink?.invalidate()
        releaseBuffers()
    }

    override public func didMoveToWindow() {
        super.didMoveToWindow()
        if let screen = window?.screen {
            contentScaleFactor = screen.scale
        }
        updateDisplayLink()
    }

    override public func layoutSubviews() {
        super.layoutSubviews()
        // Resizes while inactive wait for didBecomeActive: no GL work in the background
        guard isActive else { return }
        let width = GLsizei((bounds.width * contentScaleFactor).rounded())
        let height = GLsizei((bounds.height * contentScaleFactor).rounded())
        guard width > 0, height > 0, (width, height) != pixelSize else { return }
        rebuildBuffers(width: width, height: height)
        renderFrame(advance: false)
    }

    @objc private func willResignActive() {
        isActive = false
        updateDisplayLink()
        GLContext.shared.makeCurrent()
        // iOS terminates apps that issue GL commands in the background
        glFinish()
    }

    @objc private func didBecomeActive() {
        isActive = true
        updateDisplayLink()
        setNeedsLayout()
    }

    private func updateDisplayLink() {
        let shouldRun = window != nil && isActive
        if shouldRun, displayLink == nil {
            let link = CADisplayLink(target: self, selector: #selector(tick))
            link.preferredFrameRateRange = CAFrameRateRange(minimum: 30, maximum: 120, preferred: 120)
            link.add(to: .main, forMode: .common)
            displayLink = link
        } else if !shouldRun, let link = displayLink {
            link.invalidate()
            displayLink = nil
        }
    }

    @objc private func tick() {
        renderFrame(advance: true)
    }

    private func rebuildBuffers(width: GLsizei, height: GLsizei) {
        let context = GLContext.shared
        context.makeCurrent()
        releaseBuffers()

        glGenFramebuffers(1, &displayFramebuffer)
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), displayFramebuffer)
        glGenRenderbuffers(1, &displayColorBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), displayColorBuffer)
        guard let drawable = layer as? CAEAGLLayer,
            context.context.renderbufferStorage(Int(GL_RENDERBUFFER), from: drawable)
        else { return }
        glFramebufferRenderbuffer(
            GLenum(GL_FRAMEBUFFER), GLenum(GL_COLOR_ATTACHMENT0), GLenum(GL_RENDERBUFFER), displayColorBuffer)

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

    private func releaseBuffers() {
        GLContext.shared.makeCurrent()
        target?.release()
        target = nil
        glDeleteFramebuffers(1, &displayFramebuffer)
        glDeleteRenderbuffers(1, &displayColorBuffer)
        displayFramebuffer = 0
        displayColorBuffer = 0
        pixelSize = (0, 0)
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
        target.resolve(into: displayFramebuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), displayColorBuffer)
        GLContext.shared.context.presentRenderbuffer(Int(GL_RENDERBUFFER))
        onFrame?()
    }
}
#endif
