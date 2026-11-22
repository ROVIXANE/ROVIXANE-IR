# Security Policy

## Supported versions

| Version | Supported |
|---|---|
| 1.0.x | Yes |

## Reporting a vulnerability

Please **do not** open a public issue for security problems.

Use GitHub’s private **Report a vulnerability** button on the repository Security tab, or contact the maintainer privately through GitHub. Include:

- affected version/commit;
- reproduction steps;
- impact;
- suggested fix if you have one.

You should receive an acknowledgement within a reasonable time. Please allow time for a fix before public disclosure.

## Device security notes

- The setup access point `ROVIXANE-IR-Setup` is intentionally open so first-time users can connect. Enable dashboard login in **Settings** if other people can see the device.
- The dashboard is local-only HTTP. It does not provide HTTPS, user accounts or internet-facing protection. Do not port-forward it or expose it to the public internet.
- Do not enter a password you use elsewhere. A compromised local device could reveal it.
- Export backups contain profiles, settings and possibly login configuration. Store them privately and never paste them into issues.
- Factory reset (D1/GPIO5 to GND for 3 seconds) clears local configuration but cannot undo data you shared elsewhere.

## Supply-chain and build safety

- Firmware is built in GitHub Actions from reviewed repository source.
- The release BIN is verified for non-zero size and a valid ESP8266 image header before upload.
- A SHA-256 checksum file is published with each BIN so users can verify downloads.
- No GitHub token, API key or Wi-Fi credential is stored in the repository.

## Hardware safety

- Always use a current-limiting resistor with the IR LED.
- Disconnect USB before changing wiring.
- Do not use this project for medical, safety-critical, mains-switching or security systems.
- The ESP8266 is a 3.3 V device. Do not connect its GPIO pins to 5 V.
