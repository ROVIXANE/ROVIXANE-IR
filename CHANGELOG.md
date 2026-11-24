# Changelog

All notable changes to this project are documented here.

## [3.0.1] - 2026-12-02

### Fixed

- Version alignment across firmware, Android wrapper, dashboard and documentation.
- Release asset pipeline: firmware binary + SHA-256 checksum attached to GitHub release.

### Compatibility

- Data format `ihr-signals` v2 preserved.
- Previous releases (v1.0.0, v2.0.0, v2.0.1, v3.0.0) left untouched.

## [3.0.0] - 2026-12-01

### Changed

- Rebranded the project to ROVIXANE-IR V3 (firmware version 3.0.0, Android wrapper 3.0.0).
- Dashboard, CSS and firmware strings updated to V3 identity.
- WebAssets.h regenerated to match data/ sources.

### Compatibility

- Data format `ihr-signals` v2 preserved; V2 and V1 configurations remain readable.
- Previous releases (v1.0.0, v2.0.0, v2.0.1) left untouched.

## [2.0.0] - 2026-11-23

### Added

- Full universal remote dashboard: home/search/favorites/recents, TV, set-top box, fan, air-conditioner and custom remote layouts, plus per-button learning, editing, duplicate and delete.
- First-run setup mode: a fresh device requires an admin password through `POST /api/setup` before any write, IR transmit or OTA action is allowed.
- Favorites and recent-button views, profile defaults and profile management.
- Custom remote builder: name, icon, colour, grid width and a full stored button layout.
- IR learning per button, including protocol/HEX/bits or raw timings, with visible rejection of overflowed or invalid captures.
- Streaming OTA firmware update from the phone browser with `.bin` check, ESP8266 magic-byte check, size guard, auth and CSRF protection, and safe abort on failure.
- Crash-safe storage: atomic `.tmp` install, previous-good `.bak`, automatic recovery on boot, and a 20 KB database limit shared by storage and API.
- Diagnostics: schema version, uptime, heap, filesystem, Wi-Fi mode/RSSI, reset reason and OTA availability.
- Dark/light theme, English/Bangla switch, PWA manifest and mobile-first responsive layout.
- Host tests covering setup gating, auth/CSRF/content-type failures, masked secrets, strict IR validation, learning, OTA failure paths, `.bak` recovery and factory reset.

### Changed

- Firmware version is now `2.0.0`; release assets use `rovixane-ir-nodemcuv2-v2.0.0.bin` and tag `v2.0.0`.
- Configuration schema is nested with flat V1 compatibility; secrets are masked in API responses and preserved when omitted or restored as `********`.
- IR engine moved to `IrEngine.h` with strict 0–16 digit HEX parsing, negative-value rejection, raw timing range checks and protocol map.
- Signal storage moved to schema v2 with categories, buttons, labels and strict PUT validation; invalid replacements never become the live database.
- Web assets are regenerated from `data/` and served from PROGMEM; unknown paths still fall back to the captive dashboard.
- CI now stages the sketch under `rovixane-ir/` before compiling, so the repository folder name no longer breaks Arduino CLI.
- README, install, dashboard, FAQ and security docs describe V2 and the new setup/OTA flows.
- Merged upstream `main` (Android debug build scaffolding, upstream IR hardening and asset regeneration steps) without changing published releases.

### Security

- Destructive and state-changing routes require authentication, a `X-ROVIXANE-CSRF: 1` header and (where a body is used) a JSON content type.
- Passwords are never returned by `GET /api/config`; backups intentionally exclude them.
- Oversized request bodies and stored databases are rejected instead of silently erased.
- OTA accepts only `.bin` images starting with the ESP8266 magic byte and never restarts on a failed write.

### Notes

- Published v1.0.0 release and its legacy `ihr-universal-ir-remote-*` assets are left unchanged.
- Firmware builds and host tests are verified in CI. Physical IR range, wiring and OTA were not tested on hardware in this environment.

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
