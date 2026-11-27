import CoreGraphics
import Foundation
import ImageIO
import Testing

@testable import JugglerKit

#if canImport(UIKit)
import OpenGLES
#else
import OpenGL.GL
#endif

private final class BundleToken {}

/// Pixel comparison against the Playwright baselines in tests/visual (same harness sequence:
/// 800×600, clock pinned to 60 fps, 120 stepped frames).
extension EngineSuites {
    @MainActor
    @Suite struct RenderParity {
        struct Case: CustomTestStringConvertible, Sendable {
            let snapshot: String
            let site: String
            let style: String?
            let mode: RenderMode
            let engine: EngineType

            var testDescription: String { snapshot }
        }

        nonisolated static let patterns: [(name: String, site: String, style: String?, engine: EngineType)] = [
            ("jm-pattern-3", "3", nil, .juggleMaster),
            ("jm-pattern-531", "531", nil, .juggleMaster),
            ("jm-pattern-97531", "97531", nil, .juggleMaster),
            ("js-pattern-3b", "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)", nil, .juggleSaver),
            ("js-pattern-7c", "7c@(-2,0,-20)>(1.2,0,-5)7c@(2,0,20)>(-1.2,0,5)", nil, .juggleSaver),
            ("js-pattern-3r", "3r@(-2.5,3,-90,80)>(2,1,90,30) 3r@(2.5,3,90,80)>(-2,1,-90,30)", nil, .juggleSaver),
            ("js-pattern-3b-style", "3b", "JuggleSaver", .juggleSaver),
        ]

        /// Every pattern in 3D and 2D.
        nonisolated static let cases: [Case] = patterns.flatMap { pattern in
            [(RenderMode.threeD, "3d"), (RenderMode.twoD, "2d")].map { mode, suffix in
                Case(
                    snapshot: "\(pattern.name)-\(suffix)", site: pattern.site, style: pattern.style, mode: mode,
                    engine: pattern.engine)
            }
        }

        /// Channel difference treated as a mismatch. Rasterizers differ, so edges never match exactly.
        static let channelTolerance = 48
        /// Fraction of mismatching pixels allowed (antialiased edges, 1 px line placement).
        static let maxMismatchRatio = 0.01
        /// Colored (object) pixels may differ by this fraction, so a missing object fails within the mismatch budget.
        static let maxObjectPixelDifference = 0.1

        @Test(arguments: cases)
        func matchesWebBaseline(_ testCase: Case) throws {
            let target = try #require(EngineSuites.prepare())
            let output = try #require(OffscreenFramebuffer(width: target.width, height: target.height))
            defer { output.release() }

            let engine = Engine.shared
            engine.setRenderMode(.twoD)
            engine.render()
            engine.isPaused = true
            func selectPattern() {
                engine.setPattern(name: "\(testCase.site) pattern", site: testCase.site)
                if let style = testCase.style {
                    engine.setStyle(style)
                }
            }
            // A 3D frame of this engine leaves the shader renderer's GL state behind for its 2D renderer;
            // selecting the pattern again restarts the simulation like the web harness
            selectPattern()
            engine.setRenderMode(.threeD)
            engine.draw()
            engine.setRenderMode(testCase.mode)
            selectPattern()
            #expect(engine.engineType == testCase.engine)
            target.bind()
            engine.step(frames: 120)
            target.resolve(into: output.framebuffer)

            let actual = output.readPixels()
            let expected = try Self.loadBaseline(testCase.snapshot)
            #expect(expected.count == actual.count)

            var mismatches = 0
            for pixel in stride(from: 0, to: min(actual.count, expected.count), by: 4) {
                let diff = (0..<3).map { abs(Int(actual[pixel + $0]) - Int(expected[pixel + $0])) }.max() ?? 0
                if diff > Self.channelTolerance { mismatches += 1 }
            }
            let ratio = Double(mismatches) / Double(actual.count / 4)
            Self.saveIfRequested(actual, name: testCase.snapshot)
            #expect(ratio <= Self.maxMismatchRatio, "\(testCase.snapshot): \(mismatches) mismatching pixels")

            let objectsActual = Self.objectPixels(actual)
            let objectsExpected = Self.objectPixels(expected)
            #expect(objectsExpected > 0)
            #expect(
                abs(objectsActual - objectsExpected) <= Int(Double(objectsExpected) * Self.maxObjectPixelDifference),
                "\(testCase.snapshot): \(objectsActual) object pixels, expected \(objectsExpected)")
        }

        /// Saturated pixels: the objects are colored, the juggler and 2D lines are white or beige.
        static func objectPixels(_ rgba: [UInt8]) -> Int {
            stride(from: 0, to: rgba.count, by: 4).count { pixel in
                let channels = [rgba[pixel], rgba[pixel + 1], rgba[pixel + 2]]
                return Int(channels.max() ?? 0) - Int(channels.min() ?? 0) > 96
            }
        }

        static func loadBaseline(_ name: String) throws -> [UInt8] {
            let bundle = Bundle(for: BundleToken.self)
            let url = try #require(
                bundle.url(forResource: name, withExtension: "png", subdirectory: "visual.spec.ts-snapshots"))
            let source = try #require(CGImageSourceCreateWithURL(url as CFURL, nil))
            let image = try #require(CGImageSourceCreateImageAtIndex(source, 0, nil))
            return try rgba(of: image)
        }

        static func rgba(of image: CGImage) throws -> [UInt8] {
            var pixels = [UInt8](repeating: 0, count: image.width * image.height * 4)
            let drawn = pixels.withUnsafeMutableBytes { buffer -> Bool in
                guard
                    let context = CGContext(
                        data: buffer.baseAddress, width: image.width, height: image.height, bitsPerComponent: 8,
                        bytesPerRow: image.width * 4, space: CGColorSpace(name: CGColorSpace.sRGB)!,
                        bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue)
                else { return false }
                context.draw(image, in: CGRect(x: 0, y: 0, width: image.width, height: image.height))
                return true
            }
            try #require(drawn)
            return pixels
        }

        /// Writes the rendered frame as PNG when JM_SNAPSHOT_DIR is set, for inspecting failures.
        static func saveIfRequested(_ pixels: [UInt8], name: String) {
            guard let directory = ProcessInfo.processInfo.environment["JM_SNAPSHOT_DIR"] else { return }
            let width = EngineSuites.width
            let height = EngineSuites.height
            var data = pixels
            guard
                let context = CGContext(
                    data: &data, width: width, height: height, bitsPerComponent: 8, bytesPerRow: width * 4,
                    space: CGColorSpace(name: CGColorSpace.sRGB)!,
                    bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue),
                let image = context.makeImage(),
                let destination = CGImageDestinationCreateWithURL(
                    URL(fileURLWithPath: directory).appendingPathComponent("\(name).png") as CFURL,
                    "public.png" as CFString, 1, nil)
            else { return }
            CGImageDestinationAddImage(destination, image, nil)
            CGImageDestinationFinalize(destination)
        }
    }
}
