import { defineConfig } from '@playwright/test';

export default defineConfig({
  testDir: './visual',
  snapshotDir: './visual/snapshots',
  // Platform-free names: headless SwiftShader renders near-identically on all OSes
  snapshotPathTemplate: '{snapshotDir}/{testFileName}-snapshots/{arg}{ext}',
  timeout: 30000,
  expect: {
    toHaveScreenshot: {
      // Output is byte-identical on one machine; headroom is for cross-platform
      // rasterization noise only. A ball is ~500 px — keep the budget below that.
      maxDiffPixels: 100,
    },
  },
  use: {
    browserName: 'chromium',
    headless: true,
    viewport: { width: 800, height: 600 },
  },
  webServer: {
    command: 'python3 -m http.server 8080 -d visual',
    port: 8080,
    reuseExistingServer: !process.env.CI,
  },
  reporter: 'html',
});
