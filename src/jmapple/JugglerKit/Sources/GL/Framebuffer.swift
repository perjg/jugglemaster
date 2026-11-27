#if canImport(UIKit)
import OpenGLES
#else
import OpenGL.GL
import OpenGL.GL3
#endif

/// Multisampled color + depth render target. The engine draws into it and
/// `resolve(into:)` downsamples into the framebuffer that is shown or read.
/// Must be created and used with `GLContext.shared` current.
@MainActor
public final class MultisampleTarget {
    public static let preferredSamples: GLsizei = 4

    public let width: GLsizei
    public let height: GLsizei
    public let samples: GLsizei

    private var framebuffer: GLuint = 0
    private var colorBuffer: GLuint = 0
    private var depthBuffer: GLuint = 0

    public init?(width: GLsizei, height: GLsizei, samples: GLsizei = MultisampleTarget.preferredSamples) {
        guard width > 0, height > 0 else { return nil }
        var maxSamples: GLint = 0
        glGetIntegerv(GLenum(GL_MAX_SAMPLES), &maxSamples)

        self.width = width
        self.height = height
        self.samples = min(samples, GLsizei(maxSamples))

        glGenFramebuffers(1, &framebuffer)
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), framebuffer)

        glGenRenderbuffers(1, &colorBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), colorBuffer)
        glRenderbufferStorageMultisample(GLenum(GL_RENDERBUFFER), self.samples, GLenum(GL_RGBA8), width, height)
        glFramebufferRenderbuffer(
            GLenum(GL_FRAMEBUFFER), GLenum(GL_COLOR_ATTACHMENT0), GLenum(GL_RENDERBUFFER), colorBuffer)

        glGenRenderbuffers(1, &depthBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), depthBuffer)
        glRenderbufferStorageMultisample(
            GLenum(GL_RENDERBUFFER), self.samples, GLenum(GL_DEPTH_COMPONENT24), width, height)
        glFramebufferRenderbuffer(
            GLenum(GL_FRAMEBUFFER), GLenum(GL_DEPTH_ATTACHMENT), GLenum(GL_RENDERBUFFER), depthBuffer)

        guard glCheckFramebufferStatus(GLenum(GL_FRAMEBUFFER)) == GLenum(GL_FRAMEBUFFER_COMPLETE) else {
            release()
            return nil
        }
    }

    public func bind() {
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), framebuffer)
    }

    /// Resolves into `target` (0 = the context's default framebuffer) and leaves it bound.
    public func resolve(into target: GLuint) {
        glBindFramebuffer(GLenum(GL_READ_FRAMEBUFFER), framebuffer)
        glBindFramebuffer(GLenum(GL_DRAW_FRAMEBUFFER), target)
        glBlitFramebuffer(
            0, 0, GLint(width), GLint(height), 0, 0, GLint(width), GLint(height),
            GLbitfield(GL_COLOR_BUFFER_BIT), GLenum(GL_NEAREST))
        #if canImport(UIKit)
        let discard = [GLenum(GL_COLOR_ATTACHMENT0), GLenum(GL_DEPTH_ATTACHMENT)]
        glInvalidateFramebuffer(GLenum(GL_READ_FRAMEBUFFER), GLsizei(discard.count), discard)
        #endif
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), target)
    }

    /// Deletes the GL objects; the instance is unusable afterwards.
    public func release() {
        glDeleteFramebuffers(1, &framebuffer)
        glDeleteRenderbuffers(1, &colorBuffer)
        glDeleteRenderbuffers(1, &depthBuffer)
        framebuffer = 0
        colorBuffer = 0
        depthBuffer = 0
    }
}

/// Single-sampled RGBA framebuffer that can be read back, for tests and snapshots.
@MainActor
public final class OffscreenFramebuffer {
    public let width: GLsizei
    public let height: GLsizei

    public private(set) var framebuffer: GLuint = 0
    private var colorBuffer: GLuint = 0

    public init?(width: GLsizei, height: GLsizei) {
        guard width > 0, height > 0 else { return nil }
        self.width = width
        self.height = height

        glGenFramebuffers(1, &framebuffer)
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), framebuffer)
        glGenRenderbuffers(1, &colorBuffer)
        glBindRenderbuffer(GLenum(GL_RENDERBUFFER), colorBuffer)
        glRenderbufferStorage(GLenum(GL_RENDERBUFFER), GLenum(GL_RGBA8), width, height)
        glFramebufferRenderbuffer(
            GLenum(GL_FRAMEBUFFER), GLenum(GL_COLOR_ATTACHMENT0), GLenum(GL_RENDERBUFFER), colorBuffer)

        guard glCheckFramebufferStatus(GLenum(GL_FRAMEBUFFER)) == GLenum(GL_FRAMEBUFFER_COMPLETE) else {
            release()
            return nil
        }
    }

    /// RGBA8 pixels, top row first.
    public func readPixels() -> [UInt8] {
        glBindFramebuffer(GLenum(GL_FRAMEBUFFER), framebuffer)
        let rowBytes = Int(width) * 4
        var bottomUp = [UInt8](repeating: 0, count: rowBytes * Int(height))
        glPixelStorei(GLenum(GL_PACK_ALIGNMENT), 1)
        glReadPixels(0, 0, width, height, GLenum(GL_RGBA), GLenum(GL_UNSIGNED_BYTE), &bottomUp)

        var topDown = [UInt8](repeating: 0, count: bottomUp.count)
        for row in 0..<Int(height) {
            let src = (Int(height) - 1 - row) * rowBytes
            topDown[(row * rowBytes)..<((row + 1) * rowBytes)] = bottomUp[src..<(src + rowBytes)]
        }
        return topDown
    }

    public func release() {
        glDeleteFramebuffers(1, &framebuffer)
        glDeleteRenderbuffers(1, &colorBuffer)
        framebuffer = 0
        colorBuffer = 0
    }
}
