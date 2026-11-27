#!/usr/bin/env python3
"""Adds or updates en/nb entries in JugglerKit's string catalog: add-strings.py key=en|nb ..."""
import json
import sys

PATH = 'JugglerKit/Resources/Localizable.xcstrings'
catalog = json.load(open(PATH))
for arg in sys.argv[1:]:
    key, values = arg.split('=', 1)
    en, nb = values.split('|', 1)
    catalog['strings'][key] = {'localizations': {
        'en': {'stringUnit': {'state': 'translated', 'value': en}},
        'nb': {'stringUnit': {'state': 'translated', 'value': nb}},
    }}
catalog['strings'] = dict(sorted(catalog['strings'].items()))
with open(PATH, 'w') as f:
    json.dump(catalog, f, indent=2, ensure_ascii=False)
    f.write('\n')
