import JugglerKit
import SwiftUI

/// The web app's menu: siteswap entry or pattern library, then display settings and recents. Shown as
/// the sidebar on iPad and Mac and in a sheet on iPhone.
struct MenuView: View {
    enum Tab: Hashable {
        case siteswap
        case pattern
    }

    @Bindable var model: JugglerModel
    /// Called after a pattern was picked, so a compact layout can get out of the way.
    var onPatternChosen: () -> Void = {}

    @State private var tab: Tab
    @State private var siteswap = ""

    init(model: JugglerModel, onPatternChosen: @escaping () -> Void = {}) {
        self.model = model
        self.onPatternChosen = onPatternChosen
        _tab = State(initialValue: model.state.fromLibrary ? .pattern : .siteswap)
    }

    var body: some View {
        Form {
            Section {
                Picker(jmLocalized("menu.menu"), selection: $tab) {
                    Text(jmLocalized("menu.tabSite")).tag(Tab.siteswap)
                    Text(jmLocalized("menu.tabPattern")).tag(Tab.pattern)
                }
                .pickerStyle(.segmented)
                .labelsHidden()
                .accessibilityIdentifier("menuTab")
            }

            switch tab {
            case .siteswap: SiteswapSection(model: model, text: $siteswap, onSubmit: onPatternChosen)
            case .pattern: PatternSection(model: model, onChoose: onPatternChosen)
            }

            settingsSection

            switch tab {
            case .siteswap: recentSiteswaps
            case .pattern: recentPatterns
            }
        }
        .formStyle(.grouped)
    }

    private var settingsSection: some View {
        Section {
            Toggle(
                isOn: Binding(get: { model.renderMode == .threeD }, set: { model.setRenderMode($0 ? .threeD : .twoD) })
            ) {
                LabeledContent(jmLocalized("menu.mode"), value: model.renderMode == .threeD ? "3D" : "2D")
            }
            .accessibilityIdentifier("mode")
            Toggle(jmLocalized("menu.autoRotate"), isOn: Binding(get: { model.autoRotate }, set: model.setAutoRotate))
                .accessibilityIdentifier("autoRotate")
            Toggle(jmLocalized("menu.showSiteswap"), isOn: $model.showSiteswap)
                .accessibilityIdentifier("showSiteswap")
            VStack(alignment: .leading) {
                LabeledContent(jmLocalized("menu.speed")) {
                    Text(model.speed, format: .number.precision(.fractionLength(1))).monospacedDigit()
                }
                Slider(
                    value: Binding(get: { model.speed }, set: { model.setSpeed($0, commit: false) }),
                    in: AppState.speedRange, step: JugglerModel.speedStep
                ) { editing in
                    if !editing { model.setSpeed(model.speed) }
                }
                .accessibilityLabel(jmLocalized("menu.speed"))
                .accessibilityIdentifier("speed")
            }
            VStack(alignment: .leading) {
                Text(jmLocalized("menu.objects"))
                ObjectTypePicker(model: model, iconSize: 32)
            }
        }
    }

    @ViewBuilder private var recentSiteswaps: some View {
        if !model.recentSiteswaps.items.isEmpty {
            Section(jmLocalized("menu.recent")) {
                ForEach(model.recentSiteswaps.items, id: \.self) { item in
                    RecentRow(
                        isFavorite: item.isFavorite,
                        onApply: {
                            if model.applyRecent(item) { onPatternChosen() }
                        },
                        onToggleFavorite: { model.recentSiteswaps.toggleFavorite(item) },
                        onRemove: { model.recentSiteswaps.remove(item) }
                    ) {
                        RecentSiteswapLabel(item)
                    }
                }
            }
        }
    }

    @ViewBuilder private var recentPatterns: some View {
        if !model.recentPatterns.items.isEmpty {
            Section(jmLocalized("menu.recent")) {
                ForEach(model.recentPatterns.items, id: \.self) { item in
                    RecentRow(
                        isFavorite: item.isFavorite,
                        onApply: {
                            if model.applyRecent(item) { onPatternChosen() }
                        },
                        onToggleFavorite: { model.recentPatterns.toggleFavorite(item) },
                        onRemove: { model.recentPatterns.remove(item) }
                    ) {
                        RecentPatternLabel(item)
                    }
                }
            }
        }
    }
}

