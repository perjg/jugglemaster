import { useEffect, useState } from 'react';
import { IconZoomIn, IconZoomOut, IconZoomReset } from '@tabler/icons-react';
import { useI18nContext } from '../i18n/i18n-react';

interface ZoomControlsProps {
  module: typeof Module | null;
  isLoaded: boolean;
}

// Mirrors the engine's extraZoom clamp range in Zoom()
const ZOOM_STEP = 0.05;
const ZOOM_MIN = 0.01;
const ZOOM_MAX = 0.9;
const EPS = 0.005;

export const ZoomControls = ({ module, isLoaded }: ZoomControlsProps) => {
  const { LL } = useI18nContext();
  const [zoomLevel, setZoomLevel] = useState(0);

  const refresh = () => {
    if (module && isLoaded) setZoomLevel(module._jm_get_zoom());
  };

  // Zoom also changes outside these buttons (every pattern change re-frames)
  useEffect(() => {
    if (!module || !isLoaded) return;
    const read = () => { setZoomLevel(module._jm_get_zoom()); };
    const t0 = setTimeout(read, 0);
    const t = setInterval(read, 500);
    return () => { clearTimeout(t0); clearInterval(t); };
  }, [module, isLoaded]);

  const atMax = zoomLevel >= ZOOM_MAX - EPS;
  const atMin = zoomLevel <= ZOOM_MIN + EPS;

  const buttons = [
    {
      label: LL.zoom.zoomIn(),
      Icon: IconZoomIn,
      disabled: !isLoaded || atMax,
      onClick: () => { module?._jm_zoom(ZOOM_STEP); refresh(); },
    },
    {
      label: LL.zoom.zoomOut(),
      Icon: IconZoomOut,
      disabled: !isLoaded || atMin,
      onClick: () => { module?._jm_zoom(-ZOOM_STEP); refresh(); },
    },
    {
      label: LL.zoom.zoomReset(),
      Icon: IconZoomReset,
      disabled: !isLoaded,
      onClick: () => { module?._jm_reset_zoom(); refresh(); },
    },
  ];

  // Positioning is owned by the corner stack in Canvas
  return (
    <div
      style={{
        backgroundColor: 'rgba(0, 0, 0, 0.45)',
        borderRadius: '4px',
        display: 'flex',
        flexDirection: 'column',
        gap: '2px',
        padding: '3px',
        pointerEvents: 'auto',
      }}
      onPointerDown={e => { e.stopPropagation(); }}
    >
      {buttons.map(({ label, Icon, disabled, onClick }) => (
        <button
          key={label}
          aria-label={label}
          title={label}
          onClick={onClick}
          disabled={disabled}
          style={{
            background: 'transparent',
            border: 'none',
            color: 'white',
            opacity: disabled ? 0.35 : 1,
            padding: '4px',
            cursor: disabled ? 'default' : 'pointer',
            borderRadius: '3px',
            lineHeight: 0,
          }}
          onMouseEnter={(e) => {
            if (!disabled) e.currentTarget.style.backgroundColor = 'rgba(255,255,255,0.15)';
          }}
          onMouseLeave={(e) => {
            e.currentTarget.style.backgroundColor = 'transparent';
          }}
        >
          <Icon size={18} />
        </button>
      ))}
    </div>
  );
};
