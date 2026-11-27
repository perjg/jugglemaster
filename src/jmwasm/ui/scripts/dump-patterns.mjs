// Writes the pattern library as parsed by src/lib/patterns, plus sample search
// results, to JSON: the parity fixture for the Swift port (src/jmapple/JMKit).
// Usage: node scripts/dump-patterns.mjs <out.json>
import { build } from 'esbuild';
import { readFileSync, writeFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const data = resolve(root, '../../../data');
const out = process.argv[2];
if (!out) {
  console.error('usage: node scripts/dump-patterns.mjs <out.json>');
  process.exit(1);
}

const bundle = await build({
  entryPoints: [resolve(root, 'src/lib/patterns/index.ts')],
  bundle: true,
  format: 'esm',
  platform: 'node',
  write: false,
});
const lib = await import(`data:text/javascript;base64,${Buffer.from(bundle.outputFiles[0].text).toString('base64')}`);

const library = lib.mergePatternLibraries(
  lib.parseJm(readFileSync(resolve(data, 'patterns.jm'), 'utf8')),
  lib.parseJs(readFileSync(resolve(data, 'moresites.txt'), 'utf8')),
);
const queries = ['', 'cascade', 'CASCADE', '531', 'mills', '(4', 'x', 'shower', 'zzz-no-match'];
const filters = queries.flatMap(query => ['all', 'jm', 'js'].map(source => ({
  query,
  source,
  ids: lib.filterPatterns(library.patterns, query, source).map(p => p.id),
})));
writeFileSync(out, JSON.stringify({ library, filters }));
console.log(`${String(library.categories.length)} categories, ${String(library.patterns.length)} patterns`);
