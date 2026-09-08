#!/usr/bin/env bash
set -euo pipefail

root="${1:-.}"
fail=0

while IFS= read -r -d '' file; do
  case "$file" in
    "$root/archive/legacy/"*) continue ;;
  esac

  if grep -Eqi 'Raspberry Pi|Raspberry Pi OS|ESP32-32E' "$file"; then
    echo "ERROR: obsolete architecture reference: $file"
    fail=1
  fi

done < <(find "$root" -type f \( -name '*.md' -o -name '*.csv' -o -name '*.ino' -o -name '*.cpp' -o -name '*.h' -o -name '*.c' -o -name '*.txt' \) -print0)

if grep -RIl --exclude-dir=.git --exclude-dir=archive --exclude='check_rev_a_consistency.sh' -E 'ILI9488.*ST7796|ST7796.*ILI9488' "$root" >/dev/null 2>&1; then
  echo "ERROR: interchangeable ILI9488/ST7796 display definition found in current-facing files"
  fail=1
fi

required=(
  'ESP32-C5-N8'
  'STM32WB55CGU6'
  'FPC'
  'CC1101'
  'nRF24L01+'
  'PN532'
  'microSD'
)

for term in "${required[@]}"; do
  if ! grep -RIl --exclude-dir=.git --exclude-dir=archive --exclude='check_rev_a_consistency.sh' -- "$term" "$root" >/dev/null 2>&1; then
    echo "ERROR: required Rev A term missing from current-facing repository: $term"
    fail=1
  fi
done

if [[ "$fail" -ne 0 ]]; then
  exit 1
fi

echo "PASS: Rev A architecture consistency checks passed"
