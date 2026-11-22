import { useEffect, useState, useRef, useCallback } from 'react';
import { useI18nContext } from '../i18n/i18n-react';

const LOAD_TIMEOUT_MS = 20000;

interface UseJMWasmReturn {
  module: typeof Module | null;
  isLoaded: boolean;
  error: Error | null;
  init: (width: number, height: number) => boolean;
}

// Global flag to prevent multiple script loads
declare global {
  interface Window {
    __JMWASM_LOADING__?: boolean;
    __JMWASM_LOADED__?: boolean;
    __JMWASM_ERROR__?: string;
    Module?: typeof Module;
  }
}

export const useJMWasm = (): UseJMWasmReturn => {
  const { LL } = useI18nContext();
  const [module, setModule] = useState<typeof Module | null>(null);
  const [isLoaded, setIsLoaded] = useState(false);
  const [error, setError] = useState<Error | null>(null);
  const scriptRef = useRef<HTMLScriptElement | null>(null);
  const llRef = useRef(LL);
  useEffect(() => { llRef.current = LL; }, [LL]);

  useEffect(() => {
    // A silent failure here used to mean an infinite spinner (seen on iOS,
    // where Lockdown Mode disables WebAssembly outright) — fail fast when
    // WebAssembly is missing and put a deadline on every waiting path.
    // (WebGL availability is not probed here: jm_init's real context-creation
    // attempt reports failure visibly, without burning a throwaway context.)
    if (!window.__JMWASM_LOADED__ && typeof WebAssembly === 'undefined') {
      const t = setTimeout(() => { setError(new Error(llRef.current.error.wasmUnavailable())); }, 0);
      return () => { clearTimeout(t); };
    }

    const deadline = setTimeout(() => {
      if (!window.__JMWASM_LOADED__) {
        window.__JMWASM_LOADING__ = false;
        setError(new Error(llRef.current.error.loadTimeout()));
      }
    }, LOAD_TIMEOUT_MS);
    const clearDeadline = () => { clearTimeout(deadline); };

    // Check if Module is already loaded and initialized
    if (window.__JMWASM_LOADED__ && window.Module) {
      const currentModule = window.Module;
      // Use setTimeout to avoid setState in effect
      setTimeout(() => {
        setIsLoaded(true);
        setModule(currentModule);
      }, 0);
      return clearDeadline;
    }

    // Check if script is already being loaded
    if (window.__JMWASM_LOADING__) {
      // Wait for the existing load to complete
      const checkInterval = setInterval(() => {
        if (window.__JMWASM_ERROR__ !== undefined) {
          clearInterval(checkInterval);
          setError(new Error(window.__JMWASM_ERROR__));
          return;
        }
        if (window.__JMWASM_LOADED__ && window.Module) {
          clearInterval(checkInterval);
          setIsLoaded(true);
          setModule(window.Module);
        }
      }, 50);

      return () => {
        clearInterval(checkInterval);
        clearDeadline();
      };
    }

    // Check if script tag already exists
    const existingScript = document.querySelector('script[src^="/jmwasm.js"]');
    if (existingScript) {
      // Script already exists, wait for Module to be ready
      window.__JMWASM_LOADING__ = true;
      const checkInterval = setInterval(() => {
        if (window.__JMWASM_ERROR__ !== undefined) {
          clearInterval(checkInterval);
          setError(new Error(window.__JMWASM_ERROR__));
          return;
        }
        const currentModule = window.Module;
        if (currentModule) {
          clearInterval(checkInterval);
          window.__JMWASM_LOADING__ = false;

          // Set up callback if not already set
          if (!currentModule.onRuntimeInitialized) {
            currentModule.onRuntimeInitialized = () => {
              window.__JMWASM_LOADED__ = true;
              setIsLoaded(true);
              if (window.Module) {
                setModule(window.Module);
              }
            };
          } else {
            // Wrap existing callback
            const originalCallback = currentModule.onRuntimeInitialized;
            currentModule.onRuntimeInitialized = () => {
              originalCallback();
              window.__JMWASM_LOADED__ = true;
              setIsLoaded(true);
              if (window.Module) {
                setModule(window.Module);
              }
            };
          }
        }
      }, 50);

      return () => {
        clearInterval(checkInterval);
        clearDeadline();
      };
    }

    // Load WASM module
    const loadModule = () => {
      try {
        window.__JMWASM_LOADING__ = true;

        // Pre-create the Module config BEFORE the script runs: locateFile
        // version-locks the .wasm to this glue (a stale cached pair produces
        // LinkErrors), and setting the callbacks up front removes the
        // script.onload race against a fast runtime init.
        window.Module = {
          locateFile: (file: string) => `/${file}?v=${__WASM_BUILD_ID__}`,
          onAbort: (reason: unknown) => {
            window.__JMWASM_LOADING__ = false;
            window.__JMWASM_ERROR__ = llRef.current.error.engineAborted({ reason: String(reason) });
            setError(new Error(window.__JMWASM_ERROR__));
          },
          onRuntimeInitialized: () => {
            window.__JMWASM_LOADING__ = false;
            window.__JMWASM_LOADED__ = true;
            setIsLoaded(true);
            if (window.Module) {
              setModule(window.Module);
            }
          },
        } as unknown as typeof Module;

        const script = document.createElement('script');
        script.src = `/jmwasm.js?v=${__WASM_BUILD_ID__}`;
        script.async = true;

        script.onload = () => {
          if (!window.Module) {
            window.__JMWASM_LOADING__ = false;
            window.__JMWASM_ERROR__ = 'Module not found after script load';
            setError(new Error(window.__JMWASM_ERROR__));
          }
        };

        script.onerror = () => {
          window.__JMWASM_LOADING__ = false;
          window.__JMWASM_ERROR__ = 'Failed to load jmwasm.js';
          setError(new Error(window.__JMWASM_ERROR__));
        };

        document.head.appendChild(script);
        scriptRef.current = script;
      } catch (err) {
        window.__JMWASM_LOADING__ = false;
        setError(err as Error);
      }
    };

    loadModule();

    // Cleanup - don't remove the script tag as it may be needed by other instances
    return () => {
      clearDeadline();
      // Only remove if we're the last one and it hasn't loaded yet
      if (scriptRef.current?.parentNode && !window.__JMWASM_LOADED__) {
        // Check if there are other script tags with the same src
        const scripts = document.querySelectorAll('script[src="/jmwasm.js"]');
        if (scripts.length === 1 && scripts[0] === scriptRef.current) {
          document.head.removeChild(scriptRef.current);
          window.__JMWASM_LOADING__ = false;
        }
      }
    };
  }, []);

  // Stable identity: Canvas lists init in its mount-effect deps, so a fresh
  // reference each render would re-run engine initialization
  const init = useCallback((width: number, height: number) => {
    if (module && isLoaded) {
      if (!module._jm_init(width, height)) {
        console.error('jm_init failed: WebGL 2.0 context could not be created');
        return false;
      }
      // Important: render once after init.
      // This neutralizes the wrapper's first-frame switch-back behavior
      module._jm_render();
      return true;
    }
    return false;
  }, [module, isLoaded]);

  return { module, isLoaded, error, init };
};
