# Installation Guide (Beginner Friendly)

This guide takes you from a blank NodeMCU board to a working phone-controlled IR remote. You do **not** need Arduino IDE or programming knowledge to use the prepared firmware.

---

## 1. Get the correct firmware

Always use the **latest verified release**, not a random file from the source tree.

1. Open [Releases](https://github.com/rmd734412-web/ihr-universal-ir-remote/releases/latest).
2. Download `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin` from **Assets**.
3. Download the matching `.sha256.txt` file too.

This BIN is built for:

- NodeMCU 1.0 (ESP-12E)
- 4 MB flash, 1 MB file system (`4M1M`)
- 80 MHz CPU

Do not flash it to ESP32, Raspberry Pi, Arduino Uno or another Wi-Fi chip.

### Optional: verify the download

In Termux:

```sh
cd ~/Downloads
sha256sum -c ihr-universal-ir-remote-nodemcuv2-v1.0.0.sha256.txt
```

`OK` means the file matches the build published by GitHub Actions.

---

## 2. Build the circuit

Follow [WIRING.md](../WIRING.md) slowly. Short version:

| NodeMCU pin | Connect to |
|---|---|
| D2 / GPIO4 | 1kΩ → BC547 base (IR LED driver) |
| D5 / GPIO14 | TSOP38238 OUT |
| D1 / GPIO5 | Optional factory-reset button to GND |
| 3V3 | TSOP VCC and IR LED anode through 47–220Ω |
| GND | Common ground for everything |

Unplug USB while wiring. Check that no bare wires touch each other before powering on.

---

## 3. Flash the BIN from Android + Termux

You need a USB OTG cable and a phone kernel that exposes USB-serial devices.

```sh
pkg install python
pip install esptool
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

You should see something like `/dev/ttyUSB0` or `/dev/ttyACM0`. Then run:

```sh
python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
  write_flash --flash_size 4MB1M 0x00000 \
  ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin
```

Replace the port with the one found on your phone.

If it says “Connecting……” forever:

- hold the board’s **FLASH** button while running the command;
- try a different USB cable (some are power-only);
- check that Termux can see the serial device;
- make sure the file path and file name are correct.

### Alternative: Arduino IDE (optional)

If you already use a computer:

1. Install Arduino IDE 2.x.
2. Add `https://arduino.esp8266.com/stable/package_esp8266com_index.json` in Preferences → Additional boards manager URLs.
3. Install **esp8266 by ESP8266 Community version 3.1.2**.
4. Install **ArduinoJson 7.4.2** and **IRremoteESP8266 2.8.6**.
5. Open the repository folder and select:
   - Board: **NodeMCU 1.0 (ESP-12E Module)**
   - Flash Size: **4MB (FS:1MB)**
   - CPU: **80 MHz**
   - Upload Speed: **115200**
6. Upload the sketch. No LittleFS upload is needed.

This path is for developers. Beginners should prefer the release BIN.

---

## 4. First boot

After flashing and powering on:

1. Wait about 5 seconds.
2. On your phone, open Wi-Fi settings and connect to **`IHR-IR-Setup`**. It is open so first setup is easy.
3. If Android says “No internet”, choose **Stay connected** / **Use this network as-is**.
4. Open a browser and go to **http://192.168.4.1**.

You should see the IHR dashboard. The built-in LED blinks while IR learning or a physical factory-reset button hold is active; it stays off during normal operation.

---

## 5. Connect to your home Wi-Fi

1. Open the **Settings** tab.
2. Enter your **WiFi network (SSID)**.
3. Enter the **WiFi password**. Leave it empty for an open network.
4. Tap **Save settings**.
5. Tap **Restart**.

The ESP8266 can only join **2.4 GHz** Wi‑Fi. If your router offers separate 2.4 GHz and 5 GHz names, use the 2.4 GHz one.

If the password is wrong or the router is unavailable, the device returns to `IHR-IR-Setup` after roughly 12 seconds. Nothing is permanently locked out.

Find the new IP address in one of these ways:

- look at your router’s connected-devices list;
- use a network scanner app;
- reconnect to `IHR-IR-Setup` and check the **Status** card;
- use a USB serial monitor at 115200 baud.

Then open `http://<device-ip>` in your phone browser.

---

## 6. Firmware updates

1. Download the newer verified BIN from Releases.
2. Save a backup first: **Settings → Export JSON**.
3. Flash the new BIN at `0x00000` with the same command as above.
4. After reboot, restore your backup if needed using **Settings → Restore JSON**.

Your profiles and settings live in LittleFS files `/config.json` and `/signals.json`, so a normal firmware update does not erase them. Factory reset does erase them.

---

## 7. Recovery

| Problem | Recovery |
|---|---|
| Forgot dashboard password | Hold D1/GPIO5 to GND for 3 seconds for factory reset |
| Wi-Fi password changed and device cannot connect | Wait 12 seconds, reconnect to `IHR-IR-Setup`, set Wi-Fi again |
| Dashboard looks broken | Clear browser cache or try another browser |
| Relay/IR LED not responding | Recheck wiring and use the phone-camera LED test in [FAQ.md](FAQ.md) |
| Flash failed | Hold FLASH button, try another cable/port, re-download the BIN |

---

## 8. What was actually tested

GitHub Actions compiles the real firmware and validates the BIN header and checksum. Host tests validate dashboard assets, API behavior and IR headers.

Physical IR range, transistor polarity on a real breadboard and Wi-Fi behavior in your home are **hardware checks you perform locally**. CI does not claim to have tested hardware.
