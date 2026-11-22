import { useState, useEffect } from 'react';
import { IconMenu2, IconAlertTriangle, IconStar, IconStarFilled, IconX } from '@tabler/icons-react';
import { Button } from '@/components/ui/button';
import { Sheet, SheetContent, SheetHeader, SheetTitle } from '@/components/ui/sheet';
import { Tabs, TabsList, TabsTrigger, TabsContent } from '@/components/ui/tabs';
import { Input } from '@/components/ui/input';
import { Slider } from '@/components/ui/slider';
import { Switch } from '@/components/ui/switch';
import { cn } from '@/lib/utils';
import { useJMWasm } from '@/hooks/useJMWasm';
import { useRecentItems } from '@/hooks/useRecentItems';
import { SPEED_MIN, SPEED_MAX } from '@/hooks/useUrlParams';
import { RenderingMode, JugglingEngine, ObjectType } from '@/constants/jmlib';
import { useI18nContext } from '@/i18n/i18n-react';
import { PatternTabContent, type RecentPattern } from '@/components/PatternTabContent';
import { IconBallSegmented, IconRing, IconClub, IconFish } from '@/components/ObjectIcons';

const STYLE_OPTIONS = ['Normal', 'Reverse', 'Shower', 'Mills Mess', 'Center', 'Windmill'] as const;

interface RecentSiteswap {
  site: string;
  style: string;
  favorite?: boolean;
}

interface MenuProps {
  onOpenChange?: (open: boolean) => void;
  showSiteswap: boolean;
  onShowSiteswapChange: (v: boolean) => void;
  autoRotate: boolean;
  onAutoRotateChange: (v: boolean) => void;
  speed: number;
  onSpeedChange: (v: number) => void;
  onSpeedCommit: (v: number) => void;
  initialStyle?: string;
  initialIs3D?: boolean;
  initialPatternSite?: string;
  initialFromLibrary?: boolean;
  initialCategoryId?: string;
  initialObjectType?: number;
  onObjectTypeChange?: (type: number) => void;
  onPatternChange?: (name: string, site: string, style: string, fromLibrary: boolean, categoryId: string) => void;
  onStyleChange?: (style: string) => void;
  onModeChange?: (is3D: boolean) => void;
}

