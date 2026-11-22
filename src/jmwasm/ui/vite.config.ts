import { defineConfig, type Plugin } from 'vite';
import react from '@vitejs/plugin-react';
import tailwindcss from '@tailwindcss/vite';
import path from 'path';
import { createHash } from 'crypto';
import { readFileSync } from 'fs';

// jmwasm.js/jmwasm.wasm are served unhashed from the site root; a stale cached
// glue paired with a fresh wasm produces LinkErrors (seen on iOS Safari's
// heuristic cache). Version both requests by the wasm content hash.
const wasmBuildId = (): string => {
  try {
    return createHash('md5')
      .update(readFileSync(path.resolve(__dirname, 'public/jmwasm.wasm')))
      .digest('hex')
      .slice(0, 8);
  } catch {
    return Date.now().toString(36);
  }
};

// Plugin to force full page reload for WASM-related files
const forceFullReloadPlugin = (): Plugin => {
  return {
    name: 'force-full-reload-wasm',
    handleHotUpdate({ file, server }) {
      // Files that need full page reload due to WASM module state
      const wasmRelatedFiles = [
        /src\/hooks\/useJMWasm\.ts$/,
        /src\/components\/Canvas\.tsx$/,
      ];

      if (wasmRelatedFiles.some((pattern) => pattern.test(file))) {
        server.ws.send({
          type: 'full-reload',
        });
        return [];
      }
    },
  };
};

// https://vite.dev/config/
export default defineConfig({
  define: {
    __WASM_BUILD_ID__: JSON.stringify(wasmBuildId()),
  },
  plugins: [
    react({
      babel: {
        plugins: ['babel-plugin-react-compiler'],
      },
    }),
    tailwindcss(),
    forceFullReloadPlugin(),
  ],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
    },
  },

  // Base path for static hosting
  // Set to '/' for root hosting, or '/jmwasm/' for subdirectory hosting
  base: '/',

  // Build configuration for Apache static hosting
  build: {
    outDir: 'dist',
    assetsDir: 'assets',
    // Ensure relative paths for assets work correctly
    rollupOptions: {
      output: {
        // Predictable asset file names
        assetFileNames: 'assets/[name]-[hash].[ext]',
        chunkFileNames: 'assets/[name]-[hash].js',
        entryFileNames: 'assets/[name]-[hash].js',
      },
    },
  },

  // Public directory for static assets (WASM files, pattern files, etc.)
  // Files in this directory are copied as-is to dist/
  publicDir: 'public',
});
