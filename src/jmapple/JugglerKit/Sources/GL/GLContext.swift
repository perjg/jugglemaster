#if canImport(UIKit)
import OpenGLES
#else
import AppKit
import OpenGL.GL
#endif

/// The process-wide GL context. The engine caches GL objects for the lifetime
/// of the process, so every view and test renders through this one context.
@MainActor
public final class GLContext {
    public static let shared = GLContext()

    #if canImport(UIKit)
    public let context: EAGLContext

    private init() {
        guard let context = EAGLContext(api: .openGLES3) else {
            fatalError("OpenGL ES 3.0 is not available")
        }
        self.context = context
    }

    public func makeCurrent() {
        if EAGLContext.current() !== context {
            EAGLContext.setCurrent(context)
        }
    }
    #else
    public let pixelFormat: NSOpenGLPixelFormat
    public let context: NSOpenGLContext

    private init() {
        // Legacy profile: the desktop shader path uses GLSL 1.20
        let attributes: [NSOpenGLPixelFormatAttribute] = [
            UInt32(NSOpenGLPFAOpenGLProfile), UInt32(NSOpenGLProfileVersionLegacy),
            UInt32(NSOpenGLPFAAccelerated),
            UInt32(NSOpenGLPFADoubleBuffer),
            UInt32(NSOpenGLPFAColorSize), 24,
            UInt32(NSOpenGLPFAAlphaSize), 8,
            UInt32(NSOpenGLPFADepthSize), 24,
            0,
        ]
        guard let pixelFormat = NSOpenGLPixelFormat(attributes: attributes),
            let context = NSOpenGLContext(format: pixelFormat, share: nil)
        else {
            fatalError("OpenGL is not available")
        }
        self.pixelFormat = pixelFormat
        self.context = context
    }

    public func makeCurrent() {
        if NSOpenGLContext.current !== context {
            context.makeCurrentContext()
        }
    }
    #endif
}