export const Menu = ({ onOpenChange, showSiteswap, onShowSiteswapChange, autoRotate, onAutoRotateChange, speed, onSpeedChange, onSpeedCommit, initialStyle, initialIs3D, initialPatternSite, initialFromLibrary, initialCategoryId, initialObjectType, onObjectTypeChange, onPatternChange, onStyleChange, onModeChange }: MenuProps) => {
  const { LL } = useI18nContext();
  const [isOpen, setIsOpen] = useState(false);
  const [isDesktop, setIsDesktop] = useState(false);

  useEffect(() => {
    const checkDesktop = () => {
      setIsDesktop(window.matchMedia('(min-width: 640px)').matches);
    };

    checkDesktop();
    window.addEventListener('resize', checkDesktop);
    return () => { window.removeEventListener('resize', checkDesktop); };
  }, []);

  const handleClose = () => {
    setIsOpen(false);
    onOpenChange?.(false);
  };

  const handleOpenChange = (open: boolean) => {
    if (!open && isDesktop) {
      return;
    }

    if (open) syncEngineFromModule();
    setIsOpen(open);
    onOpenChange?.(open);
  };
  const [siteswap, setSiteswap] = useState('');
  const [siteswapValid, setSiteswapValid] = useState<boolean | null>(null);
  const [validForJm, setValidForJm] = useState(false);
  const [validForJs, setValidForJs] = useState(false);
  const [is3D, setIs3D] = useState(initialIs3D ?? true);
  const [preferredEngine, setPreferredEngine] = useState<JugglingEngine>(
    initialStyle === 'JuggleSaver' ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER
  );
  const [actualEngine, setActualEngine] = useState<JugglingEngine>(JugglingEngine.JUGGLEMASTER);
  const [selectedStyle, setSelectedStyle] = useState<string>(
    initialStyle && initialStyle !== 'JuggleSaver' ? initialStyle : 'Normal'
  );
  const { module, isLoaded } = useJMWasm();
  const { items: recentSiteswaps, push: pushRecentSiteswap, remove: removeRecentSiteswap, toggleFavorite: toggleRecentSiteswapFavorite } = useRecentItems<RecentSiteswap>(
    'jm_recent_siteswaps',
    (a, b) => a.site === b.site,
  );
  const { items: recentPatterns, push: pushRecentPattern, remove: removeRecentPattern, toggleFavorite: toggleRecentPatternFavorite } = useRecentItems<RecentPattern>(
    'jm_recent_patterns',
    (a, b) => a.categoryId === b.categoryId && a.data === b.data,
  );
  const [activeTab, setActiveTab] = useState(initialFromLibrary ? 'pattern' : 'site');
  const [objectType, setObjectType] = useState<number>(initialObjectType ?? ObjectType.DEFAULT);

  const handleObjectTypeChange = (type: number) => {
    if (!module || !isLoaded) return;
    setObjectType(type);
    module._jm_set_object_type(type);
    onObjectTypeChange?.(type);
  };

  const syncEngineFromModule = () => {
    if (!module || !isLoaded) return;
    setActualEngine(module._jm_get_engine() === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER);
  };

  // JuggleSaver patterns embed their own object types, so selecting one goes
  // back to the pattern default; JuggleMaster patterns keep the user's choice
  const resetObjectTypeIfJuggleSaver = () => {
    if (!module || !isLoaded) return;
    if (module._jm_get_engine() !== 1) return;
    setObjectType(ObjectType.DEFAULT);
    module._jm_set_object_type(ObjectType.DEFAULT);
    onObjectTypeChange?.(ObjectType.DEFAULT);
  };

  const reportPatternChange = (name: string, site: string, style: string, fromLibrary: boolean, categoryId: string) => {
    syncEngineFromModule();
    resetObjectTypeIfJuggleSaver();
    onPatternChange?.(name, site, style, fromLibrary, categoryId);
  };

  useEffect(() => {
    if (!module || !isLoaded || !siteswap.trim()) {
      setTimeout(() => {
        setSiteswapValid(null);
        setValidForJm(false);
        setValidForJs(false);
      }, 0);
      return;
    }
    const trimmed = siteswap.trim();
    const valid = module.ccall('jm_is_valid_pattern', 'number', ['string'], [trimmed]) as number;
    const jm = module.ccall('jm_is_valid_jm_pattern', 'number', ['string'], [trimmed]) as number;
    const js = module.ccall('jm_is_valid_js_pattern', 'number', ['string'], [trimmed]) as number;
    setTimeout(() => {
      setSiteswapValid(valid === 1);
      setValidForJm(jm === 1);
      setValidForJs(js === 1);
    }, 0);
  }, [siteswap, module, isLoaded]);

  // Sync only the actual engine; preferredEngine is seeded from the URL style
  // and must not be clobbered by a read that may precede Canvas applying it
  useEffect(() => {
    if (module && isLoaded) {
      const currentEngine = module._jm_get_engine();
      const engineValue = currentEngine === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;
      setTimeout(() => {
        setActualEngine(engineValue);
      }, 0);
    }
  }, [module, isLoaded]);

  const handleSiteswapSubmit = (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    if (!module || !isLoaded || !siteswap.trim()) {
      return;
    }

    const trimmedSiteswap = siteswap.trim();

    // siteswapValid state can lag a keystroke behind; ask the engine directly
    const valid = module.ccall('jm_is_valid_pattern', 'number', ['string'], [trimmedSiteswap]) as number;
    if (valid !== 1) {
      return;
    }

    module.ccall('jm_set_pattern', null, ['string', 'string'], [trimmedSiteswap, trimmedSiteswap]);

    setTimeout(() => {
      let activeEngine = module._jm_get_engine();
      let engineValue = activeEngine === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;

      if (engineValue !== preferredEngine) {
        if (preferredEngine === JugglingEngine.JUGGLESAVER) {
          module.ccall('jm_set_style', null, ['string'], ['JuggleSaver']);
        } else {
          module.ccall('jm_set_style', null, ['string'], [selectedStyle]);
        }

        setTimeout(() => {
          activeEngine = module._jm_get_engine();
          engineValue = activeEngine === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;
          setActualEngine(engineValue);
          if (engineValue === preferredEngine) {
            setPreferredEngine(engineValue);
          }
          resetObjectTypeIfJuggleSaver();
        }, 0);
      } else {
        setActualEngine(engineValue);
        setPreferredEngine(engineValue);
      }
    }, 0);

    const styleToReport = preferredEngine === JugglingEngine.JUGGLESAVER ? 'JuggleSaver' : selectedStyle;
    reportPatternChange(trimmedSiteswap, trimmedSiteswap, styleToReport, false, '');
    pushRecentSiteswap({ site: trimmedSiteswap, style: styleToReport });
    setSiteswap('');
    if (!isDesktop) handleClose();
  };

  const handleRecentSiteswap = (item: RecentSiteswap) => {
    if (!module || !isLoaded) return;
    module.ccall('jm_set_pattern', null, ['string', 'string'], [item.site, item.site]);
    module.ccall('jm_set_style', null, ['string'], [item.style]);
    const engine = module._jm_get_engine() === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;
    setActualEngine(engine);
    if (item.style === 'JuggleSaver') {
      setPreferredEngine(JugglingEngine.JUGGLESAVER);
    } else {
      setSelectedStyle(item.style);
      setPreferredEngine(JugglingEngine.JUGGLEMASTER);
    }
    reportPatternChange(item.site, item.site, item.style, false, '');
    pushRecentSiteswap(item);
    if (!isDesktop) handleClose();
  };

  const handleRecentPattern = (item: RecentPattern) => {
    if (!module || !isLoaded) return;
    module.ccall('jm_set_pattern', null, ['string', 'string'], [item.name, item.data]);
    module.ccall('jm_set_style', null, ['string'], [item.style]);
    reportPatternChange(item.name, item.data, item.style, true, item.categoryId);
    pushRecentPattern(item);
    if (!isDesktop) handleClose();
  };

  const handleModeToggle = (checked: boolean) => {
    if (!module || !isLoaded) {
      return;
    }

    setIs3D(checked);
    module._jm_set_render_mode(
      checked ? RenderingMode.OPENGL_3D : RenderingMode.OPENGL_2D
    );
    onModeChange?.(checked);
  };

  const handleAutoRotateToggle = (checked: boolean) => {
    if (!module || !isLoaded) {
      return;
    }
    onAutoRotateChange(checked);
    module._jm_set_autorotate(checked ? 1 : 0);
  };

  const handleSpeedChange = (value: number | readonly number[]) => {
    if (!module || !isLoaded) return;
    const v = typeof value === 'number' ? value : value[0];
    module._jm_set_speed(v);
    onSpeedChange(v);
  };

  const handleSpeedCommit = (value: number | readonly number[]) => {
    const v = typeof value === 'number' ? value : value[0];
    onSpeedCommit(v);
  };

  const handleEngineToggle = (checked: boolean) => {
    if (!module || !isLoaded) {
      return;
    }

    const targetEngine = checked ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;

    const styleToApply = targetEngine === JugglingEngine.JUGGLESAVER ? 'JuggleSaver' : selectedStyle;
    module.ccall('jm_set_style', null, ['string'], [styleToApply]);
    onStyleChange?.(styleToApply);

    setTimeout(() => {
      const activeEngine = module._jm_get_engine();
      const engineValue = activeEngine === 1 ? JugglingEngine.JUGGLESAVER : JugglingEngine.JUGGLEMASTER;
      setActualEngine(engineValue);

      if (engineValue === targetEngine) {
        setPreferredEngine(targetEngine);
      } else {
        setPreferredEngine(targetEngine);
      }
    }, 0);
  };

  return (
    <>
      <Button
        variant="outline"
        size="icon"
        className={cn(
          'fixed z-50 left-[10px] bottom-[10px] sm:bottom-auto sm:top-[10px]'
        )}
        aria-label={LL.menu.menu()}
        onClick={() => { handleOpenChange(true); }}
      >
        <IconMenu2 className="h-5 w-5" />
      </Button>

      <Sheet
        open={isOpen}
        onOpenChange={handleOpenChange}
        modal={false}
      >
        <SheetContent
          side="left"
          hideOverlayOnDesktop={true}
          onClose={handleClose}
          className={cn(
            'data-[side=left]:w-full data-[side=left]:max-w-none h-full sm:data-[side=left]:w-80 sm:data-[side=left]:max-w-none'
          )}
        >
          <SheetHeader>
            <SheetTitle>{LL.title()}</SheetTitle>
          </SheetHeader>

          <div className="mt-6 space-y-6 px-4">
            <Tabs value={activeTab} onValueChange={setActiveTab} className="w-full">
              <TabsList variant="line" className="w-full">
                <TabsTrigger value="site">{LL.menu.tabSite()}</TabsTrigger>
                <TabsTrigger value="pattern">{LL.menu.tabPattern()}</TabsTrigger>
              </TabsList>
              <TabsContent value="site" className="space-y-4 mt-4">
                <div className="space-y-2">
                  <div className="flex items-center justify-between w-full gap-2">
                    <label htmlFor="siteswap" className="text-sm font-medium shrink-0">
                      {LL.menu.enterSiteswap()}
                    </label>
                    {siteswapValid === true && (
                      <div className="flex items-center gap-1.5 shrink-0">
                        {validForJm && (
                          <span
                            className="inline-flex items-center rounded px-1.5 py-0.5 text-[10px] font-medium bg-muted text-muted-foreground"
                            title={LL.menu.engineJuggleMaster()}
                          >
                            JM
                          </span>
                        )}
                        {validForJs && (
                          <span
                            className="inline-flex items-center rounded px-1.5 py-0.5 text-[10px] font-medium bg-muted text-muted-foreground"
                            title={LL.menu.engineJuggleSaver()}
                          >
                            JS
                          </span>
                        )}
                      </div>
                    )}
                  </div>
                  <form onSubmit={handleSiteswapSubmit}>
                    <Input
                      id="siteswap"
                      type="text"
                      placeholder={LL.menu.siteswapPlaceholder()}
                      value={siteswap}
                      onChange={(e) => { setSiteswap(e.target.value); }}
                      disabled={!isLoaded}
                      className={cn(
                        siteswapValid === true && 'bg-green-100 dark:bg-green-950/30 border-green-500 dark:border-green-600',
                        siteswapValid === false && 'bg-red-100 dark:bg-red-950/30 border-red-500 dark:border-red-600'
                      )}
                    />
                  </form>
                  <div className="space-y-1.5">
                    <label htmlFor="style" className="text-sm font-medium">
                      {LL.menu.style()}
                    </label>
                    <select
                      id="style"
                      value={selectedStyle}
                      onChange={(e) => {
                        const v = e.target.value;
                        setSelectedStyle(v);
                        if (module && isLoaded) {
                          module.ccall('jm_set_style', null, ['string'], [v]);
                        }
                        onStyleChange?.(v);
                      }}
                      disabled={!isLoaded || actualEngine === JugglingEngine.JUGGLESAVER}
                      className="flex h-9 w-full rounded-md border border-input bg-transparent px-3 py-2 text-[16px] sm:text-sm shadow-sm focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:cursor-not-allowed disabled:opacity-50"
                    >
                      {STYLE_OPTIONS.map((s) => (
                        <option key={s} value={s}>
                          {s}
                        </option>
                      ))}
                    </select>
                  </div>
                </div>
                <div className="flex items-center justify-between">
                  <div className="space-y-0.5 flex-1">
                    <div className="flex items-center gap-2">
                      <label htmlFor="engine" className="text-sm font-medium">
                        {LL.menu.engine()}
                      </label>
                      {preferredEngine !== actualEngine && (
                        <IconAlertTriangle className="h-4 w-4 text-yellow-500" />
                      )}
                    </div>
                    <p className="text-xs text-muted-foreground">
                      {preferredEngine === JugglingEngine.JUGGLESAVER ? LL.menu.engineJuggleSaver() : LL.menu.engineJuggleMaster()}
                    </p>
                  </div>
                  <Switch
                    id="engine"
                    checked={preferredEngine === JugglingEngine.JUGGLESAVER}
                    onCheckedChange={(checked) => {
                      handleEngineToggle(checked);
                    }}
                    disabled={!isLoaded}
                  />
                </div>

              </TabsContent>
              <TabsContent value="pattern" className="mt-4">
                <PatternTabContent
                  onPatternChange={(...args) => { reportPatternChange(...args); if (!isDesktop) handleClose(); }}
                  onPushRecent={pushRecentPattern}
                  initialSite={initialPatternSite}
                  initialFromLibrary={initialFromLibrary}
                  initialCategoryId={initialCategoryId}
                />
              </TabsContent>
            </Tabs>

            <div className="flex items-center justify-between">
              <div className="space-y-0.5">
                <label htmlFor="mode" className="text-sm font-medium">
                  {LL.menu.mode()}
                </label>
                <p className="text-xs text-muted-foreground">
                  {is3D ? LL.menu.mode3D() : LL.menu.mode2D()}
                </p>
              </div>
              <Switch
                id="mode"
                checked={is3D}
                onCheckedChange={(checked) => {
                  handleModeToggle(checked);
                }}
                disabled={!isLoaded}
              />
            </div>

            <div className="flex items-center justify-between">
              <div className="space-y-0.5">
                <label htmlFor="autorotate" className="text-sm font-medium">
                  {LL.menu.autoRotate()}
                </label>
                <p className="text-xs text-muted-foreground">
                  {autoRotate ? LL.menu.on() : LL.menu.off()}
                </p>
              </div>
              <Switch
                id="autorotate"
                checked={autoRotate}
                onCheckedChange={(checked) => {
                  handleAutoRotateToggle(checked);
                }}
                disabled={!isLoaded}
              />
            </div>

            <div className="flex items-center justify-between">
              <div className="space-y-0.5">
                <label htmlFor="showsiteswap" className="text-sm font-medium">
                  {LL.menu.showSiteswap()}
                </label>
                <p className="text-xs text-muted-foreground">
                  {showSiteswap ? LL.menu.on() : LL.menu.off()}
                </p>
              </div>
              <Switch
                id="showsiteswap"
                checked={showSiteswap}
                onCheckedChange={onShowSiteswapChange}
                disabled={!isLoaded}
              />
            </div>

            <div className="space-y-1.5">
              <div className="flex items-center justify-between">
                <label htmlFor="speed" className="text-sm font-medium">
                  {LL.menu.speed()}
                </label>
                <p className="text-xs text-muted-foreground tabular-nums">
                  {speed.toFixed(1)}
                </p>
              </div>
              <Slider
                id="speed"
                min={SPEED_MIN}
                max={SPEED_MAX}
                step={0.1}
                value={[speed]}
                onValueChange={handleSpeedChange}
                onValueCommitted={handleSpeedCommit}
                disabled={!isLoaded}
                aria-label={LL.menu.speed()}
              />
            </div>

            <div className="space-y-1.5">
              <label className="text-sm font-medium">
                {LL.menu.objects()}
              </label>
              <div className="flex gap-1">
                {[
                  ...(actualEngine === JugglingEngine.JUGGLESAVER
                    ? [{ value: ObjectType.DEFAULT as number, label: LL.menu.objectDefault(), Icon: undefined }]
                    : []),
                  { value: ObjectType.BALL as number, label: LL.menu.objectBalls(), Icon: IconBallSegmented },
                  { value: ObjectType.CLUB as number, label: LL.menu.objectClubs(), Icon: IconClub },
                  { value: ObjectType.RING as number, label: LL.menu.objectRings(), Icon: IconRing },
                  { value: ObjectType.FISH as number, label: LL.menu.objectFish(), Icon: IconFish },
                ].map(({ value, label, Icon }) => {
                  // JuggleMaster has no pattern default; DEFAULT renders as balls
                  const selected = actualEngine === JugglingEngine.JUGGLEMASTER && objectType === ObjectType.DEFAULT
                    ? value === ObjectType.BALL
                    : objectType === value;
                  return (
                    <Button
                      key={value}
                      size="sm"
                      variant={selected ? 'default' : 'outline'}
                      className="h-12 min-w-0 flex-1 px-1 text-xs"
                      disabled={!isLoaded}
                      aria-label={label}
                      onClick={() => { handleObjectTypeChange(value); }}
                    >
                      {Icon !== undefined ? <Icon className="size-9" /> : label}
                    </Button>
                  );
                })}
              </div>
            </div>

            {activeTab === 'site' && recentSiteswaps.length > 0 && (
              <div className="space-y-1">
                <hr className="border-border" />
                <p className="text-xs font-medium text-muted-foreground">Recent</p>
                <div className="space-y-0.5">
                  {recentSiteswaps.map((item, i) => (
                    <div key={i} className="flex items-center gap-0.5">
                      <button
                        onClick={() => { handleRecentSiteswap(item); }}
                        disabled={!isLoaded}
                        className="flex-1 min-w-0 text-left px-2 py-1 rounded text-sm hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed flex items-center gap-2"
                      >
                        <span className="flex-1 truncate font-mono">{item.site}</span>
                        <span className="text-xs text-muted-foreground shrink-0">{item.style}</span>
                      </button>
                      <button
                        onClick={() => { toggleRecentSiteswapFavorite(item); }}
                        disabled={!isLoaded}
                        className="shrink-0 p-1 rounded hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed text-muted-foreground"
                        aria-label={item.favorite ? 'Unfavorite' : 'Favorite'}
                      >
                        {item.favorite
                          ? <IconStarFilled className="h-3.5 w-3.5 text-yellow-500" />
                          : <IconStar className="h-3.5 w-3.5" />
                        }
                      </button>
                      <button
                        onClick={() => { removeRecentSiteswap(item); }}
                        disabled={!isLoaded}
                        className="shrink-0 p-1 rounded hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed text-muted-foreground"
                        aria-label="Remove"
                      >
                        <IconX className="h-3.5 w-3.5" />
                      </button>
                    </div>
                  ))}
                </div>
              </div>
            )}

            {activeTab === 'pattern' && recentPatterns.length > 0 && (
              <div className="space-y-1">
                <hr className="border-border" />
                <p className="text-xs font-medium text-muted-foreground">Recent</p>
                <div className="space-y-0.5">
                  {recentPatterns.map((item, i) => (
                    <div key={i} className="flex items-center gap-0.5">
                      <button
                        onClick={() => { handleRecentPattern(item); }}
                        disabled={!isLoaded}
                        className="flex-1 min-w-0 text-left px-2 py-1 rounded text-sm hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed"
                      >
                        <div className="truncate">{item.name}{item.name !== item.data ? ` (${item.data})` : ''}</div>
                        <div className="text-xs text-muted-foreground truncate">{item.categoryName}</div>
                      </button>
                      <button
                        onClick={() => { toggleRecentPatternFavorite(item); }}
                        disabled={!isLoaded}
                        className="shrink-0 p-1 rounded hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed text-muted-foreground"
                        aria-label={item.favorite ? 'Unfavorite' : 'Favorite'}
                      >
                        {item.favorite
                          ? <IconStarFilled className="h-3.5 w-3.5 text-yellow-500" />
                          : <IconStar className="h-3.5 w-3.5" />
                        }
                      </button>
                      <button
                        onClick={() => { removeRecentPattern(item); }}
                        disabled={!isLoaded}
                        className="shrink-0 p-1 rounded hover:bg-accent disabled:opacity-50 disabled:cursor-not-allowed text-muted-foreground"
                        aria-label="Remove"
                      >
                        <IconX className="h-3.5 w-3.5" />
                      </button>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>
        </SheetContent>
      </Sheet>
    </>
  );
};
