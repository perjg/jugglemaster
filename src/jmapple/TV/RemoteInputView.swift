import JugglerKit
import SwiftUI
import UIKit

/// Siri Remote input for watching mode: swipe to orbit, click for the controls, hold to toggle
/// auto-rotation, Play/Pause, arrow clicks for camera presets (left/right) and zoom (up/down).
/// Back (Menu) is not handled, so the system returns to the Home screen.
struct RemoteInput: UIViewRepresentable {
    var isActive: Bool
    var onSelect: () -> Void
    var onLongSelect: () -> Void
    var onPlayPause: () -> Void
    var onArrow: (UIPress.PressType) -> Void
    var onOrbit: (Float, Float) -> Void

    func makeUIView(context: Context) -> RemoteInputView {
        RemoteInputView()
    }

    func updateUIView(_ view: RemoteInputView, context: Context) {
        view.onSelect = onSelect
        view.onLongSelect = onLongSelect
        view.onPlayPause = onPlayPause
        view.onArrow = onArrow
        view.onOrbit = onOrbit
        view.isActive = isActive
    }
}

final class RemoteInputView: UIView {
    static let longPressDuration: TimeInterval = 0.6
    /// Swipe distance (points of the touch surface) that orbits by one view width, like dragging across the web canvas.
    static let orbitDistance: CGFloat = 1600

    var onSelect: () -> Void = {}
    var onLongSelect: () -> Void = {}
    var onPlayPause: () -> Void = {}
    var onArrow: (UIPress.PressType) -> Void = { _ in }
    var onOrbit: (Float, Float) -> Void = { _, _ in }

    var isActive = true {
        didSet {
            guard isActive != oldValue else { return }
            // Ask from the top of the window: an update scoped to this view finds nothing else to focus
            // when it stops accepting focus, and focus would stay here with the controls unreachable.
            let environment: UIFocusEnvironment = window?.rootViewController ?? self
            environment.setNeedsFocusUpdate()
            environment.updateFocusIfNeeded()
        }
    }

    private var lastTranslation = CGPoint.zero
    private var longPressTask: Task<Void, Never>?
    private var longPressFired = false

    override init(frame: CGRect) {
        super.init(frame: frame)
        backgroundColor = .clear
        let pan = UIPanGestureRecognizer(target: self, action: #selector(handlePan))
        pan.allowedTouchTypes = [NSNumber(value: UITouch.TouchType.indirect.rawValue)]
        addGestureRecognizer(pan)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) is not supported")
    }

    override var canBecomeFocused: Bool { isActive }

    @objc private func handlePan(_ pan: UIPanGestureRecognizer) {
        guard isActive else { return }
        let translation = pan.translation(in: self)
        switch pan.state {
        case .began:
            lastTranslation = translation
        case .changed:
            let dx = (translation.x - lastTranslation.x) / Self.orbitDistance
            let dy = (translation.y - lastTranslation.y) / Self.orbitDistance
            lastTranslation = translation
            onOrbit(Float(dx), Float(dy))
        default:
            lastTranslation = .zero
        }
    }

    override func pressesBegan(_ presses: Set<UIPress>, with event: UIPressesEvent?) {
        guard isActive, let press = presses.first else {
            super.pressesBegan(presses, with: event)
            return
        }
        switch press.type {
        case .select:
            longPressFired = false
            longPressTask = Task { [weak self] in
                try? await Task.sleep(for: .seconds(Self.longPressDuration))
                guard let self, !Task.isCancelled else { return }
                longPressFired = true
                onLongSelect()
            }
        case .playPause, .upArrow, .downArrow, .leftArrow, .rightArrow:
            break
        default:
            super.pressesBegan(presses, with: event)
        }
    }

    override func pressesEnded(_ presses: Set<UIPress>, with event: UIPressesEvent?) {
        guard isActive, let press = presses.first else {
            super.pressesEnded(presses, with: event)
            return
        }
        switch press.type {
        case .select:
            longPressTask?.cancel()
            if !longPressFired {
                onSelect()
            }
        case .playPause:
            onPlayPause()
        case .upArrow, .downArrow, .leftArrow, .rightArrow:
            onArrow(press.type)
        default:
            super.pressesEnded(presses, with: event)
        }
    }

    override func pressesCancelled(_ presses: Set<UIPress>, with event: UIPressesEvent?) {
        longPressTask?.cancel()
        super.pressesCancelled(presses, with: event)
    }
}
