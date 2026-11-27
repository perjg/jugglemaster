import JugglerKit
import SwiftUI

/// Siteswap entry with an on-screen keypad (siteswaps and JuggleSaver notation), live validation and
/// recent siteswaps.
struct TVSiteswapPanel: View {
    let model: JugglerModel
    let onCommit: () -> Void

    @State private var entry = ""

    /// Characters of both validators: siteswap digits a–z, multiplex/sync brackets, and JuggleSaver's
    /// object letters, positions (`@(x,y)>(x,y)`), snatches (`/`), spins (`*`), decimals and spaces.
    static let keys: [String] =
        (0...9).map(String.init) + "abcdefghijklmnopqrstuvwxyz".map(String.init)
        + ["[", "]", "(", ")", ",", ".", "-", "@", ">", "/", "*", " "]

    private let columns = Array(repeating: GridItem(.fixed(60), spacing: 8), count: 10)

    var body: some View {
        let validation = model.validate(entry)
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                entryField(validation)

                LazyVGrid(columns: columns, spacing: 8) {
                    ForEach(Self.keys, id: \.self) { key in
                        Button {
                            entry.append(key)
                        } label: {
                            Text(key == " " ? "␣" : key)
                                .font(.callout.monospaced())
                                .frame(width: 60, height: 56)
                                .background(.white.opacity(0.12), in: RoundedRectangle(cornerRadius: 10))
                        }
                        .buttonStyle(.plain)
                        .accessibilityLabel(key == " " ? jmLocalized("menu.space") : key)
                        .accessibilityIdentifier("key-\(key)")
                    }
                }

                HStack(spacing: 16) {
                    Button {
                        _ = entry.popLast()
                    } label: {
                        Image(systemName: "delete.left")
                    }
                    .accessibilityLabel(jmLocalized("menu.delete"))
                    .accessibilityIdentifier("delete")
                    Button {
                        entry = ""
                    } label: {
                        Image(systemName: "xmark")
                    }
                    .accessibilityLabel(jmLocalized("menu.clear"))
                    .accessibilityIdentifier("clear")
                    TextField(jmLocalized("menu.keyboard"), text: $entry)
                        .autocorrectionDisabled()
                        .textInputAutocapitalization(.never)
                        .accessibilityIdentifier("keyboard")
                    Button(jmLocalized("menu.juggle")) {
                        if model.submitSiteswap(entry) {
                            entry = ""
                            onCommit()
                        }
                    }
                    .disabled(validation?.isValid != true)
                    .accessibilityIdentifier("juggle")
                }

                HStack {
                    Text(jmLocalized("menu.engine"))
                    Text(model.actualEngine.title).foregroundStyle(.secondary)
                    if model.engineMismatch {
                        Image(systemName: "exclamationmark.triangle.fill").foregroundStyle(.yellow)
                    }
                }
                .font(.callout)

                if !model.recentSiteswaps.items.isEmpty {
                    Text(jmLocalized("menu.recent")).font(.headline)
                    ForEach(model.recentSiteswaps.items, id: \.self) { item in
                        Button {
                            if model.applyRecent(item) { onCommit() }
                        } label: {
                            HStack {
                                if item.isFavorite {
                                    Image(systemName: "star.fill").foregroundStyle(.yellow)
                                }
                                RecentSiteswapLabel(item)
                            }
                        }
                        .contextMenu {
                            Button(item.isFavorite ? jmLocalized("menu.unfavorite") : jmLocalized("menu.favorite")) {
                                model.recentSiteswaps.toggleFavorite(item)
                            }
                            Button(jmLocalized("menu.remove"), role: .destructive) {
                                model.recentSiteswaps.remove(item)
                            }
                        }
                    }
                }
            }
            .padding(.vertical, 8)
        }
    }

    private func entryField(_ validation: SiteswapValidation?) -> some View {
        let tint: Color =
            switch validation?.isValid {
            case true: .green
            case false: .red
            case nil: .secondary
            }
        return HStack {
            Text(entry.isEmpty ? jmLocalized("menu.siteswapPlaceholder") : entry)
                .font(.title3.monospaced())
                .foregroundStyle(entry.isEmpty ? .secondary : .primary)
                .lineLimit(2)
                .frame(maxWidth: .infinity, alignment: .leading)
                .accessibilityIdentifier("entry")
            if let validation, validation.isValid {
                if validation.juggleMaster { badge(jmLocalized("menu.filterJm")) }
                if validation.juggleSaver { badge(jmLocalized("menu.filterJs")) }
            }
        }
        .padding(16)
        .background(tint.opacity(0.2), in: RoundedRectangle(cornerRadius: 12))
        .overlay(RoundedRectangle(cornerRadius: 12).stroke(tint, lineWidth: 2))
        .accessibilityElement(children: .combine)
        .accessibilityValue(
            validation.map { $0.isValid ? jmLocalized("menu.valid") : jmLocalized("menu.invalid") } ?? "")
    }

    private func badge(_ text: String) -> some View {
        Text(text)
            .font(.caption.bold())
            .padding(.horizontal, 8)
            .padding(.vertical, 2)
            .background(.secondary.opacity(0.3), in: RoundedRectangle(cornerRadius: 4))
    }
}
