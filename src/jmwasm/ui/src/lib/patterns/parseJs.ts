import type { Category, Pattern } from './types';
import { DR_DEF, HR_DEF } from './types';
import type { ParseJmResult } from './types';

function slug(s: string): string {
  return s
    .trim()
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, '-')
    .replace(/^-|-$/g, '');
}

const JS_SUFFIX = ' (JuggleSaver)';

function getCategoryDisplayName(baseGroup: string, currentGroup: string): string {
  if (baseGroup.length > 0 && baseGroup !== currentGroup) {
    return `${baseGroup} - ${currentGroup}${JS_SUFFIX}`;
  }
  return `${currentGroup}${JS_SUFFIX}`;
}

export function parseJs(content: string): ParseJmResult {
  const patterns: Pattern[] = [];
  const categories: Category[] = [];
  const categoryIdsByName = new Map<string, string>();

  let baseGroup = '';
  let currentGroup = '';
  let skipHeader = true;

  let readName = false;
  let readAuthor = false;
  let readPattern = false;
  let name = '';
  let data = '';
  let currentCategoryName = '';
  let currentCategoryId = '';

  const lines = content.split(/\r?\n/);

  for (const line of lines) {
    if (skipHeader) {
      if (line.trim().length > 0) skipHeader = false;
      else continue;
    }
    const trimmed = line.trim();
    if (trimmed.startsWith('$')) {
      const rest = trimmed.slice(1).trim();
      if (rest.startsWith('----------')) continue;
      if (rest.startsWith('(')) {
        baseGroup = currentGroup;
        continue;
      }
      currentGroup = rest;
      continue;
    }

    if (!readPattern && trimmed.length === 0) continue;

    if (readPattern && data.length > 0 && trimmed.length > 0) {
      data += ' ';
    }

    for (let i = 0; i < line.length; i++) {
      const c = line.charAt(i);

      if (c === '"') {
        if (readName) {
          readName = false;
          readPattern = true;
          let j = i + 1;
          while (j < line.length && line[j] === ' ') j++;
          i = j - 1;
          continue;
        }
        readName = true;
        readAuthor = false;
        readPattern = false;
        name = '';
        data = '';
        if (currentGroup.length > 0) {
          currentCategoryName = getCategoryDisplayName(baseGroup, currentGroup);
          if (!categoryIdsByName.has(currentCategoryName)) {
            const idSlug = slug(currentCategoryName);
            const id = idSlug ? `js-${idSlug}` : `js-cat-${String(categories.length)}`;
            categoryIdsByName.set(currentCategoryName, id);
            categories.push({ id, name: currentCategoryName, source: 'js' });
          }
          currentCategoryId = categoryIdsByName.get(currentCategoryName) ?? '';
        }
        continue;
      }

      if (readName && c === '(' && !readAuthor) {
        readAuthor = true;
        continue;
      }
      if (readName && c === ')' && readAuthor) {
        readAuthor = false;
        continue;
      }
      if (readName && readAuthor) {
        continue;
      }
      if (c === ';' && readPattern) {
        readPattern = false;
        const dataTrimmed = data.trim();
        if (dataTrimmed.length > 0 && currentCategoryId) {
          const nameTrimmed = name.trim();
          const nameSlug = slug(nameTrimmed);
          const id = `${currentCategoryId}-${nameSlug ? nameSlug : 'p'}-${String(patterns.length)}`;
          patterns.push({
            id,
            name: nameTrimmed || dataTrimmed,
            data: dataTrimmed,
            style: 'JuggleSaver',
            categoryName: currentCategoryName,
            categoryId: currentCategoryId,
            hr: HR_DEF,
            dr: DR_DEF,
            source: 'js',
          });
        }
        data = '';
        continue;
      }

      if (readName && !readAuthor) name += c;
      else if (readPattern) data += c;
    }
  }

  return { patterns, categories };
}
