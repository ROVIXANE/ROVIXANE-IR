#!/bin/sh
# Compile-checks the firmware against the REAL IRremoteESP8266 v2.8.6 headers.
# Skips gracefully when there is no internet access.
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
LIB="$ROOT/tests/.irlib"
HDR="$ROOT/tests/.irheaders"

if [ ! -f "$LIB/src/IRsend.h" ]; then
  mkdir -p "$LIB"
  if ! curl -fsSL --max-time 60 -o "$LIB/lib.tar.gz" \
      https://codeload.github.com/crankyoldgit/IRremoteESP8266/tar.gz/refs/tags/v2.8.6; then
    echo "Real-IR header check skipped (no internet)."
    exit 0
  fi
  tar -xzf "$LIB/lib.tar.gz" -C "$LIB" --strip-components=1
fi

mkdir -p "$HDR"
cp "$ROOT/tests/host/"*.h "$HDR/"
rm -f "$HDR/IRremoteESP8266.h" "$HDR/IRrecv.h" "$HDR/IRsend.h"
printf '#include "Arduino.h"\n#include "%s/rovixane-ir.ino"\n' "$ROOT" > "$HDR/probe.cpp"

CXX=${CXX:-g++}
command -v "$CXX" >/dev/null 2>&1 || CXX=clang++
FLAGS=$(grep -v '^#' "$ROOT/build.opt" | tr '\n' ' ')
"$CXX" -std=gnu++17 -fsyntax-only -Wno-deprecated-declarations \
  -I "$HDR" -I "$LIB/src" -I "$ROOT" $FLAGS "$HDR/probe.cpp"
echo "Real IRremoteESP8266 v2.8.6 header check passed."
