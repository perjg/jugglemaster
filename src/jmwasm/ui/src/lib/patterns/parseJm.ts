import type { Category, Pattern } from './types';
import {
  DR_DEF,
  DR_MAX,
  DR_MIN,
  HR_DEF,
  HR_MAX,
  HR_MIN,
  type ParseJmResult,
} from './types';

function slug(s: string): string {
  return s
    .trim()
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, '-')
    .replace(/^-|-$/g, '');
}

function legalPatternFirstChar(c: string): boolean {
  if (c.length === 0) return false;
  const ch = c[0];
  return (
    ch === '[' ||
    ch === '(' ||
    (ch >= 'a' && ch <= 'z') ||
    (ch >= 'A' && ch <= 'Z') ||
    (ch >= '0' && ch <= '9')
  );
}

function clampHr(v: number): number {
  if (v < HR_MIN) return HR_MIN;
  if (v > HR_MAX) return HR_MAX;
  return v;
}

function clampDr(v: number): number {
  if (v < DR_MIN) return DR_MIN;
  if (v > DR_MAX) return DR_MAX;
  return v;
}

/**
 * Extract category name from a line like "/[ 3-Cascade Step By Step ]".
 * Returns null if the line is not a category line.
 */
function parseCategoryLine(line: string): string | null {
  const trimmed = line.trim();
  if (!trimmed.startsWith('/[') || !trimmed.endsWith(']')) return null;
  const inner = trimmed.slice(2, -1).trim();
  return inner;
}

export function parseJm(content: string): ParseJmResult {
  const patterns: Pattern[] = [];
  const categories: Category[] = [];
  const categoryIdsByName = new Map<string, string>();

  let currentCategoryName: string | null = null;
  let currentCategoryId: string | null = null;
  let currentStyle = 'Normal';
  let currentHr = HR_DEF;
  let currentDr = DR_DEF;

  const lines = content.split(/\r?\n/);

  for (const rawLine of lines) {
    const line = rawLine.trim();
    if (line.length === 0) continue;
    if (line.startsWith(';')) continue;

    const hrRegex = /^#HR=(\d*\.?\d+)/;
    const hrMatch = hrRegex.exec(line);
    if (hrMatch) {
      const v = parseFloat(hrMatch[1]);
      if (!Number.isNaN(v)) currentHr = clampHr(v);
      continue;
    }

    const drRegex = /^#DR=(\d*\.?\d+)/;
    const drMatch = drRegex.exec(line);
    if (drMatch) {
      const v = parseFloat(drMatch[1]);
      if (!Number.isNaN(v)) currentDr = clampDr(v);
      continue;
    }

    const categoryName = parseCategoryLine(line);
    if (categoryName !== null) {
      currentStyle = 'Normal';
      currentHr = HR_DEF;
      currentDr = DR_DEF;
      currentCategoryName = categoryName;
      if (!categoryIdsByName.has(categoryName)) {
        const id = slug(categoryName) || `category-${String(categories.length)}`;
        categoryIdsByName.set(categoryName, id);
        categories.push({ id, name: categoryName, source: 'jm' });
      }
      currentCategoryId = categoryIdsByName.get(categoryName) ?? null;
      continue;
    }

    if (line.startsWith('%')) {
      const styleName = line.slice(1).trim();
      currentStyle = styleName.length > 0 ? styleName : 'Normal';
      continue;
    }

    if (line.startsWith('{')) {
      continue;
    }

    if (!legalPatternFirstChar(line)) continue;
    if (currentCategoryName === null || currentCategoryId === null) continue;

    const firstSpace = line.includes('\t') ? line.indexOf('\t') : line.indexOf(' ');
    let data: string;
    let name: string;
    if (firstSpace >= 0) {
      data = line.slice(0, firstSpace).trim();
      name = line.slice(firstSpace + 1).trim();
      if (name.length === 0) name = data;
    } else {
      data = line.trim();
      name = data;
    }

    const dataSlug = slug(data.slice(0, 30));
    const nameSlug = slug(name.slice(0, 50));
    const id = `${currentCategoryId}-${nameSlug || 'p'}-${dataSlug || 'd'}-${String(patterns.length)}`;

    patterns.push({
      id,
      name,
      data,
      style: currentStyle,
      categoryName: currentCategoryName,
      categoryId: currentCategoryId,
      hr: currentHr,
      dr: currentDr,
      source: 'jm',
    });
  }

  return { patterns, categories };
}
