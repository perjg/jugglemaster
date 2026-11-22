import { useState, useEffect, useRef } from 'react';

interface SiteswapInfo {
  site: string;
  start: number;
  stop: number;
}

const EMPTY: SiteswapInfo = { site: '', start: 0, stop: 0 };

export const useSiteswapInfo = (
  module: typeof Module | null,
  isLoaded: boolean,
  enabled: boolean
): SiteswapInfo => {
  const [info, setInfo] = useState<SiteswapInfo>(EMPTY);
  const rafRef = useRef<number>(0);

  useEffect(() => {
    if (!module || !isLoaded || !enabled) return;

    let prev = EMPTY;
    const poll = () => {
      const site = module.ccall('jm_get_site', 'string', [], []) as string;
      const start = module._jm_get_sitepos_start();
      const stop = module._jm_get_sitepos_stop();
      // Only re-render when something actually changed
      if (site !== prev.site || start !== prev.start || stop !== prev.stop) {
        prev = { site, start, stop };
        setInfo(prev);
      }
      rafRef.current = requestAnimationFrame(poll);
    };

    rafRef.current = requestAnimationFrame(poll);

    return () => {
      cancelAnimationFrame(rafRef.current);
    };
  }, [module, isLoaded, enabled]);

  return info;
};
