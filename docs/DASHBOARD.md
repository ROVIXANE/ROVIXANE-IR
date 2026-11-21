# Dashboard Guide

The dashboard is a mobile-first web app stored inside the firmware. It works offline and needs no internet connection once the BIN is flashed.

Open it at `http://192.168.4.1` during setup, or at your device’s home-network IP later.

---

## First screen

The app opens on the **Remotes** tab. A starter `LG TV (examples)` profile is installed on first boot so you can try buttons immediately. Those example codes are not guaranteed to match your TV; use **Learn** for reliable codes.

Top-right controls:

- **বাং / EN** — switch Bengali and English instantly.
- Connection/status area — shows AP mode, home Wi-Fi mode and IP address.

Use large buttons with your thumb. Scroll profiles horizontally; tap a button to send its IR code.

---

## Tabs

### Remotes

- **+ Profile** — create a new remote (TV, AC, fan, set-top box or anything else).
- **Edit** — rename a profile or change its icon and colour.
- **Duplicate** — copy a profile and its buttons.
- **Delete** — remove a profile.
- **+ Add button** — create a custom button with name, icon, colour, protocol, HEX code, bit count and repeat count.

Tapping a button sends the stored signal using GPIO4/D2 through the BC547 + IR LED driver.

Buttons can be renamed, recoloured, re-coded and deleted from the **✎** editor. This is how you build custom remotes for devices not in the starter data.

### Learn

1. Go to **Learn** → **Start learning**.
2. Point your original remote at the TSOP38238 receiver, about 2–10 cm away.
3. Press one button on the original remote.
4. The dashboard shows **Protocol**, **HEX**, **Bits** and, for unknown protocols, raw timings.
5. Tap **Save as button**, choose a name and colour, and pick the target profile.

Tips:

- Work away from direct sunlight and strong fluorescent lights.
- Keep the remote close and aim at the sensor lens.
- If decoding fails, try another button or move to a shaded area.
- Save learned signals immediately; they are kept in `/signals.json`.

### Settings

- **WiFi network (SSID)** / **WiFi password** — join your router.
- **Language** — Bengali or English.
- **Login / password** — optional dashboard protection.
- **Save settings** — writes `/config.json`.
- **Restart** — reboots the ESP8266.
- **Export JSON** — downloads a full backup of profiles, buttons and settings.
- **Restore JSON** — imports a previous backup.
- **Factory reset** — erases profiles, buttons and settings.

### Status

Shows the current connection state:

| State | Meaning |
|---|---|
| Setup AP | Broadcasting `IHR-IR-Setup` at `192.168.4.1` |
| Connecting | Trying the saved Wi-Fi network |
| Connected | Joined your router; shows the assigned IP |
| Fallback | Could not join; returning to setup AP |

---

## Categories

The starter profile is a TV example, but profiles are generic. Use names, icons and colours to organise:

- **TV** — learned power, volume, channel, input buttons
- **AC** — learned mode, temperature, fan-speed buttons
- **Fan** — learned speed and oscillation buttons
- **Set-top box / projector / audio** — any IR device you can learn from

This firmware supports IR protocols compiled into the build: NEC, LG, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 and raw capture. Wi-Fi–only or Bluetooth–only devices cannot be controlled.

---

## Backup and restore

1. **Settings → Export JSON** saves a file on your phone.
2. Keep a copy somewhere safe.
3. After a firmware update or factory reset, choose **Restore JSON** and select that file.

Never post the backup publicly if you enabled dashboard login, because it contains configuration data.

---

## Factory reset

You can reset in either way:

- **Settings → Factory reset** in the dashboard, or
- hold **D1 / GPIO5** to **GND** for 3 seconds.

After reboot the setup AP returns. If you use the physical D1/GPIO5 button instead, the built-in LED blinks while the button is held.

---

## Keyboard and accessibility

The interface uses large touch targets and high-contrast colours. It is designed for phones first, but works in desktop browsers too. If text looks too small, zoom in with your browser; the layout remains usable because controls wrap to one column on narrow screens.
