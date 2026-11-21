# Frequently Asked Questions

## Wi-Fi and setup

### I cannot see `IHR-IR-Setup`.

- Make sure the board has power (USB or 5 V).
- Wait at least 12 seconds after boot.
- The setup AP returns automatically if saved Wi-Fi fails.
- Check that your phone is not stuck on a 5 GHz-only network; the ESP8266 uses 2.4 GHz.
- If a previous dashboard login is enabled and you cannot get in, hold D1/GPIO5 to GND for 3 seconds.

### Android says “No internet” and closes the page.

Choose **Stay connected**, **Use this network as-is** or **Don't switch**. Then manually open `http://192.168.4.1`.

### `192.168.4.1` does not open.

- Confirm you are connected to `IHR-IR-Setup`, not another network.
- Try Chrome or Firefox.
- Clear the browser cache for that address.
- Disable VPN/private DNS temporarily.

### The device will not join my Wi-Fi.

- Use a **2.4 GHz** SSID.
- Check the password; WPA2 is preferred.
- Captive-portal networks (hotels, campuses) are not supported.
- After a failed attempt, the setup AP comes back automatically in about 12 seconds.

### I do not know the device IP after switching to home Wi-Fi.

Check your router’s client list, use a LAN scanner app, or reconnect to the setup AP to read the status page. Serial Monitor at 115200 baud also prints the IP on boot.

---

## IR learning and sending

### Learn shows nothing.

- Aim the original remote at the TSOP38238 lens from 2–10 cm.
- Move away from sunlight, strong lamps and other IR sources.
- Verify TSOP pin order: **1 = OUT → D5**, **2 = GND**, **3 = VCC → 3V3**.
- Add a 100nF capacitor across TSOP VCC/GND if the signal is noisy.
- Try a different button on the original remote.

### My TV does not respond when I tap a button.

- Check the IR LED direction: long leg/anode goes through 47–220Ω to 3V3.
- Check BC547 pins: with the flat side facing you and legs down: **C – B – E**.
- Open your phone camera and point it at the IR LED; tap a button. A faint purple flash means the LED is driven.
- Use **Learn** to capture your real remote codes instead of the example profile.
- Move closer and aim at the TV’s front sensor.

### Can it control my AC or fan?

Yes, if the device uses IR and its protocol is supported or can be captured as raw timing. Create a profile named AC/Fan and learn the buttons. Wi-Fi-only or Bluetooth-only devices cannot be controlled.

### Why are some protocols missing?

To keep the firmware small, only NEC, LG, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 and raw capture are compiled in. Other protocols in IRremoteESP8266 are intentionally disabled.

---

## Data and safety

### Will I lose my remotes when updating firmware?

No. Profiles and settings are stored in LittleFS files `/config.json` and `/signals.json`. Still export a JSON backup before updating. **Factory reset** does erase them.

### Is the setup Wi-Fi password protected?

No. The `IHR-IR-Setup` access point is intentionally open so first-time setup is simple. Enable dashboard login in Settings if other people can reach the device.

### Can I expose the dashboard to the internet?

Not recommended. There is no HTTPS, account system or internet firewall. Keep it on your local network.

### Can I control mains-powered or safety-critical equipment?

No. This project is for IR remote control of consumer AV devices. Do not use it for medical, security, heating, mains switching or safety-critical equipment.

---

## Build and project

### Do I need Arduino IDE?

No. Download the verified BIN from GitHub Releases and flash it with the documented Termux commands. Arduino IDE is only for people who want to modify and compile the source themselves.

### How do I know the release BIN is real?

Each release is created only after GitHub Actions successfully compiles the ESP8266 firmware and verifies the BIN header. The release assets include a SHA-256 checksum file.

### Was the hardware tested in CI?

No. CI compiles and validates software artifacts. Wiring, IR range and real-device behavior must be verified by the person assembling the hardware.

### Where do I report a bug?

Open a GitHub Issue with your board model, firmware version, wiring summary and what you expected versus what happened. For security issues, follow [SECURITY.md](SECURITY.md) instead of opening a public issue.
