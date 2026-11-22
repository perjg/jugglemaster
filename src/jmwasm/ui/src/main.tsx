import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import './index.css';
import App from './App.tsx';
import TypesafeI18n from './i18n/i18n-react';
import { loadAllLocales } from './i18n/i18n-util.sync';
import { baseLocale } from './i18n/i18n-util';

// Load all locales synchronously
loadAllLocales();

const rootElement = document.getElementById('root');
if (!rootElement) {
  throw new Error('Root element not found');
}

createRoot(rootElement).render(
  <StrictMode>
    <TypesafeI18n locale={baseLocale}>
      <App />
    </TypesafeI18n>
  </StrictMode>,
);
