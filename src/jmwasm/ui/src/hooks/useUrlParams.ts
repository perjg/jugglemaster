import { useCallback, useMemo } from 'react';
import { RenderingMode } from '@/constants/jmlib';

export type ObjectParam = '' | 'ball' | 'club' | 'ring' | 'fish';

export interface AppUrlParams {
  pattern: string;
  name: string;
  style: string;
  mode: '2d' | '3d';
  showSiteswap: boolean;
  autoRotate: boolean;
  fromLibrary: boolean;
  categoryId: string;
  object: ObjectParam;
  speed: number;
}

// Engine clamps to the same range (JMLib::setSpeed)
export const SPEED_MIN = 0.1;
export const SPEED_MAX = 10;
export const SPEED_DEFAULT = 3;

const OBJECT_PARAM_TO_TYPE: Record<Exclude<ObjectParam, ''>, number> = { ball: 1, club: 2, ring: 3, fish: 6 };

export function objectParamToType(p: ObjectParam): number {
  return p === '' ? 0 : OBJECT_PARAM_TO_TYPE[p];
}

export function objectTypeToParam(t: number): ObjectParam {
  return t === 1 ? 'ball' : t === 2 ? 'club' : t === 3 ? 'ring' : t === 6 ? 'fish' : '';
}

const DEFAULTS: AppUrlParams = {
  pattern: '3',
  name: '3 Cascade',
  style: 'Normal',
  mode: '3d',
  showSiteswap: false,
  autoRotate: false,
  fromLibrary: false,
  categoryId: '',
  object: '',
  speed: SPEED_DEFAULT,
};

function readSpeed(sp: URLSearchParams): number {
  const v = Number(sp.get('speed'));
  return Number.isFinite(v) && v >= SPEED_MIN && v <= SPEED_MAX ? v : SPEED_DEFAULT;
}

function readParams(): AppUrlParams {
  const sp = new URLSearchParams(window.location.search);
  return {
    pattern: sp.get('pattern') ?? DEFAULTS.pattern,
    name: sp.get('name') ?? sp.get('pattern') ?? DEFAULTS.name,
    style: sp.get('style') ?? DEFAULTS.style,
    mode: sp.get('mode') === '2d' ? '2d' : '3d',
    showSiteswap: sp.get('siteswap') === '1',
    autoRotate: sp.get('autorotate') === '1',
    fromLibrary: sp.get('lib') === '1',
    categoryId: sp.get('cat') ?? '',
    object: (['ball', 'club', 'ring', 'fish'] as const).find(v => v === sp.get('object')) ?? '',
    speed: readSpeed(sp),
  };
}

function writeParams(params: AppUrlParams): void {
  const sp = new URLSearchParams();
  if (params.pattern !== DEFAULTS.pattern) sp.set('pattern', params.pattern);
  if (params.name !== params.pattern && params.name !== DEFAULTS.name) sp.set('name', params.name);
  if (params.style !== DEFAULTS.style) sp.set('style', params.style);
  if (params.mode !== DEFAULTS.mode) sp.set('mode', params.mode);
  if (params.showSiteswap) sp.set('siteswap', '1');
  if (params.autoRotate) sp.set('autorotate', '1');
  if (params.fromLibrary) sp.set('lib', '1');
  if (params.categoryId) sp.set('cat', params.categoryId);
  if (params.object) sp.set('object', params.object);
  if (params.speed !== DEFAULTS.speed) sp.set('speed', String(params.speed));
  const search = sp.toString();
  window.history.replaceState(null, '', search ? `${window.location.pathname}?${search}` : window.location.pathname);
}

export function urlParamsToCanvas(params: AppUrlParams) {
  return {
    initialPattern: { name: params.name, site: params.pattern },
    initialStyle: params.style,
    initialMode: params.mode === '2d' ? RenderingMode.OPENGL_2D : RenderingMode.OPENGL_3D,
    initialObjectType: objectParamToType(params.object),
    initialSpeed: params.speed,
  };
}

export function useUrlParams() {
  const initial = useMemo(() => readParams(), []);

  const setParams = useCallback((patch: Partial<AppUrlParams>) => {
    writeParams({ ...readParams(), ...patch });
  }, []);

  return { initial, setParams };
}
