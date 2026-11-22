import { mergePatternLibraries } from './merge';
import { parseJm } from './parseJm';
import { parseJs } from './parseJs';
import type { PatternLibrary } from './types';

export async function loadPatternLibrary(): Promise<PatternLibrary> {
  const [jmResponse, jsResponse] = await Promise.all([
    fetch('/patterns.jm'),
    fetch('/moresites.txt'),
  ]);

  if (!jmResponse.ok) {
    throw new Error(
      `Failed to load patterns.jm: ${String(jmResponse.status)} ${jmResponse.statusText}`
    );
  }
  if (!jsResponse.ok) {
    throw new Error(
      `Failed to load moresites.txt: ${String(jsResponse.status)} ${jsResponse.statusText}`
    );
  }

  const [jmText, jsText] = await Promise.all([jmResponse.text(), jsResponse.text()]);

  const jmResult = parseJm(jmText);
  const jsResult = parseJs(jsText);

  return mergePatternLibraries(jmResult, jsResult);
}
