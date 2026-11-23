# Security Policy

## Supported versions

| Version | Supported |
|---|---|
| 2.0.x | Yes |
| 1.0.x | Legacy, no new fixes |

## Reporting a vulnerability

Please **do not** open a public issue for security problems.

Use GitHub’s private **Report a vulnerability** button on the repository Security tab, or contact the maintainer privately through GitHub. Include:

- affected version/commit;
- reproduction steps;
- impact;
- suggested fix if you have one.

You should receive an acknowledgement within a reasonable time. Please allow time for a fix before public disclosure.

## Device security notes

- A fresh v2.0.0 device starts in **setup mode**: login is required by default and no admin password exists yet. Until you create one, mutation routes, IR transmit and OTA are blocked. The setup access point `ROVIXANE-IR-Setup` is open: anyone nearby who connects first can set the admin password. Set up in a trusted location immediately and set an AP password in **Settings**.
- Every state-changing request needs HTTP Basic auth (after setup), the `X-ROVIXANE-CSRF: 1` header and a JSON/multipart content type. This blocks cross-site form posts and simple text requests.
- Secrets are masked in API responses (`"********"` plus `*Set` booleans) and intentionally omitted from backups. Restoring a backup never overwrites a stored password with a mask.
- The dashboard is local-only HTTP. It does not provide HTTPS, user accounts or internet-facing protection. Do not port-forward it or expose it to the public internet.
- Do not enter a password you use elsewhere. A compromised local device could reveal it.
- Export backups contain profiles and settings but **not** passwords. They can still reveal your device layout and network SSID; store them privately and never paste them into issues.
- OTA accepts only `.bin` files with the ESP8266 magic byte through an authenticated, CSRF-protected, streaming upload. The updater performs format and flash-write checks, but no cryptographic signature or publisher authentication is checked. An interrupted upload should be aborted; these checks do not guarantee every truncated or malicious image is rejected. Only install a trusted release, verify its published checksum, and keep USB recovery available.
- Factory reset (D1/GPIO5 to GND for 3 seconds) clears local configuration but cannot undo data you shared elsewhere.

## Supply-chain and build safety

- Firmware is built in GitHub Actions from reviewed repository source.
- The release BIN is verified for non-zero size and a valid ESP8266 image header before upload.
- Host tests exercise setup gating, auth/CSRF/content-type failures, masked secrets, strict IR validation, OTA failure paths, `.bak` recovery and factory reset.
- A SHA-256 checksum file is published with each BIN so users can verify downloads.
- No GitHub token, API key or Wi-Fi credential is stored in the repository.

## Hardware safety

- Always use a current-limiting resistor with the IR LED.
- Disconnect USB before changing wiring.
- Do not use this project for medical, safety-critical, mains-switching or security systems.
- The ESP8266 is a 3.3 V device. Do not connect its GPIO pins to 5 V.

## Storage recovery

A mount failure or unreadable configuration stops boot without automatically formatting or overwriting the files. A brand-new or erased LittleFS partition also cannot mount: to initialize it, hold the config button (D1/GPIO5 to GND) during boot for at least three seconds. This explicitly formats and erases the entire LittleFS partition; never do this if data may need recovery. A serial console at 115200 baud reports the reason. Preserve a flash image before attempting recovery; recovery may require an experienced operator to restore the LittleFS partition or reflash. A successfully recovered backup leaves the damaged primary as `.corrupt`; if that quarantine cannot be created, writes are blocked. Factory reset removes the quarantined copies too, so do not reset before preserving data. An unreadable signal database is exposed empty but remains read-only until recovery or factory reset.

## Hardware smoke tests

On a spare ESP8266, verify setup and authentication, learn and replay both decoded and raw signals, disconnect and restore Wi-Fi, and upload a trusted firmware image by OTA with USB recovery available. Exercise Android login and file selection on a device or emulator connected to the board. Host mocks do not establish flash power-loss behavior, RF performance, or real OTA reliability.
