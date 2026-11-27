import { test, expect } from '@playwright/test';

// Helpers defined on the test harness page (test_harness.html)
declare function setPattern(site: string): void;
declare function setRenderMode(mode: number): void;
declare function setStyle(style: string): void;
declare function stepFrames(n: number): void;
declare global {
  interface Window { Module: { _jm_get_engine: () => number } }
}

// The harness pauses the loop and pins the clock to 60 fps, so N stepped
// frames always produce the same image
const WARMUP_FRAMES = 120;

test.describe('Visual Regression Tests', () => {
  test.beforeEach(async ({ page }) => {
    // Wait for WASM to initialize (harness starts paused)
    const ready = page.waitForEvent('console', msg =>
      msg.text().includes('Test harness ready')
    );
    await page.goto('http://localhost:8080/test_harness.html');
    await ready;
  });

  // engine: 0 = JuggleMaster, 1 = JuggleSaver (wrapper auto-selects per pattern)
  const testCases = [
    { name: 'jm-pattern-3', engine: 0, fn: () => setPattern('3') },
    { name: 'jm-pattern-531', engine: 0, fn: () => setPattern('531') },
    { name: 'jm-pattern-97531', engine: 0, fn: () => setPattern('97531') },
    { name: 'js-pattern-3b', engine: 1, fn: () => setPattern('3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)') },
    { name: 'js-pattern-7c', engine: 1, fn: () => setPattern('7c@(-2,0,-20)>(1.2,0,-5)7c@(2,0,20)>(-1.2,0,5)') },
    { name: 'js-pattern-3r', engine: 1, fn: () => setPattern('3r@(-2.5,3,-90,80)>(2,1,90,30) 3r@(2.5,3,90,80)>(-2,1,-90,30)') },
    { name: 'js-pattern-3b-style', engine: 1, fn: () => { setPattern('3b'); setStyle('JuggleSaver'); } },
    { name: 'js-passing-966', engine: 1, fn: () => setPattern('J2:9b6b6b') },
    { name: 'js-passing-7c', engine: 1, fn: () => setPattern('J2:7c') },
    { name: 'js-passing-sync', engine: 1, fn: () => setPattern('<3pc 3c 3c 3c|3pc 3c 3c 3c>') },
    { name: 'js-passing-feed', engine: 1, fn: () => setPattern('<3p2c 3p3c|3p1c 3c|3c 3p1c>') },
  ];

  for (const { name, engine, fn } of testCases) {
    // Test in 3D Mode (Default)
    test(`${name} (3D)`, async ({ page }) => {
      await page.evaluate(fn);
      expect(await page.evaluate(() => window.Module._jm_get_engine())).toBe(engine);
      await page.evaluate(n => { setRenderMode(2); stepFrames(n); }, WARMUP_FRAMES);
      await expect(page).toHaveScreenshot(`${name}-3d.png`);
    });

    // Test in 2D Mode
    test(`${name} (2D)`, async ({ page }) => {
      await page.evaluate(fn);
      expect(await page.evaluate(() => window.Module._jm_get_engine())).toBe(engine);
      await page.evaluate(n => { setRenderMode(1); stepFrames(n); }, WARMUP_FRAMES);
      await expect(page).toHaveScreenshot(`${name}-2d.png`);
    });
  }
});
