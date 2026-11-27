import SwiftUI

/// Outline icons for the object picker, drawn in a 24×24 box like the web's ObjectIcons.tsx.
public struct ObjectIcon: View {
    public let type: ObjectType

    public init(_ type: ObjectType) {
        self.type = type
    }

    public var body: some View {
        switch type {
        case .ring:
            RingShape().fill(style: FillStyle(eoFill: true))
        case .default:
            Text(type.title)
        default:
            IconOutline(type: type).stroke(style: StrokeStyle(lineWidth: 1.5, lineCap: .round, lineJoin: .round))
        }
    }
}

private struct IconOutline: Shape {
    let type: ObjectType

    func path(in rect: CGRect) -> Path {
        let scale = min(rect.width, rect.height) / 24
        let transform = CGAffineTransform(translationX: rect.midX - 12 * scale, y: rect.midY - 12 * scale)
            .scaledBy(x: scale, y: scale)
        var path = Path()
        switch type {
        case .ball:
            path.addEllipse(in: CGRect(x: 3, y: 3, width: 18, height: 18))
            path.move(to: CGPoint(x: 7.5, y: 4.2))
            path.addQuadCurve(to: CGPoint(x: 14.5, y: 9.5), control: CGPoint(x: 10.8, y: 6.6))
            path.addQuadCurve(to: CGPoint(x: 16.5, y: 19.8), control: CGPoint(x: 16.6, y: 14.8))
            path.move(to: CGPoint(x: 4.2, y: 16.5))
            path.addQuadCurve(to: CGPoint(x: 14.5, y: 9.5), control: CGPoint(x: 9.2, y: 14.5))
            path.addQuadCurve(to: CGPoint(x: 19.8, y: 7.5), control: CGPoint(x: 17.3, y: 8.1))
        case .fish:
            path.move(to: CGPoint(x: 3, y: 12))
            path.addCurve(
                to: CGPoint(x: 16.5, y: 11.2), control1: CGPoint(x: 6, y: 7.5), control2: CGPoint(x: 12.5, y: 7.5))
            path.addLine(to: CGPoint(x: 21, y: 8.5))
            path.addCurve(
                to: CGPoint(x: 21, y: 15.5), control1: CGPoint(x: 20, y: 10.5), control2: CGPoint(x: 20, y: 13.5))
            path.addLine(to: CGPoint(x: 16.5, y: 12.8))
            path.addCurve(
                to: CGPoint(x: 3, y: 12), control1: CGPoint(x: 12.5, y: 16.5), control2: CGPoint(x: 6, y: 16.5))
            path.closeSubpath()
            path.addEllipse(in: CGRect(x: 6.05, y: 10.45, width: 1.5, height: 1.5))
        case .club:
            path.addEllipse(in: CGRect(x: 1.3, y: 10.7, width: 2.6, height: 2.6))
            path.move(to: CGPoint(x: 4.4, y: 11.5))
            path.addLine(to: CGPoint(x: 9.5, y: 11))
            path.addCurve(
                to: CGPoint(x: 16.5, y: 9.3), control1: CGPoint(x: 11.5, y: 9.8), control2: CGPoint(x: 14.5, y: 9.3))
            path.addCurve(
                to: CGPoint(x: 22.4, y: 12), control1: CGPoint(x: 19.9, y: 9.3), control2: CGPoint(x: 22.4, y: 10.4))
            path.addCurve(
                to: CGPoint(x: 16.5, y: 14.7), control1: CGPoint(x: 22.4, y: 13.6), control2: CGPoint(x: 19.9, y: 14.7))
            path.addCurve(
                to: CGPoint(x: 9.5, y: 13), control1: CGPoint(x: 14.5, y: 14.7), control2: CGPoint(x: 11.5, y: 14.2))
            path.addLine(to: CGPoint(x: 4.4, y: 12.5))
        case .ring, .default:
            break
        }
        return path.applying(transform)
    }
}

private struct RingShape: Shape {
    func path(in rect: CGRect) -> Path {
        let scale = min(rect.width, rect.height) / 24
        let center = CGPoint(x: rect.midX, y: rect.midY)
        var path = Path()
        for radius in [9 * scale, 6.75 * scale] {
            path.addEllipse(
                in: CGRect(x: center.x - radius, y: center.y - radius, width: radius * 2, height: radius * 2))
        }
        return path
    }
}
