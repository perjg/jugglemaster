import { useState, useMemo, useCallback } from 'react';
import { useI18nContext } from '@/i18n/i18n-react';
import { usePatternLibrary } from '@/hooks/usePatternLibrary';
import { useJMWasm } from '@/hooks/useJMWasm';
import {
  Combobox,
  ComboboxInput,
  ComboboxContent,
  ComboboxEmpty,
  ComboboxItem,
  ComboboxList,
} from '@/components/ui/combobox';
import { Input } from '@/components/ui/input';
import { Button } from '@/components/ui/button';
import { filterPatterns } from '@/lib/patterns';
import type { Category, Pattern, SourceFilter } from '@/lib/patterns';

const MAX_SEARCH_RESULTS = 50;

export interface RecentPattern {
  name: string;
  data: string;
  style: string;
  categoryId: string;
  categoryName: string;
  favorite?: boolean;
}

interface PatternTabContentProps {
  onPatternChange?: (name: string, data: string, style: string, fromLibrary: boolean, categoryId: string) => void;
  onPushRecent?: (item: RecentPattern) => void;
  initialSite?: string;
  initialFromLibrary?: boolean;
  initialCategoryId?: string;
}

export function PatternTabContent({ onPatternChange, onPushRecent, initialSite, initialFromLibrary, initialCategoryId }: PatternTabContentProps) {
  const { LL } = useI18nContext();
  const { module, isLoaded } = useJMWasm();
  const { library, loading, error } = usePatternLibrary();

  // URL's initial pattern resolved against the library; preselects the comboboxes
  const initialSelection = useMemo(() => {
    if (!library || !initialFromLibrary || !initialCategoryId || !initialSite) return null;
    const pattern = library.patterns.find(p => p.categoryId === initialCategoryId && p.data === initialSite);
    if (!pattern) return null;
    const category = library.categories.find(c => c.id === pattern.categoryId);
    if (!category) return null;
    return { category, pattern };
  }, [library, initialFromLibrary, initialCategoryId, initialSite]);

  // null = user hasn't touched the comboboxes; fall back to initialSelection
  const [userSelection, setUserSelection] = useState<{ category: Category | null; pattern: Pattern | null } | null>(null);
  const selectedCategory = userSelection ? userSelection.category : initialSelection?.category ?? null;
  const selectedPattern = userSelection ? userSelection.pattern : initialSelection?.pattern ?? null;

  const [searchText, setSearchText] = useState('');
  const [sourceFilter, setSourceFilter] = useState<SourceFilter>('all');
  const searching = searchText.trim() !== '';

  const categories = useMemo(() => {
    if (!library) return [];
    return library.categories.filter(c => sourceFilter === 'all' || c.source === sourceFilter);
  }, [library, sourceFilter]);

  // Free-text search runs across the whole library (not just the selected category)
  const searchResults = useMemo(() => {
    if (!library || !searching) return [];
    return filterPatterns(library.patterns, searchText, sourceFilter);
  }, [library, searching, searchText, sourceFilter]);

  const patternsInCategory = useMemo(() => {
    if (!library || selectedCategory === null) return [];
    return library.patterns.filter((p) => p.categoryId === selectedCategory.id);
  }, [library, selectedCategory]);

  const applyPattern = useCallback((pattern: Pattern, addToRecent = true) => {
    if (!module || !isLoaded) return;
    const isJuggleSaver = pattern.source === 'js' || pattern.categoryName.includes('JuggleSaver');
    const styleName = isJuggleSaver ? 'JuggleSaver' : pattern.style;
    // Pattern first: setPattern re-selects the engine per pattern, so the
    // JuggleSaver style switch must come after it
    module.ccall('jm_set_pattern', null, ['string', 'string'], [pattern.name, pattern.data]);
    module.ccall('jm_set_style', null, ['string'], [styleName]);
    onPatternChange?.(pattern.name, pattern.data, styleName, true, pattern.categoryId);
    if (addToRecent) {
      onPushRecent?.({ name: pattern.name, data: pattern.data, style: styleName, categoryId: pattern.categoryId, categoryName: pattern.categoryName });
    }
  }, [module, isLoaded, onPatternChange, onPushRecent]);

  // No initial engine-apply here: Canvas applies the URL pattern at startup;
  // initialSelection only preselects the comboboxes.

  const handleCategoryChange = (value: Category | null) => {
    setUserSelection({ category: value, pattern: null });
  };

  const handlePatternChange = (value: Pattern | null) => {
    setUserSelection({ category: selectedCategory, pattern: value });
    if (value !== null) {
      applyPattern(value);
    }
  };

  const handleSearchResult = (pattern: Pattern) => {
    // Keep the browse comboboxes in sync so clearing the search shows the pick
    const category = library?.categories.find(c => c.id === pattern.categoryId) ?? null;
    setUserSelection({ category, pattern });
    applyPattern(pattern);
  };

  /** Workaround for aria-hidden bug: when combobox closes, focus is inside the Sheet;
   * Base UI may try to set aria-hidden on the Sheet. Blur first so no descendant has focus,
   * then refocus on next frame after cleanup runs. */
  const handleComboboxOpenChange = (open: boolean) => {
    if (open) return;
    const active = document.activeElement as HTMLElement | null;
    if (active?.blur) {
      active.blur();
      requestAnimationFrame(() => {
        active.focus();
      });
    }
  };

  if (loading) {
    return (
      <p className="text-sm text-muted-foreground">
        {LL.menu.patternsLoading()}
      </p>
    );
  }

  if (error) {
    return (
      <p className="text-sm text-destructive">
        {LL.menu.patternsLoadError()}
      </p>
    );
  }

  return (
    <div className="space-y-4">
      <div className="flex items-center gap-2">
        <Input
          type="search"
          value={searchText}
          onChange={(e) => { setSearchText(e.target.value); }}
          placeholder={LL.menu.searchPlaceholder()}
          aria-label={LL.menu.searchPlaceholder()}
          className="text-[16px] sm:text-xs"
          disabled={!isLoaded}
        />
        <div className="flex gap-1">
          {([
            { value: 'all', label: LL.menu.filterAll() },
            { value: 'jm', label: LL.menu.filterJm() },
            { value: 'js', label: LL.menu.filterJs() },
          ] as const).map(({ value, label }) => (
            <Button
              key={value}
              size="sm"
              variant={sourceFilter === value ? 'default' : 'outline'}
              className="px-2 text-xs"
              onClick={() => { setSourceFilter(value); }}
            >
              {label}
            </Button>
          ))}
        </div>
      </div>

      {searching && (
        <div className="space-y-1">
          {searchResults.length === 0 && (
            <p className="text-sm text-muted-foreground">{LL.menu.searchNoResults()}</p>
          )}
          <div className="max-h-72 space-y-0.5 overflow-y-auto">
            {searchResults.slice(0, MAX_SEARCH_RESULTS).map((p) => (
              <button
                key={p.id}
                onClick={() => { handleSearchResult(p); }}
                className="block w-full rounded-sm px-2 py-1.5 text-left text-sm hover:bg-muted"
              >
                <span className="font-medium">{p.name}</span>
                {p.data !== '' && <span className="text-muted-foreground"> ({p.data})</span>}
                <span className="block truncate text-xs text-muted-foreground">{p.categoryName}</span>
              </button>
            ))}
          </div>
          {searchResults.length > MAX_SEARCH_RESULTS && (
            <p className="text-xs text-muted-foreground">
              {LL.menu.searchTruncated({ shown: MAX_SEARCH_RESULTS, total: searchResults.length })}
            </p>
          )}
        </div>
      )}

      {!searching && (
      <>
      <div className="space-y-1.5">
        <label className="text-sm font-medium">
          {LL.menu.category()}
        </label>
        <Combobox
          items={categories}
          value={selectedCategory}
          onValueChange={(value) => {
            handleCategoryChange(value);
          }}
          onOpenChange={handleComboboxOpenChange}
          itemToStringLabel={(cat) => cat.name}
          isItemEqualToValue={(a, b) => a.id === b.id}
          modal={false}
        >
          <ComboboxInput
            placeholder={LL.menu.patternsSelectCategory()}
            disabled={!isLoaded || categories.length === 0}
          />
          <ComboboxContent>
            <ComboboxEmpty>
              {LL.menu.patternsNoCategory()}
            </ComboboxEmpty>
            <ComboboxList>
              {(item: Category) => (
                <ComboboxItem key={item.id} value={item}>
                  {item.name}
                </ComboboxItem>
              )}
            </ComboboxList>
          </ComboboxContent>
        </Combobox>
      </div>

      <div className="space-y-1.5">
        <label className="text-sm font-medium">
          {LL.menu.pattern()}
        </label>
        <Combobox
          items={patternsInCategory}
          value={selectedPattern}
          onValueChange={(value) => {
            handlePatternChange(value);
          }}
          onOpenChange={handleComboboxOpenChange}
          itemToStringLabel={(p) => p.name}
          isItemEqualToValue={(a, b) => a.id === b.id}
          modal={false}
        >
          <ComboboxInput
            placeholder={LL.menu.patternsSelectPattern()}
            disabled={!isLoaded || selectedCategory === null || patternsInCategory.length === 0}
          />
          <ComboboxContent>
            <ComboboxEmpty>
              {LL.menu.patternsNoPatterns()}
            </ComboboxEmpty>
            <ComboboxList>
              {(item: Pattern) => (
                <ComboboxItem key={item.id} value={item}>
                  {item.name}
                  {item.data ? ` (${item.data})` : ''}
                </ComboboxItem>
              )}
            </ComboboxList>
          </ComboboxContent>
        </Combobox>
      </div>

      {selectedCategory === null && categories.length > 0 && (
        <p className="text-sm text-muted-foreground">
          {LL.menu.patternsNoCategory()}
        </p>
      )}
      </>
      )}
    </div>
  );
}
