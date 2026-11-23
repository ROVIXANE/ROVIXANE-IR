# ইনস্টলেশন গাইড — নতুন ব্যবহারকারীদের জন্য

এই গাইডটি আপনাকে একটি খালি NodeMCU ESP8266 ESP-12E বোর্ড থেকে শুরু করে ফোন-নিয়ন্ত্রিত **ROVIXANE IR** রিমোট পর্যন্ত নিয়ে যাবে। প্রস্তুত firmware BIN ব্যবহার করলে Arduino IDE বা প্রোগ্রামিং জ্ঞানের প্রয়োজন নেই।

> **সংস্করণ:** এই গাইড **v2.0.0**-এর জন্য। প্রকাশিত **v1.0.0** release ও তার legacy asset অপরিবর্তিত। v2.0.0 BIN-এর নাম `rovixane-ir-nodemcuv2-v2.0.0.bin` এবং প্রথম বুটে অ্যাডমিন পাসওয়ার্ড সেট করা বাধ্যতামূলক।

---

## ১. কী লাগবে

### হার্ডওয়্যার

- NodeMCU ESP8266 ESP-12E (4 MB flash)
- TSOP38238 ইনফ্রারেড রিসিভার
- TSAL620 ইনফ্রারেড LED
- BC547 ট্রানজিস্টর
- 1 kΩ রেজিস্টার (বেস)
- IR LED-র সাথে 47–220 Ω রেজিস্টার (যোগ করার পরামর্শ; মান **UNVERIFIED**)
- USB OTG কেবল (Android/Termux-এ ফ্ল্যাশ করলে)
- ঐচ্ছিক: 100 nF ক্যাপাসিটর, push button

### সফটওয়্যার

- প্রস্তুত BIN: কোনো IDE লাগবে না
- Android/Termux: `python3`, `esptool`
- Source থেকে বিল্ড: Arduino IDE 2.x, esp8266 3.1.2, ArduinoJson 7.4.2, IRremoteESP8266 2.8.6

---

## ২. সঠিক firmware BIN বেছে নিন

