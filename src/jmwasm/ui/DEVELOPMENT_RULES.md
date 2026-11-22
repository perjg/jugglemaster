# Development Rules

## Build and Lint
- **Always run `npm run lint` after making code changes**
- **Always run `npm run build` to verify the build succeeds**
- Fix all linter errors before considering changes complete
- Never commit code with linter errors

## Internationalization (i18n)
- **All user-facing strings MUST be internationalized**
- Use `typesafe-i18n` for all text that appears in the UI
- Add translations to both `src/i18n/en/index.ts` and `src/i18n/no/index.ts`
- **After adding or modifying translation strings, run `npm run typesafe-i18n` to regenerate TypeScript types**
- Use the `useI18nContext` hook to access translations: `const { LL } = useI18nContext()`
- Never hardcode strings like "Settings", "Enter Siteswap", "Mode", "On", "Off", etc.
- Even aria-labels and placeholders should be internationalized when possible

## shadcn/ui Components
- **Always ask the user before adding new shadcn/ui components**
- Do not add new components without explicit approval
- If a component is needed, explain why and wait for confirmation before running `npx shadcn@latest add`

## Code Quality
- Follow TypeScript strict mode rules
- Avoid `any` types - use proper types or `unknown`
- Avoid non-null assertions (`!`) - use proper null checks
- Remove unnecessary comments
- Keep code clean and maintainable
- If multiple attempts are required to successfully implement a feature or fix a bug, be mindful of code generated that may not be required and clean up as needed
