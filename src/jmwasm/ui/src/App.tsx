import { useState } from 'react';
import { Canvas } from './components/Canvas';
import { Menu } from './components/Menu';
import { useUrlParams, urlParamsToCanvas, objectTypeToParam } from '@/hooks/useUrlParams';
import { cn } from './lib/utils';
import './App.css';

function App() {
  const { initial, setParams } = useUrlParams();

  const canvasInit = urlParamsToCanvas(initial);

  const [isMenuOpen, setIsMenuOpen] = useState(false);
  const [showSiteswap, setShowSiteswap] = useState(initial.showSiteswap);
  const [autoRotate, setAutoRotate] = useState(initial.autoRotate);
  const [speed, setSpeed] = useState(initial.speed);
  const [colorJugglers, setColorJugglers] = useState(initial.colorJugglers);
  const [feedWidth, setFeedWidth] = useState(initial.feedWidth);

  const handleShowSiteswapChange = (v: boolean) => {
    setShowSiteswap(v);
    setParams({ showSiteswap: v });
  };

  const handleAutoRotateChange = (v: boolean) => {
    setAutoRotate(v);
    setParams({ autoRotate: v });
  };

  const handleColorJugglersChange = (v: boolean) => {
    setColorJugglers(v);
    setParams({ colorJugglers: v });
  };

  const handleFeedWidthChange = (v: number) => {
    setFeedWidth(v);
    setParams({ feedWidth: v });
  };

  // URL writes only on commit: browsers throttle history.replaceState, and a
  // slider drag fires a change per step
  const handleSpeedChange = (v: number) => {
    setSpeed(v);
  };

  const handleSpeedCommit = (v: number) => {
    setSpeed(v);
    setParams({ speed: v });
  };

  // Everything else writes the URL on change; speed commits on slider release,
  // so flush it before a share link is built
  const handleBeforeShare = () => {
    setParams({ speed });
  };

  return (
    <div className="app">
      <Menu
        onOpenChange={setIsMenuOpen}
        showSiteswap={showSiteswap}
        onShowSiteswapChange={handleShowSiteswapChange}
        autoRotate={autoRotate}
        onAutoRotateChange={handleAutoRotateChange}
        colorJugglers={colorJugglers}
        onColorJugglersChange={handleColorJugglersChange}
        feedWidth={feedWidth}
        onFeedWidthChange={handleFeedWidthChange}
        speed={speed}
        onSpeedChange={handleSpeedChange}
        onSpeedCommit={handleSpeedCommit}
        initialStyle={initial.style}
        initialIs3D={initial.mode === '3d'}
        initialPatternSite={initial.pattern}
        initialFromLibrary={initial.fromLibrary}
        initialCategoryId={initial.categoryId}
        initialObjectType={canvasInit.initialObjectType}
        onObjectTypeChange={(t) => { setParams({ object: objectTypeToParam(t) }); }}
        onPatternChange={(name, site, style, fromLibrary, categoryId) => { setParams({ name, pattern: site, style, fromLibrary, categoryId }); }}
        onStyleChange={(style) => { setParams({ style }); }}
        onModeChange={(is3d) => { setParams({ mode: is3d ? '3d' : '2d' }); }}
      />
      <div
        className={cn(
          'flex-1 h-full transition-all duration-200 ease-in-out',
          isMenuOpen && 'sm:ml-80'
        )}
      >
        <Canvas
          showSiteswap={showSiteswap}
          onAutoRotateChange={handleAutoRotateChange}
          initialPattern={canvasInit.initialPattern}
          initialStyle={canvasInit.initialStyle}
          initialMode={canvasInit.initialMode}
          initialAutoRotate={initial.autoRotate}
          initialObjectType={canvasInit.initialObjectType}
          initialSpeed={canvasInit.initialSpeed}
          initialColorJugglers={canvasInit.initialColorJugglers}
          initialFeedWidth={canvasInit.initialFeedWidth}
          onBeforeShare={handleBeforeShare}
        />
      </div>
    </div>
  );
}

export default App;
