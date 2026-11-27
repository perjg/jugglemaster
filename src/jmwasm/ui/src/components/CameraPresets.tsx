import { useEffect, useState } from 'react';
import { cn } from '../lib/utils';
import { useI18nContext } from '../i18n/i18n-react';

interface CameraPresetsProps {
  module: typeof Module | null;
  isLoaded: boolean;
  onAutoRotateChange: (v: boolean) => void;
}

const PRESETS = [
  { key: 'left',  index: 1 },
  { key: 'back',  index: 2 },
  { key: 'front', index: 0 },
  { key: 'high',  index: 4 },
  { key: 'right', index: 3 },
] as const;

// Straight down: only offered while several jugglers pass
const TOP = { key: 'top', index: 5 } as const;

const useJugglerCount = (module: typeof Module | null, isLoaded: boolean) => {
  const [count, setCount] = useState(1);
  useEffect(() => {
    if (!module || !isLoaded) return;
    let frame = 0;
    const poll = () => {
      const next = module._jm_get_juggler_count();
      setCount(prev => (prev === next ? prev : next));
      frame = requestAnimationFrame(poll);
    };
    frame = requestAnimationFrame(poll);
    return () => { cancelAnimationFrame(frame); };
  }, [module, isLoaded]);
  return count;
};

export const CameraPresets = ({ module, isLoaded, onAutoRotateChange }: CameraPresetsProps) => {
  const { LL } = useI18nContext();
  const presets = useJugglerCount(module, isLoaded) > 1 ? [...PRESETS, TOP] : PRESETS;
  const handlePreset = (index: number) => {
    if (!module || !isLoaded) return;
    module._jm_set_camera_preset(index);
    onAutoRotateChange(false);
  };

  return (
    <div
      className={cn('fixed sm:absolute left-0 right-0 flex justify-center')}
      style={{
        bottom: 'calc(10px + env(safe-area-inset-bottom, 0px))',
        pointerEvents: 'none',
      }}
    >
      <div
        style={{
          backgroundColor: 'rgba(0, 0, 0, 0.45)',
          borderRadius: '4px',
          display: 'flex',
          gap: '2px',
          padding: '3px',
          pointerEvents: 'auto',
        }}
        onPointerDown={(e) => { e.stopPropagation(); }}
      >
        {presets.map(({ key, index }) => (
          <button
            key={key}
            onClick={() => { handlePreset(index); }}
            disabled={!isLoaded}
            style={{
              background: 'transparent',
              border: 'none',
              color: 'white',
              fontFamily: 'monospace',
              fontSize: '12px',
              padding: '2px 8px',
              cursor: isLoaded ? 'pointer' : 'default',
              borderRadius: '3px',
            }}
            onMouseEnter={(e) => {
              (e.currentTarget as HTMLButtonElement).style.backgroundColor = 'rgba(255,255,255,0.15)';
            }}
            onMouseLeave={(e) => {
              (e.currentTarget as HTMLButtonElement).style.backgroundColor = 'transparent';
            }}
          >
            {LL.camera[key]()}
          </button>
        ))}
      </div>
    </div>
  );
};
