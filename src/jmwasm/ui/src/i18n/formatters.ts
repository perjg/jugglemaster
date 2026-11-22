import type { FormattersInitializer } from 'typesafe-i18n';
import type { Locales, Formatters } from './i18n-types.js';

export const initFormatters: FormattersInitializer<Locales, Formatters> = (locale: Locales) => {
	console.log(`Initializing formatters for locale: ${locale}`);

	const formatters: Formatters = {
		// add your formatter functions here
	};

	return formatters;
};
