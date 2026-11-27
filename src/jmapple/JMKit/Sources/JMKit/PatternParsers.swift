import Foundation

// Ports of src/jmwasm/ui/src/lib/patterns/parseJm.ts and parseJs.ts. Ids must match the web app:
// share links carry the category id.

extension PatternLibrary {
    /// Parses the JuggleMaster `.jm` format (`/[ Category ]`, `%Style`, `#HR=`, `#DR=`, `site name`).
    static func parseJuggleMaster(_ content: String) -> PatternLibrary {
        var patterns: [Pattern] = []
        var categories: [Category] = []
        var categoryIdsByName: [String: String] = [:]

        var categoryName: String?
        var categoryId: String?
        var style = PatternDefaults.style
        var hr = PatternDefaults.hr
        var dr = PatternDefaults.dr

        for rawLine in splitLines(content) {
            let line = trimJS(rawLine)
            if line.isEmpty || line.hasPrefix(";") { continue }

            if let value = numberAfter("#HR=", in: line) {
                hr = value.clamped(to: PatternDefaults.hrRange)
                continue
            }
            if let value = numberAfter("#DR=", in: line) {
                dr = value.clamped(to: PatternDefaults.drRange)
                continue
            }

            if let name = parseCategoryLine(line) {
                style = PatternDefaults.style
                hr = PatternDefaults.hr
                dr = PatternDefaults.dr
                categoryName = name
                if categoryIdsByName[name] == nil {
                    let id = slug(name).isEmpty ? "category-\(categories.count)" : slug(name)
                    categoryIdsByName[name] = id
                    categories.append(Category(id: id, name: name, source: .jm))
                }
                categoryId = categoryIdsByName[name]
                continue
            }

            if line.hasPrefix("%") {
                let name = trimJS(String(line.dropFirst()))
                style = name.isEmpty ? PatternDefaults.style : name
                continue
            }

            if line.hasPrefix("{") { continue }
            guard let first = line.unicodeScalars.first, isLegalPatternStart(first),
                let categoryName, let categoryId
            else { continue }

            let separator = line.contains("\t") ? "\t" : " "
            let data: String
            var name: String
            if let range = line.range(of: separator) {
                data = trimJS(String(line[..<range.lowerBound]))
                name = trimJS(String(line[range.upperBound...]))
                if name.isEmpty { name = data }
            } else {
                data = line
                name = data
            }

            let dataSlug = slug(prefixUTF16(data, 30))
            let nameSlug = slug(prefixUTF16(name, 50))
            let id =
                "\(categoryId)-\(nameSlug.isEmpty ? "p" : nameSlug)-\(dataSlug.isEmpty ? "d" : dataSlug)-\(patterns.count)"
            patterns.append(
                Pattern(
                    id: id, name: name, data: data, style: style, categoryName: categoryName,
                    categoryId: categoryId, hr: hr, dr: dr, source: .jm))
        }

        return PatternLibrary(categories: categories, patterns: patterns)
    }

    /// Parses the JuggleSaver `moresites.txt` format (`$ Group`, `"Name (Author)" pattern;`).
    static func parseJuggleSaver(_ content: String) -> PatternLibrary {
        var patterns: [Pattern] = []
        var categories: [Category] = []
        var categoryIdsByName: [String: String] = [:]

        var baseGroup = ""
        var currentGroup = ""
        var skipHeader = true

        var readName = false
        var readAuthor = false
        var readPattern = false
        var name = ""
        var data = ""
        var categoryName = ""
        var categoryId = ""

        for line in splitLines(content) {
            let trimmed = trimJS(line)
            if skipHeader {
                if trimmed.isEmpty { continue }
                skipHeader = false
            }

            if trimmed.hasPrefix("$") {
                let rest = trimJS(String(trimmed.dropFirst()))
                if rest.hasPrefix("----------") { continue }
                if rest.hasPrefix("(") {
                    baseGroup = currentGroup
                    continue
                }
                currentGroup = rest
                continue
            }

            if !readPattern && trimmed.isEmpty { continue }
            if readPattern && !data.isEmpty && !trimmed.isEmpty {
                data += " "
            }

            let chars = Array(line)
            var index = 0
            while index < chars.count {
                let char = chars[index]
                defer { index += 1 }

                if char == "\"" {
                    if readName {
                        readName = false
                        readPattern = true
                        while index + 1 < chars.count && chars[index + 1] == " " {
                            index += 1
                        }
                        continue
                    }
                    readName = true
                    readAuthor = false
                    readPattern = false
                    name = ""
                    data = ""
                    if !currentGroup.isEmpty {
                        categoryName = juggleSaverCategoryName(base: baseGroup, current: currentGroup)
                        if categoryIdsByName[categoryName] == nil {
                            let idSlug = slug(categoryName)
                            let id = idSlug.isEmpty ? "js-cat-\(categories.count)" : "js-\(idSlug)"
                            categoryIdsByName[categoryName] = id
                            categories.append(Category(id: id, name: categoryName, source: .js))
                        }
                        categoryId = categoryIdsByName[categoryName] ?? ""
                    }
                    continue
                }

                if readName && char == "(" && !readAuthor {
                    readAuthor = true
                    continue
                }
                if readName && char == ")" && readAuthor {
                    readAuthor = false
                    continue
                }
                if readName && readAuthor { continue }

                if char == ";" && readPattern {
                    readPattern = false
                    let dataTrimmed = trimJS(data)
                    if !dataTrimmed.isEmpty && !categoryId.isEmpty {
                        let nameTrimmed = trimJS(name)
                        let nameSlug = slug(nameTrimmed)
                        let id = "\(categoryId)-\(nameSlug.isEmpty ? "p" : nameSlug)-\(patterns.count)"
                        patterns.append(
                            Pattern(
                                id: id, name: nameTrimmed.isEmpty ? dataTrimmed : nameTrimmed, data: dataTrimmed,
                                style: PatternDefaults.juggleSaverStyle, categoryName: categoryName,
                                categoryId: categoryId, source: .js))
                    }
                    data = ""
                    continue
                }

                if readName && !readAuthor {
                    name.append(char)
                } else if readPattern {
                    data.append(char)
                }
            }
        }

        return PatternLibrary(categories: categories, patterns: patterns)
    }
}

private func juggleSaverCategoryName(base: String, current: String) -> String {
    let suffix = " (JuggleSaver)"
    if !base.isEmpty && base != current {
        return "\(base) - \(current)\(suffix)"
    }
    return "\(current)\(suffix)"
}

/// `/[ Name ]` → "Name".
private func parseCategoryLine(_ line: String) -> String? {
    guard line.hasPrefix("/["), line.hasSuffix("]") else { return nil }
    return trimJS(String(line.dropFirst(2).dropLast()))
}

/// `parseFloat` of the number matched by `^PREFIX(\d*\.?\d+)`.
private func numberAfter(_ prefix: String, in line: String) -> Double? {
    guard line.hasPrefix(prefix),
        let match = line.dropFirst(prefix.count).prefixMatch(of: /\d*\.?\d+/)
    else { return nil }
    return Double(match.output)
}

private func isLegalPatternStart(_ scalar: Unicode.Scalar) -> Bool {
    scalar == "[" || scalar == "(" || ("a"..."z").contains(scalar) || ("A"..."Z").contains(scalar)
        || ("0"..."9").contains(scalar)
}

extension Comparable {
    func clamped(to range: ClosedRange<Self>) -> Self {
        min(max(self, range.lowerBound), range.upperBound)
    }
}
