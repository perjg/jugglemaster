#!/bin/sh
# Condenses xcodebuild output to diagnostics from this repo, test results and the final status.
grep -E '^/[^ ]+:[0-9]+:[0-9]+: (error|warning)|^(Test|Suite) .*(passed|failed|recorded an issue)|✘|^\*\* .* \*\*|^Testing failed|error: ' \
  | grep -v -E '/jmlib/.*warning:|appintentsmetadataprocessor' \
  | awk '!seen[$0]++'
