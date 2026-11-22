import { useEffect, useState } from 'react';
import { loadPatternLibrary } from '@/lib/patterns';
import type { PatternLibrary } from '@/lib/patterns';

interface UsePatternLibraryReturn {
  library: PatternLibrary | null;
  loading: boolean;
  error: Error | null;
}

export function usePatternLibrary(): UsePatternLibraryReturn {
  const [library, setLibrary] = useState<PatternLibrary | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    let cancelled = false;

    loadPatternLibrary()
      .then((result) => {
        if (cancelled) return;
        setTimeout(() => {
          setLibrary(result);
          setLoading(false);
          setError(null);
        }, 0);
      })
      .catch((err: unknown) => {
        if (cancelled) return;
        const normalized = err instanceof Error ? err : new Error(String(err));
        setTimeout(() => {
          setError(normalized);
          setLoading(false);
        }, 0);
      });

    return () => {
      cancelled = true;
    };
  }, []);

  return { library, loading, error };
}
