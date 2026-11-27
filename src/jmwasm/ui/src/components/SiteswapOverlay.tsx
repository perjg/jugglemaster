import type { SiteswapRow } from '../hooks/useSiteswapInfo';

interface SiteswapOverlayProps {
  rows: SiteswapRow[];
  visible: boolean;
}

export const SiteswapOverlay = ({ rows, visible }: SiteswapOverlayProps) => {
  if (!visible || rows.length === 0 || !rows[0].site) return null;

  return (
    <div
      style={{
        position: 'absolute',
        top: 0,
        left: 0,
        right: 0,
        pointerEvents: 'none',
        display: 'flex',
        justifyContent: 'center',
        paddingTop: '12px',
      }}
    >
      <div
        style={{
          backgroundColor: 'rgba(0, 0, 0, 0.45)',
          padding: '4px 12px',
          borderRadius: '4px',
          fontFamily: 'monospace',
          fontSize: '16px',
          lineHeight: 1.4,
        }}
      >
        {rows.map(({ site, start, stop, color }, i) => (
          <div key={i}>
            {color && <span style={{ color, fontWeight: 'bold' }}>{String.fromCharCode(65 + i)} </span>}
            <span style={{ color: 'white' }}>{site.slice(0, start)}</span>
            <span style={{ color: '#ff4444' }}>{site.slice(start, stop)}</span>
            <span style={{ color: 'white' }}>{site.slice(stop)}</span>
          </div>
        ))}
      </div>
    </div>
  );
};