1. [ROVIXANE-IR Releases](https://github.com/ROVIXANE/ROVIXANE-IR/releases/latest) খুলুন।
2. **Assets** থেকে NodeMCU BIN ডাউনলোড করুন: `rovixane-ir-nodemcuv2-v2.0.0.bin`।
   পুরোনো প্রকাশিত v1.0.0-এর নাম `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin` (ইতিহাস অপরিবর্তিত)।
3. মিলে যাওয়া `.sha256.txt` ফাইলটিও ডাউনলোড করুন।

BIN টি এই কনফিগারেশনের জন্য তৈরি:

| সেটিং | মান |
|---|---|
| Board | NodeMCU 1.0 (ESP-12E) |
| Flash | 4 MB, file system 1 MB (`4M1M`) |
| CPU | 80 MHz |
| Flash offset | `0x00000` |

ESP32, Arduino Uno, Raspberry Pi বা অন্য Wi-Fi chip-এ এটি ফ্ল্যাশ করবেন না।

### ডাউনলোড যাচাই (ঐচ্ছিক কিন্তু পরামর্শযোগ্য)

Termux-এ:

```sh
cd ~/Downloads
sha256sum -c rovixane-ir-nodemcuv2-v2.0.0.sha256.txt
```

`OK` মানে ফাইলটি GitHub Actions-এ তৈরি প্রকাশিত বিল্ডের সাথে মিলে যাচ্ছে।

---

## ৩. সার্কিট তৈরি করুন

USB কেবল খুলে রেখে [WIRING.md](../WIRING.md) অনুসরণ করুন। সংক্ষেপে:

| NodeMCU পিন | সংযোগ |
|---|---|
| D2 / GPIO4 | 1 kΩ → BC547 base (IR LED driver) |
| D5 / GPIO14 | TSOP38238 OUT |
| D1 / GPIO5 | ঐচ্ছিক ফ্যাক্টরি রিসেট বাটন → GND |
| 3V3 | TSOP VCC; IR LED anode 47–220 Ω দিয়ে |
| GND | সব কম্পোনেন্টের common ground |

> GPIO সংজ্ঞা source-এ যাচাই করা। TSOP pinout, LED polarity ও R/C মান ডকুমেন্টে **UNVERIFIED**; পাওয়ার দেওয়ার আগে datasheet দিয়ে যাচাই করুন এবং খালি তার যেন পরস্পরকে স্পর্শ না করে।

---

## ৪. Android + Termux থেকে BIN ফ্ল্যাশ

USB OTG কেবল লাগবে এবং ফোনের kernel-কে USB-serial device দেখতে হবে।

```sh
pkg install python
pip install esptool
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

সাধারণত `/dev/ttyUSB0` বা `/dev/ttyACM0` দেখা যায়। তারপর:

```sh
python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
  write_flash --flash_size 4MB1M 0x00000 \
  rovixane-ir-nodemcuv2-v2.0.0.bin
```

আপনার ফোনে যে port দেখা গেছে সেটি দিন।

**“Connecting……”-এ আটকে গেলে:**

- কমান্ড চালানোর সময় বোর্ডের **FLASH** বাটন চেপে রাখুন;
- অন্য USB cable ব্যবহার করুন (কিছু cable শুধু power দেয়);
- Termux-এ serial device দেখা যাচ্ছে কি যাচাই করুন;
- ফাইলের নাম ও path ঠিক আছে কি দেখুন।

### বিকল্প: Arduino IDE (ডেভেলপারদের জন্য)

1. Arduino IDE 2.x ইনস্টল করুন।
2. Preferences → Additional boards manager URLs-এ দিন:
   `https://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. **esp8266 by ESP8266 Community 3.1.2** ইনস্টল করুন।
4. **ArduinoJson 7.4.2** ও **IRremoteESP8266 2.8.6** ইনস্টল করুন।
5. রিপোজিটরি ফোল্ডার খুলুন এবং সেট করুন:
   - Board: **NodeMCU 1.0 (ESP-12E Module)**
   - Flash Size: **4MB (FS:1MB)**
   - CPU: **80 MHz**
   - Upload Speed: **115200**
6. sketch upload করুন। LittleFS upload লাগবে না।

`data/` বদলে থাকলে আগে `python3 tools/generate_assets.py` চালান।

---

## ৫. প্রথম বুট ও onboarding

1. পাওয়ার দেওয়ার পর প্রায় ৫–১২ সেকেন্ড অপেক্ষা করুন।
2. ফোনের Wi-Fi তালিকায় `ROVIXANE-IR-Setup` খুঁজে সংযোগ করুন। এটি প্রথম সেটআপের জন্য ওপেন।
3. Android “No internet” / “ইন্টারনেট নেই” বললে **Stay connected** / **Use this network as-is** বেছে নিন।
4. ব্রাউজারে খুলুন: **http://192.168.4.1**
5. **Create admin password** স্ক্রিনে কমপক্ষে ৮ অক্ষরের একটি পাসওয়ার্ড দিন → **Create password**।
6. লগইন হয়ে গেলে **Settings** ট্যাব খুলুন।

> সেটআপের আগে ডিভাইসটি write-protected: IR পাঠানো, config/signals লেখা ও OTA বন্ধ থাকে। এটি ইচ্ছাকৃত নিরাপত্তা বৈশিষ্ট্য, বাগ নয়। পাসওয়ার্ড ভুলে গেলে D1/GPIO5 দিয়ে factory reset করুন।

বিল্ট-ইন LED সাধারণ অবস্থায় জ্বালা থাকে না; IR শেখা বা physical factory-reset button চাপা অবস্থায় জ্বলজ্বল করে।

---

## ৬. বাড়ির Wi-Fi নেটওয়ার্কে সংযোগ

1. **Settings** → `Wi-Fi network (SSID)`-এ রাউটারের 2.4 GHz নাম দিন।
2. `Wi-Fi password`-এ পাসওয়ার্ড দিন; ওপেন নেটওয়ার্ক হলে খালি রাখুন।
3. **Save settings** চাপুন।
4. **Restart** চাপুন; কয়েক সেকেন্ড অপেক্ষা করুন।
5. ফোনকে আপনার নিজের Wi-Fi তে সংযোগ করুন।

ESP8266 শুধু **2.4 GHz** সমর্থিত। রাউটারে আলাদা 2.4 GHz ও 5 GHz নাম থাকলে 2.4 GHz টি বাছুন। পাসওয়ার্ড ভুল হলে বা রাউটার না পাওয়া গেলে প্রায় ১২ সেকেন্ড পর setup AP ফিরে আসে।

### নতুন IP কীভাবে পাবেন

- router-এর connected-devices তালিকা দেখুন;
- LAN scanner app ব্যবহার করুন;
- আবার `ROVIXANE-IR-Setup`-এ সংযোগ করে Status/Diagnostics দেখুন;
- USB serial monitor 115200 baud-এ বুট লগ দেখুন।

তারপর ব্রাউজারে `http://<device-ip>` খুলুন।

---

## ৭. firmware আপডেট

### বিকল্প ১: ফোনের ব্রাউজার থেকে OTA (v2.0.0+)

1. Releases থেকে নতুন `.bin` নামান; আগে **Settings → Download backup** দিয়ে JSON ব্যাকআপ রাখুন।
2. একই Wi-Fi-তে থেকে ড্যাশবোর্ড খুলুন → **Settings → Firmware update**।
3. `.bin` ফাইল বাছুন → **Upload & install**। আপলোড streaming, তাই পুরো ফাইল RAM-এ রাখতে হয় না।
4. ১০০% হলে ডিভাইস নিজে রিস্টার্ট হয়। ভুল magic, সাইজ বা সংযোগ ব্যর্থ হলে আপডেট commit হয় না এবং পুরোনো firmware চালু থাকে।

OTA-তে login (Basic auth), `X-ROVIXANE-CSRF: 1` ও multipart upload লাগে; ড্যাশবোর্ড নিজে যোগ করে। ফাইল অবশ্যই `.bin` হতে হবে এবং ESP8266 image (`0xE9` দিয়ে শুরু) হতে হবে।

### বিকল্প ২: USB + esptool

1. উপরের esptool কমান্ড দিয়ে নতুন BIN `0x00000`-এ ফ্ল্যাশ করুন।
2. রিস্টার্টের পর প্রয়োজনে **Settings → Restore backup** দিয়ে ব্যাকআপ ফিরিয়ে আনুন।

প্রোফাইল ও সেটিংস LittleFS-এ (`/config.json`, `/signals.json`, `.bak`) থাকে; সাধারণ firmware update এগুলো মুছে না। **Factory reset** মুছে দেয়। V1 থেকে এলে flat config/signals স্বয়ংক্রিয়ভাবে V2 schema-তে normalise হয়।

---

## ৮. সমস্যা হলে

| সমস্যা | করণীয় |
|---|---|
| Setup AP দেখা যাচ্ছে না | পাওয়ার, `ready` লগ, ১২ সেকেন্ড অপেক্ষা যাচাই করুন |
| `192.168.4.1` খুলছে না | সঠিক AP-তে আছেন কি দেখুন; “Stay connected” বাছুন; cache clear করুন |
| Wi-Fi join হচ্ছে না | 2.4 GHz SSID, পাসওয়ার্ড ও WPA2 যাচাই করুন; captive portal সমর্থিত নয় |
| IR LED সাড়া দেয় না | [FAQ.md](FAQ.md)-এর camera test ও [WIRING.md](../WIRING.md) দেখুন |
| Learn খালি থাকে | TSOP pinout, 100 nF, রোদ/শক্তিশালী আলো এড়িয়ে আবার চেষ্টা করুন |
| লগইন পাসওয়ার্ড ভুলে গেছেন | D1/GPIO5 কে GND-র সাথে ৩ সেকেন্ড ধরে factory reset |
| ফ্ল্যাশ ব্যর্থ | FLASH বাটন, অন্য cable/port, নতুন BIN ডাউনলোড |

---

## ৯. কী যাচাই হয়েছে, কী আপনাকে যাচাই করতে হবে

| বিষয় | অবস্থা |
|---|---|
| Asset generator ও `WebAssets.h` sync | host test দিয়ে যাচাই |
| DOM id, API route, translation key, PWA wiring | host test দিয়ে যাচাই |
| Starter signal database schema | host test দিয়ে যাচাই |
| IRremoteESP8266 v2.8.6 header contract | host test দিয়ে যাচাই |
| Release BIN source compile + checksum | CI দিয়ে যাচাই |
| Setup gating, auth/CSRF, masked secrets, OTA failure paths, `.bak` recovery | host test দিয়ে যাচাই |
| বাস্তব GPIO, TSOP pinout, LED polarity, R/C মান | **UNVERIFIED** — আপনাকে যাচাই করতে হবে |
| IR range, ফোনের camera test, বাস্তব OTA/flash, বাড়ির Wi-Fi আচরণ | hardware-নির্ভর; CI এটি দাবি করে না |
| এই local environment-এ ESP8266 compile | `arduino-cli` না থাকায় এখানে দাবি করা হয় না |

নিরাপত্তা ও সীমাবদ্ধতার জন্য [SECURITY.md](../SECURITY.md) এবং [FAQ.md](FAQ.md) দেখুন।

### LittleFS initialization and recovery

On a new or erased LittleFS partition, hold the D1/GPIO5 config button to GND during power-on for at least three seconds to explicitly initialize storage. **This formats and erases the entire LittleFS partition.** If the device previously contained data, do not format it: see [storage recovery](../SECURITY.md#storage-recovery) first. A failed mount without that physical confirmation stops boot without formatting.
