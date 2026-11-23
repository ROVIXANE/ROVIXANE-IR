# ROVIXANE-IR-v2.0.0 — Universal Infrared Remote

ROVIXANE IR v2.0.0 is a complete V2 of the ESP8266 universal IR remote: a mobile-first dashboard with TV, AC, fan, set-top-box and custom remotes, per-button learning, favorites, crash-safe storage, a first-run security setup, and phone-browser OTA updates.

> The previously published v1.0.0 release and its legacy `ihr-universal-ir-remote-*` assets are intentionally left unchanged. v2.0.0 assets follow the `rovixane-ir-nodemcuv2-v2.0.0.*` naming.

## What this release contains

- NodeMCU ESP8266 firmware: `rovixane-ir-nodemcuv2-v2.0.0.bin`
- SHA-256 checksum: `rovixane-ir-nodemcuv2-v2.0.0.sha256.txt`
- Source, tests and documentation in this repository

The firmware is compiled by GitHub Actions with Arduino CLI 1.6.0, ESP8266 core 3.1.2, ArduinoJson 7.4.2 and IRremoteESP8266 2.8.6. The BIN is checked for non-zero size and a valid ESP8266 image header before publishing.

## Highlights

- **Universal remote UI:** home, search, favorites, recents and quick actions; TV/STB layouts with navigation, volume, channel, numbers and media keys.
- **AC and fan remotes:** honest state UI (temperature/mode/fan) that sends your learned/mapped signals; no fabricated protocol encoding.
- **Custom builder:** create a remote with its own name, icon, colour, grid width and button layout.
- **Learn per button:** capture from the original remote and save protocol/HEX/bits or raw timings directly to a button. Invalid or overflowed captures are rejected visibly.
- **First-run setup:** a fresh device is in setup mode until you create an admin password. Until then, writes, IR transmit and OTA are blocked.
- **Security hardening:** HTTP Basic auth, `X-ROVIXANE-CSRF: 1`, JSON content type on mutations, masked secrets, no passwords in backups.
- **Crash-safe storage:** atomic writes, `.bak` recovery and a shared 20 KB limit. A corrupt or oversized database is never silently erased.
- **Streaming OTA:** upload a `.bin` from the phone; magic/size checks and safe abort. The device restarts only after a verified finalize.
- **English/Bangla UI** with dark/light theme and an embedded PWA manifest.

## Supported board

| Item | Required value |
|---|---|
| Board | NodeMCU 1.0 (ESP-12E) |
| Board ID | `esp8266:esp8266:nodemcuv2` |
| Flash | 4 MB total, 1 MB file system (`4M1M`) |
| CPU | 80 MHz |
| Upload baud | 115200 |

Other ESP8266 boards may work electrically, but this release is built and verified specifically for NodeMCU 1.0 / ESP-12E. Do not flash this BIN to a non-ESP8266 board.

## What you need

- NodeMCU ESP-12E
- TSOP38238 IR receiver
- TSAL620 IR LED
- BC547 NPN transistor
- 1kΩ resistor for the transistor base
- 47–220Ω resistor in series with the IR LED (recommended)
- USB cable and a phone with Wi-Fi and a web browser

See [WIRING.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/WIRING.md) for wiring. Component pinouts and resistor values are documented, not hardware-verified.

## Install from Android / Termux

1. Download `rovixane-ir-nodemcuv2-v2.0.0.bin` from the **Assets** section.
2. Optionally verify the download:

   ```sh
   sha256sum -c rovixane-ir-nodemcuv2-v2.0.0.sha256.txt
   ```

3. Install esptool in Termux:

   ```sh
   pkg install python
   pip install esptool
   ```

4. Connect the NodeMCU with a USB OTG cable and find the serial port:

   ```sh
   ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
   ```

5. Flash the firmware (adjust the port if needed):

   ```sh
   python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
     write_flash --flash_size 4MB1M 0x00000 \
     rovixane-ir-nodemcuv2-v2.0.0.bin
   ```

6. Reconnect power. Your phone should see `ROVIXANE-IR-Setup`; open `http://192.168.4.1` and create the admin password.

If flashing stalls at “Connecting…”, hold the board’s **FLASH** button while starting the command. Do not upload a LittleFS image; all web assets are inside the firmware.

## Updating from v1.0.0

- Keep a backup first: **Settings → Download backup**.
- Flash the v2.0.0 BIN with esptool, or use **Settings → Firmware update** on a device already running V2.
- `/config.json` and `/signals.json` are preserved when the filesystem is not erased. V1 flat config and signals are normalised to the V2 schema on load.
- A fresh device starts in setup mode; existing V1 configurations with login disabled remain open (change that in Settings).

## Security notes

- The dashboard is served over **plain HTTP** because of ESP8266 limits. Use it only on a trusted LAN or the setup AP; do not expose it to the internet.
- The setup AP is open until you set an AP password in Settings.
- API mutations require authentication plus `X-ROVIXANE-CSRF: 1` and JSON content type. The dashboard does this automatically.
- Password fields are masked in API responses and omitted from backups.

## Verification scope

- GitHub Actions runs host tests and the real ESP8266 compile; the attached BIN is produced by that build.
- IR range, receiver sensitivity, transistor wiring and OTA behaviour were **not** tested on physical hardware here. Verify them with your own board.
- Starter LG volume/source codes are examples marked `verified:false`; learn your own codes before relying on them.

## License

MIT. See [LICENSE](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/LICENSE).
