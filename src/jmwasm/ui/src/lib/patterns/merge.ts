import type { PatternLibrary } from './types';
import type { ParseJmResult } from './types';

export function mergePatternLibraries(
  jm: ParseJmResult,
  js: ParseJmResult
): PatternLibrary {
  return {
    categories: [...jm.categories, ...js.categories],
    patterns: [...jm.patterns, ...js.patterns],
  };
}