/// Typed siteswap with live validation, style and engine preference.
private struct SiteswapSection: View {
    @Bindable var model: JugglerModel
    @Binding var text: String
    let onSubmit: () -> Void

    @State private var selection: TextSelection?

    var body: some View {
        let validation = model.validate(text)
        Section {
            HStack {
                Text(jmLocalized("menu.enterSiteswap")).font(.subheadline.weight(.medium))
                Spacer()
                if let validation, validation.isValid {
                    if validation.juggleMaster { Badge(text: jmLocalized("menu.filterJm")) }
                    if validation.juggleSaver { Badge(text: jmLocalized("menu.filterJs")) }
                }
            }
            TextField(jmLocalized("menu.siteswapPlaceholder"), text: $text, selection: $selection)
                .font(.body.monospaced())
                .autocorrectionDisabled()
                #if os(iOS)
            .textInputAutocapitalization(.never)
            .keyboardType(.asciiCapable)
            .submitLabel(.go)
            .toolbar {
                ToolbarItemGroup(placement: .keyboard) {
                    ForEach(["[", "]", "(", ")", ",", "x"], id: \.self) { symbol in
                        Button(symbol) { insert(symbol) }
                        .font(.body.monospaced())
                    }
                }
            }
                #endif
                .padding(6)
                .background(tint(validation).opacity(0.15), in: RoundedRectangle(cornerRadius: 6))
                .overlay(
                    RoundedRectangle(cornerRadius: 6).stroke(tint(validation), lineWidth: validation == nil ? 0 : 1)
                )
                .onSubmit {
                    if model.submitSiteswap(text) {
                        text = ""
                        onSubmit()
                    }
                }
                .accessibilityIdentifier("siteswapField")

            Picker(jmLocalized("menu.style"), selection: Binding(get: { model.selectedStyle }, set: model.setStyle)) {
                ForEach(JugglerModel.styles, id: \.self) { Text($0).tag($0) }
            }
            .disabled(!model.isStyleEnabled)
            .accessibilityIdentifier("style")

            Toggle(
                isOn: Binding(
                    get: { model.preferredEngine == .juggleSaver },
                    set: { model.setPreferredEngine($0 ? .juggleSaver : .juggleMaster) })
            ) {
                HStack {
                    LabeledContent(jmLocalized("menu.engine"), value: model.preferredEngine.title)
                    if model.engineMismatch {
                        Image(systemName: "exclamationmark.triangle.fill")
                            .foregroundStyle(.yellow)
                            .accessibilityLabel(model.actualEngine.title)
                    }
                }
            }
            .accessibilityIdentifier("engine")
        }
    }

    /// Replaces the selection (or inserts at the cursor) like typing would.
    private func insert(_ symbol: String) {
        guard let selection, case .selection(let range) = selection.indices else {
            text.append(symbol)
            return
        }
        let offset = text.distance(from: text.startIndex, to: range.lowerBound)
        text.replaceSubrange(range, with: symbol)
        let cursor = text.index(text.startIndex, offsetBy: offset + symbol.count)
        self.selection = TextSelection(insertionPoint: cursor)
    }

    private func tint(_ validation: SiteswapValidation?) -> Color {
        switch validation?.isValid {
        case true: .green
        case false: .red
        case nil: .clear
        }
    }
}

/// Library search and category browsing.
private struct PatternSection: View {
    let model: JugglerModel
    let onChoose: () -> Void

    static let maxSearchResults = 50

    @State private var query = ""
    @State private var filter = SourceFilter.all
    @State private var categoryId: String?

    var body: some View {
        Section {
            if let library = model.library {
                content(library)
            } else if model.libraryFailed {
                Text(jmLocalized("menu.patternsLoadError")).foregroundStyle(.red)
            } else {
                Text(jmLocalized("menu.patternsLoading")).foregroundStyle(.secondary)
            }
        }
    }

