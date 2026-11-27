import { useEffect, useRef, useState } from 'react';
import { useJMWasm } from '../hooks/useJMWasm';
import { useSiteswapInfo } from '../hooks/useSiteswapInfo';
import { useOrbitCamera } from '../hooks/useOrbitCamera';
import { SiteswapOverlay } from './SiteswapOverlay';
import { CameraPresets } from './CameraPresets';
import { ZoomControls } from './ZoomControls';
import { ShareButton } from './ShareButton';
import { RenderingMode } from '../constants/jmlib';
import { useI18nContext } from '../i18n/i18n-react';

interface CanvasProps {
  showSiteswap: boolean;
  onAutoRotateChange: (v: boolean) => void;
  initialPattern: { name: string; site: string };
  initialStyle: string;
  initialMode: RenderingMode;
  initialAutoRotate: boolean;
  initialObjectType: number;
  initialSpeed: number;
  initialColorJugglers: boolean;
  initialFeedWidth: number;
  onBeforeShare?: () => void;
}

export const Canvas = ({ showSiteswap, onAutoRotateChange, initialPattern, initialStyle, initialMode, initialAutoRotate, initialObjectType, initialSpeed, initialColorJugglers, initialFeedWidth, onBeforeShare }: CanvasProps) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const { module, isLoaded, error, init } = useJMWasm();
  const { LL } = useI18nContext();
  const siteswapRows = useSiteswapInfo(module, isLoaded, showSiteswap);
  const orbitRef = useOrbitCamera(module, isLoaded, onAutoRotateChange);
  const [initFailed, setInitFailed] = useState(false);

  useEffect(() => {
    if (isLoaded && module && canvasRef.current) {
      const canvas = canvasRef.current;
      if (!init(canvas.width, canvas.height)) {
        // WebGL context creation failed inside the engine — a silent black
        // canvas otherwise
        const t = setTimeout(() => { setInitFailed(true); }, 0);
        return () => { clearTimeout(t); };
      }

      module._jm_set_object_type(initialObjectType);
      module._jm_set_feed_width(initialFeedWidth);
      module.ccall('jm_set_pattern', null, ['string', 'string'], [initialPattern.name, initialPattern.site]);
      module.ccall('jm_set_style', null, ['string'], [initialStyle]);
      module._jm_set_render_mode(initialMode);
      module._jm_set_autorotate(initialAutoRotate ? 1 : 0);
      module._jm_set_speed(initialSpeed);
      module._jm_set_juggler_colors(initialColorJugglers ? 1 : 0);
    }
  }, [isLoaded, module, init, initialPattern.name, initialPattern.site, initialStyle, initialMode, initialAutoRotate, initialObjectType, initialSpeed, initialColorJugglers, initialFeedWidth]);

  useEffect(() => {
    if (!canvasRef.current || !module || !isLoaded) {
      return;
    }

    const canvas = canvasRef.current;

    const handleResize = () => {
      const rect = canvas.getBoundingClientRect();
      canvas.width = rect.width;
      canvas.height = rect.height;
      module._jm_resize(canvas.width, canvas.height);
    };

    window.addEventListener('resize', handleResize);

    const resizeObserver = new ResizeObserver(() => {
      handleResize();
    });

    resizeObserver.observe(canvas);
    handleResize();

    return () => {
      window.removeEventListener('resize', handleResize);
      resizeObserver.disconnect();
    };
  }, [module, isLoaded]);

  const errorText = error !== null
    ? LL.error.wasmLoadFailed({ message: error.message })
    : initFailed
      ? LL.error.initFailed()
      : null;

  if (errorText !== null) {
    return (
      <div className="flex h-full flex-col items-center justify-center gap-3 p-6 text-center">
        <p className="max-w-md text-sm text-destructive">{errorText}</p>
        <p className="max-w-md break-all font-mono text-[10px] text-muted-foreground">
          {navigator.userAgent}
        </p>
      </div>
    );
  }

  if (!isLoaded) {
    return (
      <div className="loading-container">
        <div className="spinner"></div>
      </div>
    );
  }

  return (
    <div
      ref={orbitRef}
      style={{ position: 'relative', width: '100%', height: '100%', touchAction: 'none' }}
    >
      <canvas
        id="canvas"
        ref={canvasRef}
        width={800}
        height={600}
        style={{ display: 'block', width: '100%', height: '100%' }}
      />
      <SiteswapOverlay rows={siteswapRows} visible={showSiteswap} />
      <CameraPresets module={module} isLoaded={isLoaded} onAutoRotateChange={onAutoRotateChange} />
      <div
        className="fixed sm:absolute right-[10px] bottom-[10px] sm:bottom-auto sm:top-[10px] flex flex-col items-end gap-2"
        style={{ pointerEvents: 'none' }}
      >
        <ZoomControls module={module} isLoaded={isLoaded} />
        <ShareButton onBeforeShare={onBeforeShare} />
      </div>
    </div>
  );
};
