import { useRef, useEffect } from 'react';

export const useOrbitCamera = (
  module: typeof Module | null,
  isLoaded: boolean,
  onAutoRotateChange: (v: boolean) => void
) => {
  const containerRef = useRef<HTMLDivElement | null>(null);
  const isDragging = useRef(false);
  const lastX = useRef(0);
  const lastY = useRef(0);
  const hasDisabledAutoRotate = useRef(false);
  const onAutoRotateChangeRef = useRef(onAutoRotateChange);
  useEffect(() => {
    onAutoRotateChangeRef.current = onAutoRotateChange;
  }, [onAutoRotateChange]);

  useEffect(() => {
    const el = containerRef.current;
    if (!el || !module || !isLoaded) return;

    const onPointerDown = (e: PointerEvent) => {
      if ((e.target as Element).closest('button')) return;
      el.setPointerCapture(e.pointerId);
      isDragging.current = true;
      lastX.current = e.clientX;
      lastY.current = e.clientY;
    };

    const onPointerMove = (e: PointerEvent) => {
      if (!isDragging.current) return;
      e.preventDefault();
      const rect = el.getBoundingClientRect();
      const dx = (e.clientX - lastX.current) / rect.width;
      const dy = (e.clientY - lastY.current) / rect.height;
      if (!hasDisabledAutoRotate.current) {
        module._jm_set_autorotate(0);
        onAutoRotateChangeRef.current(false);
        hasDisabledAutoRotate.current = true;
      }
      module._jm_orbit_camera(dx, dy);
      lastX.current = e.clientX;
      lastY.current = e.clientY;
    };

    const onPointerUp = (e: PointerEvent) => {
      el.releasePointerCapture(e.pointerId);
      isDragging.current = false;
      hasDisabledAutoRotate.current = false;
    };

    const onDblClick = (e: MouseEvent) => {
      if ((e.target as Element).closest('button')) return;
      const next = module._jm_get_autorotate() ? 0 : 1;
      module._jm_set_autorotate(next);
      onAutoRotateChangeRef.current(next === 1);
    };

    el.addEventListener('pointerdown', onPointerDown);
    el.addEventListener('pointermove', onPointerMove, { passive: false });
    el.addEventListener('pointerup', onPointerUp);
    el.addEventListener('pointercancel', onPointerUp);
    el.addEventListener('dblclick', onDblClick);

    return () => {
      el.removeEventListener('pointerdown', onPointerDown);
      el.removeEventListener('pointermove', onPointerMove);
      el.removeEventListener('pointerup', onPointerUp);
      el.removeEventListener('pointercancel', onPointerUp);
      el.removeEventListener('dblclick', onDblClick);
    };
  }, [module, isLoaded]);

  return containerRef;
};
