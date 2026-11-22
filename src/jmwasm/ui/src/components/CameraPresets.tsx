import { cn } from '../lib/utils';

interface CameraPresetsProps {
  module: typeof Module | null;
  isLoaded: boolean;
  onAutoRotateChange: (v: boolean) => void;
}

const PRESETS = [
  { label: 'Left',  index: 1 },
  { label: 'Back',  index: 2 },
  { label: 'Front', index: 0 },
  { label: 'High',  index: 4 },
  { label: 'Right', index: 3 },
] as const;

export const CameraPresets = ({ module, isLoaded, onAutoRotateChange }: CameraPresetsProps) => {
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
        {PRESETS.map(({ label, index }) => (
          <button
            key={label}
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
            {label}
          </button>
        ))}
      </div>
    </div>
  );
};
