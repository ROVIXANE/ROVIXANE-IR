# Changelog

All notable changes to this project are documented here.

## [1.0.0] - 2026-11-21

### Added

- ESP8266 NodeMCU 1.0 firmware with Wi-Fi setup access point and captive portal fallback.
- Mobile-first Bengali/English dashboard with large touch buttons.
- Remotes tab with profiles, duplicate/edit/delete and custom buttons.
- IR learning and signal storage with protocol, HEX, bits and raw timings.
- Settings for Wi-Fi, language, login, restart, backup/restore and factory reset.
- Clear connection status and automatic fallback to setup AP after failed Wi-Fi connection.
- Embedded web assets; no LittleFS upload required.
- GitHub Actions host tests, real ESP8266 firmware compilation, artifact upload and verified GitHub Release.
- Beginner documentation: README, WIRING, INSTALL, DASHBOARD, FAQ, CONTRIBUTING, SECURITY and release notes.

### Notes

- Supported board for the published BIN: NodeMCU 1.0 (ESP-12E), 4M1M flash, 80 MHz.
- Compiled IR protocols: NEC, LG, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 and raw capture.
- Physical IR range and wiring are not tested by CI; builders must verify hardware locally.
