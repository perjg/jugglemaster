interface SiteswapOverlayProps {
  site: string;
  start: number;
  stop: number;
  visible: boolean;
}

export const SiteswapOverlay = ({ site, start, stop, visible }: SiteswapOverlayProps) => {
  if (!visible || !site) return null;

  const before = site.slice(0, start);
  const active = site.slice(start, stop);
  const after = site.slice(stop);

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
        <span style={{ color: 'white' }}>{before}</span>
        <span style={{ color: '#ff4444' }}>{active}</span>
        <span style={{ color: 'white' }}>{after}</span>
      </div>
    </div>
  );
};
