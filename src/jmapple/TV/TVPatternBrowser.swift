import JugglerKit
import SwiftUI

/// The pattern library: recents, categories and search. Focusing a pattern previews it on the
/// juggler; Select keeps it.
struct TVPatternBrowser: View {
    let model: JugglerModel
    @Binding var filter: SourceFilter
    let onSearch: () -> Void
    let onCommit: () -> Void

    @State private var path: [JMKit.Category] = []

    var body: some View {
        NavigationStack(path: $path) {
            Group {
                if let library = model.library {
                    root(library)
                } else if model.libraryFailed {
                    Text(jmLocalized("menu.patternsLoadError"))
                } else {
                    ProgressView(jmLocalized("menu.patternsLoading"))
                }
            }
            .navigationDestination(for: JMKit.Category.self) { category in
                TVPatternList(
                    title: category.name, patterns: model.library?.patterns(in: category.id) ?? [], model: model,
                    onCommit: onCommit)
            }
        }
    }

    private func root(_ library: PatternLibrary) -> some View {
        VStack(alignment: .leading, spacing: 16) {
            HStack(spacing: 12) {
                ForEach(SourceFilter.allCases, id: \.self) { option in
                    Button(option.title) { filter = option }
                        .modifier(SelectedTint(selected: filter == option))
                        .accessibilityIdentifier("filter-\(option.rawValue)")
                }
                Button(action: onSearch) {
                    Label(jmLocalized("menu.search"), systemImage: "magnifyingglass")
                }
                .accessibilityIdentifier("search")
            }
            .focusSection()
            rootList(library)
        }
    }

    private func rootList(_ library: PatternLibrary) -> some View {
        List {
            if !model.recentPatterns.items.isEmpty {
                Section(jmLocalized("menu.recent")) {
                    ForEach(model.recentPatterns.items, id: \.self) { item in
                        Button {
                            if model.applyRecent(item) { onCommit() }
                        } label: {
                            HStack {
                                if item.isFavorite {
                                    Image(systemName: "star.fill").foregroundStyle(.yellow)
                                }
                                RecentPatternLabel(item)
                            }
                        }
                        .contextMenu {
                            Button(item.isFavorite ? jmLocalized("menu.unfavorite") : jmLocalized("menu.favorite")) {
                                model.recentPatterns.toggleFavorite(item)
                            }
                            Button(jmLocalized("menu.remove"), role: .destructive) {
                                model.recentPatterns.remove(item)
                            }
                        }
                    }
                }
            }

            Section(jmLocalized("menu.category")) {
                ForEach(library.categories(for: filter)) { category in
                    NavigationLink(category.name, value: category)
                        .accessibilityIdentifier("category-\(category.id)")
                }
            }
        }
    }
}

/// Patterns of a category or search; focus previews after a short pause, Select commits.
struct TVPatternList: View {
    let title: String
    let patterns: [Pattern]
    let model: JugglerModel
    let onCommit: () -> Void

    static let previewDelay: Duration = .milliseconds(500)

    @FocusState private var focused: Pattern.ID?

    var body: some View {
        List(patterns) { pattern in
            Button {
                if model.commitPreview(pattern) { onCommit() }
            } label: {
                VStack(alignment: .leading, spacing: 4) {
                    Text(pattern.name).lineLimit(1)
                    if pattern.data != pattern.name {
                        Text(pattern.data).font(.caption.monospaced()).foregroundStyle(.secondary).lineLimit(1)
                    }
                }
            }
            .focused($focused, equals: pattern.id)
            .accessibilityIdentifier("pattern-\(pattern.id)")
        }
        .navigationTitle(title)
        .task(id: focused) {
            guard let id = focused, let pattern = patterns.first(where: { $0.id == id }) else { return }
            try? await Task.sleep(for: Self.previewDelay)
            if !Task.isCancelled {
                model.preview(pattern)
            }
        }
    }
}

/// Full-screen search across the library: the tvOS search keyboard needs the whole width (it also takes
/// dictation and the iPhone's Continuity Keyboard). Select juggles a result.
struct TVPatternSearch: View {
    let model: JugglerModel
    let filter: SourceFilter
    let onCommit: () -> Void

    @State private var query = ""

    var body: some View {
        let results = query.isEmpty ? [] : (model.library?.search(query, source: filter) ?? [])
        NavigationStack {
            List(results) { pattern in
                Button {
                    if model.apply(pattern) { onCommit() }
                } label: {
                    HStack {
                        Text(pattern.name).lineLimit(1)
                        Spacer()
                        Text(pattern.data).font(.caption.monospaced()).foregroundStyle(.secondary).lineLimit(1)
                        Text(pattern.categoryName).font(.caption).foregroundStyle(.secondary).lineLimit(1)
                            .frame(maxWidth: 520, alignment: .trailing)
                    }
                }
                .accessibilityIdentifier("result-\(pattern.id)")
            }
            .searchable(text: $query, prompt: jmLocalized("menu.searchPlaceholder"))
            .overlay {
                if !query.isEmpty && results.isEmpty {
                    Text(jmLocalized("menu.searchNoResults")).foregroundStyle(.secondary)
                }
            }
        }
    }
}

/// Marks the selected option of a button group.
struct SelectedTint: ViewModifier {
    let selected: Bool

    func body(content: Content) -> some View {
        content
            .overlay(alignment: .bottom) {
                if selected {
                    Capsule().frame(height: 4).padding(.horizontal, 12).offset(y: 4)
                }
            }
            .accessibilityAddTraits(selected ? .isSelected : [])
    }
}
