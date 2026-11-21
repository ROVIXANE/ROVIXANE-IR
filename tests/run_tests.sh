#!/bin/sh
# Runs every check on the host (no ESP8266 or Arduino IDE needed).
set -e
cd "$(dirname "$0")/.."

python3 tests/check_assets.py

CXX=${CXX:-g++}
command -v "$CXX" >/dev/null 2>&1 || CXX=clang++
"$CXX" -std=gnu++17 -Wall -Wno-deprecated-declarations -Wno-unused-variable \
  -I tests/host -I . \
  -o tests/.test_firmware \
  tests/host/test_firmware.cpp AppConfig.cpp Storage.cpp SignalStore.cpp

./tests/.test_firmware
sh tests/check_real_ir_headers.sh