    @ViewBuilder private func content(_ library: PatternLibrary) -> some View {
        TextField(jmLocalized("menu.searchPlaceholder"), text: $query)
            .autocorrectionDisabled()
            .accessibilityIdentifier("patternSearch")
        Picker(jmLocalized("menu.filterAll"), selection: $filter) {
            ForEach(SourceFilter.allCases, id: \.self) { Text($0.title).tag($0) }
        }
        .pickerStyle(.segmented)
        .labelsHidden()

        if query.trimmingCharacters(in: .whitespaces).isEmpty {
            let categories = library.categories(for: filter)
            Picker(jmLocalized("menu.category"), selection: categorySelection(categories)) {
                Text(jmLocalized("menu.patternsSelectCategory")).tag(String?.none)
                ForEach(categories) { Text($0.name).tag(Optional($0.id)) }
            }
            .accessibilityIdentifier("category")
            if let id = categorySelection(categories).wrappedValue {
                patternRows(library.patterns(in: id))
            } else {
                Text(jmLocalized("menu.patternsNoCategory")).foregroundStyle(.secondary)
            }
        } else {
            let results = library.search(query, source: filter)
            if results.isEmpty {
                Text(jmLocalized("menu.searchNoResults")).foregroundStyle(.secondary)
            }
            patternRows(Array(results.prefix(Self.maxSearchResults)))
            if results.count > Self.maxSearchResults {
                Text(
                    String(
                        format: jmLocalized("menu.searchTruncated"), Self.maxSearchResults, results.count)
                )
                .font(.caption)
                .foregroundStyle(.secondary)
            }
        }
    }

    /// The chosen category, else the one of the juggled library pattern; reset when filtered out.
    private func categorySelection(_ categories: [JMKit.Category]) -> Binding<String?> {
        Binding(
            get: {
                let id = categoryId ?? (model.state.fromLibrary ? model.state.categoryId : nil)
                return categories.contains { $0.id == id } ? id : nil
            },
            set: { categoryId = $0 })
    }

    private func patternRows(_ patterns: [JMKit.Pattern]) -> some View {
        ForEach(patterns) { pattern in
            let current =
                model.state.fromLibrary && model.state.categoryId == pattern.categoryId
                && model.state.pattern == pattern.data && model.state.name == pattern.name
                && model.state.style == pattern.effectiveStyle
            Button {
                categoryId = pattern.categoryId
                if model.apply(pattern) { onChoose() }
            } label: {
                HStack {
                    VStack(alignment: .leading, spacing: 2) {
                        Text(pattern.name).lineLimit(1)
                        if pattern.data != pattern.name {
                            Text(pattern.data).font(.caption.monospaced()).foregroundStyle(.secondary).lineLimit(1)
                        }
                    }
                    Spacer()
                    if current {
                        Image(systemName: "checkmark").foregroundStyle(.tint)
                    }
                }
                .contentShape(Rectangle())
            }
            .buttonStyle(.plain)
            .accessibilityAddTraits(current ? .isSelected : [])
        }
    }
}

/// A recent entry with favorite and remove actions (inline, in the context menu and as swipe actions).
private struct RecentRow<Label: View>: View {
    let isFavorite: Bool
    let onApply: () -> Void
    let onToggleFavorite: () -> Void
    let onRemove: () -> Void
    @ViewBuilder let label: Label

    var body: some View {
        HStack(spacing: 8) {
            Button(action: onApply) {
                label.contentShape(Rectangle())
            }
            .buttonStyle(.plain)
            Button(action: onToggleFavorite) {
                Image(systemName: isFavorite ? "star.fill" : "star")
                    .foregroundStyle(isFavorite ? .yellow : .secondary)
            }
            .buttonStyle(.borderless)
            .accessibilityLabel(isFavorite ? jmLocalized("menu.unfavorite") : jmLocalized("menu.favorite"))
            Button(action: onRemove) {
                Image(systemName: "xmark").foregroundStyle(.secondary)
            }
            .buttonStyle(.borderless)
            .accessibilityLabel(jmLocalized("menu.remove"))
        }
        .contextMenu {
            Button(isFavorite ? jmLocalized("menu.unfavorite") : jmLocalized("menu.favorite"), action: onToggleFavorite)
            Button(jmLocalized("menu.remove"), role: .destructive, action: onRemove)
        }
        #if os(iOS)
        .swipeActions {
            Button(jmLocalized("menu.remove"), role: .destructive, action: onRemove)
        }
        #endif
    }
}

private struct Badge: View {
    let text: String

    var body: some View {
        Text(text)
            .font(.caption2.weight(.medium))
            .padding(.horizontal, 6)
            .padding(.vertical, 2)
            .background(.quaternary, in: RoundedRectangle(cornerRadius: 4))
            .foregroundStyle(.secondary)
    }
}
