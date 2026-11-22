/*
 * Types and constants for pattern library (JuggleMaster .jm and JuggleSaver formats).
 * Defaults/limits match src/jmlib/jmlib.h.
 */

export const HR_DEF = 0.17;
export const HR_MIN = 0.04;
export const HR_MAX = 1.0;
export const DR_DEF = 0.5;
export const DR_MIN = 0.1;
export const DR_MAX = 0.9;

export interface Pattern {
  id: string;
  name: string;
  data: string;
  style: string;
  categoryName: string;
  categoryId: string;
  hr?: number;
  dr?: number;
  source: 'jm' | 'js';
}

export interface Category {
  id: string;
  name: string;
  patternCount?: number;
  source: 'jm' | 'js';
}

export interface ParseJmResult {
  patterns: Pattern[];
  categories: Category[];
}

export type ParseJsResult = ParseJmResult;

export interface PatternLibrary {
  patterns: Pattern[];
  categories: Category[];
}
