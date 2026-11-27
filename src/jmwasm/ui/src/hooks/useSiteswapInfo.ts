import { useState, useEffect, useRef } from 'react';

/** One line of the siteswap overlay: `site[start, stop)` is the throw being made. */
export interface SiteswapRow {
  site: string;
  start: number;
  stop: number;
  /** Body colour of the row's juggler (passing patterns written per juggler) */
  color?: string;
}

const EMPTY: SiteswapRow[] = [];

const toHexColor = (rgb: number) => `#${rgb.toString(16).padStart(6, '0')}`;

const readRows = (module: typeof Module): SiteswapRow[] => {
  const count = module._jm_get_site_rows();
  if (count <= 1) {
    return [{
      site: module.ccall('jm_get_site', 'string', [], []) as string,
      start: module._jm_get_sitepos_start(),
      stop: module._jm_get_sitepos_stop(),
    }];
  }
  return Array.from({ length: count }, (_, row) => ({
    site: module.ccall('jm_get_row_site', 'string', ['number'], [row]) as string,
    start: module._jm_get_row_sitepos_start(row),
    stop: module._jm_get_row_sitepos_stop(row),
    color: toHexColor(module._jm_get_juggler_color(row)),
  }));
};

const sameRows = (a: SiteswapRow[], b: SiteswapRow[]) =>
  a.length === b.length &&
  a.every((r, i) => r.site === b[i].site && r.start === b[i].start && r.stop === b[i].stop && r.color === b[i].color);

export const useSiteswapInfo = (
  module: typeof Module | null,
  isLoaded: boolean,
  enabled: boolean
): SiteswapRow[] => {
  const [rows, setRows] = useState<SiteswapRow[]>(EMPTY);
  const rafRef = useRef<number>(0);

  useEffect(() => {
    if (!module || !isLoaded || !enabled) return;

    let prev = EMPTY;
    const poll = () => {
      const next = readRows(module);
      // Only re-render when something actually changed
      if (!sameRows(prev, next)) {
        prev = next;
        setRows(next);
      }
      rafRef.current = requestAnimationFrame(poll);
    };

    rafRef.current = requestAnimationFrame(poll);

    return () => {
      cancelAnimationFrame(rafRef.current);
    };
  }, [module, isLoaded, enabled]);

  return rows;
};
