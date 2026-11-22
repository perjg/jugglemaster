# JuggleMaster Visual Regression Tests

Automated visual tests for the JuggleMaster WebAssembly engine using Playwright.

## Prerequisites

- Node.js 18+
- jmwasm built (`cd ../src/jmwasm && make`)

## Setup

```bash
npm install
npx playwright install chromium
```

## Running Tests

```bash
npm test              # Run tests, compare to baselines
npm run test:update   # Update baseline screenshots
npm run test:report   # View test report
```

## Structure

```
tests/
├── package.json
├── playwright.config.ts
├── README.md
└── visual/
    ├── test_harness.html     # Minimal WASM loader
    ├── visual.spec.ts        # Test definitions
    └── snapshots/            # Baseline images
```
