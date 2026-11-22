import type { Pattern } from './types';

export type SourceFilter = 'all' | 'jm' | 'js';

/** Partial, case-insensitive match on pattern name and siteswap data. */
export function filterPatterns(
  patterns: Pattern[],
  searchText: string,
  source: SourceFilter = 'all',
  categoryId?: string
): Pattern[] {
  const q = searchText.trim().toLowerCase();
  return patterns.filter(p =>
    (source === 'all' || p.source === source) &&
    (categoryId === undefined || p.categoryId === categoryId) &&
    (q === '' || p.name.toLowerCase().includes(q) || p.data.toLowerCase().includes(q))
  );
}
