# ROVIXANE IR — Universal Infrared Remote v1.0.0

First public firmware release.

> **Naming note:** The published v1.0.0 release and its assets are intentionally left unchanged, including the legacy `ihr-universal-ir-remote-*` filenames and the `IHR-IR-Setup` SSID used by that firmware. Future releases follow `ROVIXANE-IR-vX.X.X` / `rovixane-ir-nodemcuv2-vX.X.X.bin` naming and use `ROVIXANE-IR-Setup`.

## What this release contains

- Real compiled NodeMCU ESP8266 firmware (not a placeholder): `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin` (published v1.0.0 asset, legacy name)
- SHA-256 checksum file: `ihr-universal-ir-remote-nodemcuv2-v1.0.0.sha256.txt`
- First-run setup card, clearer connection status, per-button delete, supported-protocol picker, and automatic setup-AP teardown after Wi-Fi recovery
- Source code and full documentation in this repository

The firmware was compiled by GitHub Actions with Arduino CLI 1.6.0, ESP8266 core 3.1.2, ArduinoJson 7.4.2 and IRremoteESP8266 2.8.6. The BIN was checked for non-zero size and a valid ESP8266 image header before being attached here.

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

See [WIRING.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/WIRING.md) for a beginner-friendly, illustrated-by-text wiring guide.

## Install from Android / Termux without Arduino IDE

1. Download `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin` from the **Assets** section of this release.
2. Optionally verify the download with the `.sha256.txt` file:

   ```sh
   sha256sum -c ihr-universal-ir-remote-nodemcuv2-v1.0.0.sha256.txt
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

5. Flash at address `0x00000`:

   ```sh
   python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
     write_flash --flash_size 4MB1M 0x00000 \
     ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin
   ```

   Replace `/dev/ttyUSB0` with the port shown on your phone. If flashing does not start, hold the board's **FLASH** button while running the command.

6. Reconnect power. Your phone should see the Wi-Fi network `IHR-IR-Setup` (the SSID used by the published v1.0.0 firmware; newer firmware uses `ROVIXANE-IR-Setup`).
7. Connect to it and open **http://192.168.4.1** in a browser. Choose **Stay connected** if Android asks.
8. In **Settings**, enter your home Wi-Fi name and password, save, and restart. The device falls back to `IHR-IR-Setup` after about 12 seconds if it cannot connect.

No LittleFS upload is needed. The entire dashboard is compiled into the firmware.

More detail: [INSTALL.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/docs/INSTALL.md).

## First-time setup and daily use

- **Remotes**: create a profile, add buttons, rename/edit/delete buttons.
- **Learn**: point your original remote at the TSOP38238 receiver and save the decoded signal.
- **Settings**: Wi-Fi, language (বাংলা / English), login, restart, backup/restore, factory reset.
- **Status**: shows Wi-Fi mode, IP address and whether the access point is active.

Step-by-step guides: [DASHBOARD.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/docs/DASHBOARD.md), [FAQ.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/docs/FAQ.md).

## Known limitations

- Only protocols compiled into this build are available: NEC, LG, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 and raw timing capture.
- IR learning needs line of sight and works best away from direct sunlight or strong fluorescent/LED lighting.
- The setup access point is open by default so a first-time user can connect. Enable the dashboard login on untrusted networks.
- Physical IR transmit/receive range was not hardware-tested in CI. Electrical wiring and real-device range must be checked by the builder.
- This is a 2.4 GHz Wi-Fi device; 5 GHz-only networks are not supported by the ESP8266.

## Safety and legal notes

- Do not connect the IR LED directly to 3.3 V without a current-limiting resistor.
- Do not use this project to control medical, safety-critical, mains switching or security equipment.
- Use factory reset if you forget the dashboard password: hold D1/GPIO5 to GND for 3 seconds.

## Full documentation

Start at [README.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/README.md). Contributions are welcome; see [CONTRIBUTING.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/CONTRIBUTING.md). Report security concerns privately as described in [SECURITY.md](https://github.com/ROVIXANE/ROVIXANE-IR/blob/main/SECURITY.md).
