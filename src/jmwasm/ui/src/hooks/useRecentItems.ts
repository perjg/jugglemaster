import { useState } from 'react';

function sortByFavorite<T extends { favorite?: boolean }>(items: T[]): T[] {
  return [...items.filter(i => i.favorite), ...items.filter(i => !i.favorite)];
}

export function useRecentItems<T extends { favorite?: boolean }>(
  storageKey: string,
  isEqual: (a: T, b: T) => boolean,
  maxItems = 10,
) {
  const [items, setItems] = useState<T[]>(() => {
    try {
      const raw = localStorage.getItem(storageKey);
      return raw ? (JSON.parse(raw) as T[]) : [];
    } catch {
      return [];
    }
  });

  const persist = (next: T[]) => {
    try { localStorage.setItem(storageKey, JSON.stringify(next)); } catch { /* storage unavailable */ }
  };

  const push = (item: T) => {
    setItems(prev => {
      const deduped = prev.filter(p => !isEqual(p, item));
      const next = sortByFavorite([item, ...deduped]).slice(0, maxItems);
      persist(next);
      return next;
    });
  };

  const remove = (item: T) => {
    setItems(prev => {
      const next = prev.filter(p => !isEqual(p, item));
      persist(next);
      return next;
    });
  };

  const toggleFavorite = (item: T) => {
    setItems(prev => {
      const next = sortByFavorite(
        prev.map(p => isEqual(p, item) ? { ...p, favorite: !p.favorite } : p)
      );
      persist(next);
      return next;
    });
  };

  return { items, push, remove, toggleFavorite };
}
