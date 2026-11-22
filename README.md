# ROVIXANE IR — Universal Infrared Remote

[![CI](https://github.com/ROVIXANE/ROVIXANE-IR/actions/workflows/ci.yml/badge.svg)](https://github.com/ROVIXANE/ROVIXANE-IR/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/ROVIXANE/ROVIXANE-IR?label=release)](https://github.com/ROVIXANE/ROVIXANE-IR/releases/latest)

**ROVIXANE IR** হলো NodeMCU ESP8266 ESP-12E-এর উপর ভিত্তি করে তৈরি একটি ওপেন-সোর্স, অফলাইন-ফার্স্ট Wi-Fi ইনফ্রারেড রিমোট। ফোনের ব্রাউজার থেকে টিভি, এসি, ফ্যান বা অন্য যেকোনো IR ডিভাইসের বাটন চাপুন, আসল রিমোট থেকে নতুন সংকেত শিখুন, প্রোফাইল বানান—ইন্টারনেটের প্রয়োজন নেই।

**English:** ROVIXANE IR is an open-source, offline-first universal infrared remote controller built on the NodeMCU ESP8266 ESP-12E. It serves a mobile dashboard directly from the microcontroller, learns signals from an existing remote, stores reusable profiles, and keeps every asset locally in the firmware.

> **প্রকাশিত অবস্থা:** GitHub-এ প্রকাশিত **v1.0.0** release এবং তার asset অপরিবর্তিত রাখা হয়েছে। এই working tree-তে করা বাংলা UX/UI, onboarding ও ডকুমেন্টেশন উন্নতি নতুন করে বিল্ড ও প্রকাশ না হলে বর্তমান v1.0.0 BIN-এ দেখা যাবে না।

---

## দ্রুত লিংক

| যা খুঁজছেন | লিংক |
|---|---|
| ফার্মওয়্যার ডাউনলোড | [GitHub Releases](https://github.com/ROVIXANE/ROVIXANE-IR/releases/latest) |
| নতুন ব্যবহারকারীর ইনস্টলেশন | [docs/INSTALL.md](docs/INSTALL.md) |
| ড্যাশবোর্ড ব্যবহার | [docs/DASHBOARD.md](docs/DASHBOARD.md) |
| সাধারণ প্রশ্ন ও সমস্যা | [docs/FAQ.md](docs/FAQ.md) |
| ওয়্যারিং ও GPIO | [WIRING.md](WIRING.md) |
| নিরাপত্তা তথ্য | [SECURITY.md](SECURITY.md) |
| অবদান | [CONTRIBUTING.md](CONTRIBUTING.md) |
| সংস্করণ ইতিহাস | [CHANGELOG.md](CHANGELOG.md) |

---

## পরিচিতি

| বিবরণ | তথ্য |
|---|---|
| ব্র্যান্ড | ROVIXANE |
| প্রকল্প | ROVIXANE-IR |
| রিপোজিটরি | [github.com/ROVIXANE/ROVIXANE-IR](https://github.com/ROVIXANE/ROVIXANE-IR) |
| হার্ডওয়্যার | NodeMCU ESP8266 ESP-12E |
| বর্তমান প্রকাশিত সংস্করণ | v1.0.0 |
| ইন্টারফেস | বাংলা ও ইংরেজি; ড্যাশবোর্ডে এক ক্লিকে বদলানো যায় |
| সংযোগ | 2.4 GHz Wi-Fi, AP মোডে `ROVIXANE-IR-Setup`, IP `192.168.4.1` |
| অফলাইন | HTML, CSS, JS, PWA manifest—সব ফার্মওয়্যারের ভেতরে |

![ROVIXANE IR ওয়্যারিং ডায়াগ্রাম — NodeMCU ESP-12E, TSOP38238, TSAL620 ও BC547](docs/images/rovixane-ir-wiring.svg)

> GPIO সংজ্ঞা firmware/`Compatibility.h`-এ যাচাই করা। কম্পোনেন্ট pinout, LED polarity ও R/C মান শুধু ডকুমেন্টেশনে আছে এবং **UNVERIFIED** হিসেবে চিহ্নিত। মোবাইলে ছবিতে ট্যাপ করে জুম করুন; [SVG](docs/images/rovixane-ir-wiring.svg) ও [PNG](docs/images/rovixane-ir-wiring.png) দুই ফরম্যাটেই আছে। বিস্তারিত: [WIRING.md](WIRING.md)।

---

## কী কী করতে পারবেন

| বৈশিষ্ট্য | কীভাবে কাজ করে |
|---|---|
| রিমোট নিয়ন্ত্রণ | **Remotes** ট্যাব থেকে প্রোফাইল বেছে বাটন চাপুন |
| নতুন সংকেত শেখা | **Learn signal** → শেখা শুরু করুন → আসল রিমোটের বাটন চাপুন |
| সংকেত সংরক্ষণ | শেখার পর **Save as button** দিয়ে বাটন হিসেবে রাখুন |
| প্রোটোকল ও Raw | NEC, LG, LG2, SONY, SAMSUNG, JVC, PANASONIC, RC5, RC6, DENON, COOLIX, MIDEA24 এবং Raw টাইমিং |
| নিজস্ব বাটন | নাম, বাংলা নাম, আইকন, রঙ, HEX কোড, বিট সংখ্যা, ক্যারিয়ার ফ্রিকোয়েন্সি ও পুনরাবৃত্তি ঠিক করুন |
| প্রোফাইল ব্যবস্থাপনা | নতুন প্রোফাইল, সম্পাদনা, অনুলিপি ও মুছে ফেলা |
| Wi-Fi সেটআপ | প্রথম বুটে ওপেন setup AP; পরে বাড়ির 2.4 GHz নেটওয়ার্কে সংযোগ |
| ব্যাকআপ ও পুনরুদ্ধার | **Download backup** / **Restore backup** দিয়ে JSON ফাইল |
| রিস্টার্ট ও ফ্যাক্টরি রিসেট | ড্যাশবোর্ড থেকে অথবা D1/GPIO5 পিন দিয়ে |
| ডায়াগনস্টিকস | ফার্মওয়্যার, আপটাইম, Heap মেমোরি, ফাইল সিস্টেম, IP, RSSI, রিসেট কারণ |
| ভাষা | বাংলা/ইংরেজি টগল; পছন্দ ডিভাইসে সংরক্ষিত হয় |
| সম্পূর্ণ অফলাইন | কোনো CDN, remote font বা বাইরের API নেই |

---

## প্রয়োজনীয় জিনিস

### হার্ডওয়্যার

- NodeMCU ESP8266 ESP-12E
- TSOP38238 ইনফ্রারেড রিসিভার
- TSAL620 ইনফ্রারেড LED
- BC547 ট্রানজিস্টর
- 1 kΩ রেজিস্টার (BC547 বেসের জন্য)
- IR LED-র সাথে 47–220 Ω রেজিস্টার যোগ করার পরামর্শ; মান নির্ভরশীল ও **UNVERIFIED**
- ঐচ্ছিক: 100 nF ক্যাপাসিটর, ফ্যাক্টরি রিসেটের push button

### সফটওয়্যার

- প্রস্তুত BIN ব্যবহার করলে Arduino IDE লাগবে না
- নিজে কম্পাইল করতে চাইলে Arduino IDE 2.x
- ESP8266 board package 3.1.2
- ArduinoJson 7.4.2 এবং IRremoteESP8266 2.8.6
- Termux/Android-এ ফ্ল্যাশ করলে `python3` ও `esptool`

---

## দ্রুত শুরু: প্রকাশিত BIN

1. [সর্বশেষ Release](https://github.com/ROVIXANE/ROVIXANE-IR/releases/latest) খুলুন।
2. NodeMCU BIN এবং মিলে যাওয়া `.sha256.txt` ডাউনলোড করুন।
3. checksum যাচাই করুন:

```sh
cd ~/Downloads
sha256sum -c ihr-universal-ir-remote-nodemcuv2-v1.0.0.sha256.txt
```

4. USB OTG দিয়ে বোর্ড সংযোগ করুন ও ফ্ল্যাশ করুন:

```sh
pkg install python
pip install esptool
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
  write_flash --flash_size 4MB1M 0x00000 \
  ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin
```

> **v1.0.0 asset নাম:** প্রকাশিত ফাইলের নাম `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin`—পুরনো নাম অপরিবর্তিত রাখা হয়েছে। ভবিষ্যত বিল্ডের নাম `rovixane-ir-nodemcuv2-vX.X.X.bin`।

PORT `/dev/ttyACM0` হতে পারে। “Connecting……”-এ আটকে গেলে বোর্ডের **FLASH** বাটন চেপে রেখে কমান্ড চালান। LittleFS/IOS upload লাগবে না—ওয়েব পেজগুলো BIN-এর ভেতরেই আছে। বিস্তারিত: [INSTALL.md](docs/INSTALL.md)।

---

## প্রথম বুট ও Wi-Fi সেটআপ

1. বোর্ডে পাওয়ার দিন; প্রায় ৫–১২ সেকেন্ড অপেক্ষা করুন।
2. ফোনে `ROVIXANE-IR-Setup` Wi-Fi নেটওয়ার্কে সংযোগ করুন। এটি প্রথম সেটআপের জন্য ওপেন রাখা হয়।
3. Android “No internet” বললে **Stay connected** / **Use this network as-is** বেছে নিন।
4. ব্রাউজারে `http://192.168.4.1` খুলুন।
5. **Settings** → `Wi-Fi network (SSID)` ও `Wi-Fi password` দিন → **Save settings** → **Restart**।
6. ফোনকে আপনার নিজের 2.4 GHz Wi-Fi তে সংযোগ করে router-এর client list বা Serial Monitor থেকে IP বের করুন।

Wi-Fi ব্যর্থ হলে প্রায় ১২ সেকেন্ড পর setup AP ফিরে আসে, তাই ডিভাইস “হারিয়ে” যায় না।

---

## Arduino IDE থেকে source বিল্ড

1. Arduino IDE 2.x-এ **File → Preferences → Additional boards manager URLs**-এ দিন:
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
2. **Boards Manager** → `esp8266 by ESP8266 Community 3.1.2` ইনস্টল করুন।
3. **Library Manager** → `ArduinoJson 7.4.2`, `IRremoteESP8266 2.8.6` ইনস্টল করুন।
4. রিপোজিটরি ফোল্ডার খুলুন; `rovixane-ir.ino` অটো খুলবে।
5. Board: **NodeMCU 1.0 (ESP-12E Module)**, Flash Size: **4MB (FS:1MB)**, CPU: **80 MHz**, Upload Speed: **115200**।
6. Upload করুন; Serial Monitor 115200-এ `1.0.0 ready` দেখলে firmware চলছে।

`data/`-এ কোনো.web ফাইল বদলে থাকলে আগে চালান:

```sh
python3 tools/generate_assets.py
```

---

## ড্যাশবোর্ড পরিচিতি

- **Remotes:** প্রোফাইল বেছে বাটন চাপুন; ✎ দিয়ে বাটন সম্পাদনা করুন।
- **Learn signal:** আসল রিমোট TSOP38238-এর দিকে ধরে বাটন চাপুন; প্রোটোকল, HEX, বিট, ক্যারিয়ার ফ্রিকোয়েন্সি ও Raw টাইমিং দেখা যাবে।
- **Settings:** ডিভাইসের নাম, setup AP, Wi-Fi, ক্যাপ্টিভ পোর্টাল, লগইন, পুনরাবৃত্তি ও ভাষা।
- **Backup & restore:** JSON ব্যাকআপ ডাউনলোড/পুনরুদ্ধার, রিস্টার্ট ও ফ্যাক্টরি রিসেট।
- **Diagnostics:** ফার্মওয়্যার, আপটাইম, Heap মেমোরি, ফাইল সিস্টেম, IP, RSSI ও রিস্টার্ট কারণ।

> `LG TV (examples)` প্রোফাইলের কোড শুধু উদাহরণ; সব টিভিতে একই নয়। নিজের ডিভাইসের জন্য **Learn signal** দিয়ে কোড যাচাই করুন।

বিস্তারিত: [DASHBOARD.md](docs/DASHBOARD.md)।

---

## ব্যাকআপ, আপডেট ও পুনরুদ্ধার

- **Download backup:** প্রোফাইল, বাটন ও সেটিংস এক `.json` ফাইলে রাখে।
- **Restore backup:** আগের ব্যাকআপ ফাইল আবার লোড করে।
- Firmware আপডেটে LittleFS фай `/config.json` ও `/signals.json` সাধারণত সংরক্ষিত থাকে; তবুও আপডেটের আগে ব্যাকআপ নিন।
- **Factory reset** সব প্রোফাইল, বাটন ও সেটিংস মুছে দেয়। প্রয়োজনে D1/GPIO5 কে GND-র সাথে ৩ সেকেন্ড ধরে রাখুন।

---

## ডকুমেন্টেশন

| ডকুমেন্ট | কী আছে |
|---|---|
| [INSTALL.md](docs/INSTALL.md) | Release বেছে নেওয়া, checksum, Termux/Arduino IDE ফ্ল্যাশ, প্রথম সেটআপ, recovery |
| [DASHBOARD.md](docs/DASHBOARD.md) | ট্যাব, বাটন, শেখা, সেটিংস, ডায়াগনস্টিকস, মোবাইল ব্যবহার |
| [FAQ.md](docs/FAQ.md) | Wi-Fi, IR, ব্যাকআপ, ভাষা, বিল্ড ও নিরাপত্তা সংক্রান্ত প্রশ্ন |
| [WIRING.md](WIRING.md) | GPIO সংযোগ ও কম্পোনেন্ট তথ্য; unverified অংশ স্পষ্ট |
| [SECURITY.md](SECURITY.md) | দুর্বলতা রিপোর্ট ও নিরাপদ ব্যবহার |
| [CONTRIBUTING.md](CONTRIBUTING.md) | অবদানের নিয়ম |
| [CHANGELOG.md](CHANGELOG.md) / [RELEASE_NOTES.md](RELEASE_NOTES.md) | সংস্করণ তথ্য |

---

## প্রকল্পের গঠন

| ফাইল / ফোল্ডার | কাজ |
|---|---|
| `rovixane-ir.ino` | Wi-Fi, web server, IR send/learn, API |
| `AppConfig.*`, `Storage.*`, `SignalStore.*` | সেটিংস ও সংকেত LittleFS-এ রাখে |
| `Compatibility.h` | GPIO/pin compatibility macros |
| `WebAssets.h` | `data/` থেকে অটো-জেনারেটেড web asset header |
| `data/index.html`, `app.css`, `app.js` | ড্যাশবোর্ড source |
| `data/manifest.webmanifest` | PWA metadata |
| `data/devices.json` | প্রথম বুটের LG উদাহরণ প্রোফাইল |
| `tools/generate_assets.py` | `WebAssets.h` তৈরির টুল |
| `tests/` | host tests, asset contract checks, real IR header check |
| `.github/workflows/ci.yml` | firmware build, verification, release guard |

---

## যাচাই ও উন্নয়ন

```sh
python3 tools/generate_assets.py   # data/ বদলের পর
sh tests/run_tests.sh              # asset + host tests
```

`tests/run_tests.sh` যাচাই করে:

- `WebAssets.h` exactly matches `data/` generator output
- DOM id, API route, translation key ও PWA wiring
- starter signal database schema
- host-side firmware behavior
- real `IRremoteESP8266 v2.8.6` header contract

> এই local environment-এ `arduino-cli` নেই; তাই এখান থেকে বাস্তব ESP8266 firmware compile দাবি করা হয় না। CI-তে source compile ও BIN verification কনফিগার করা আছে। বাস্তব হার্ডওয়্যার পরীক্ষা ব্যবহারকারীকেই করতে হয়।

---

## সমস্যা হলে

| লক্ষণ | প্রথম ধাপ |
|---|---|
| `ROVIXANE-IR-Setup` দেখা যাচ্ছে না | পাওয়ার, `ready` লগ ও ১২ সেকেন্ড অপেক্ষা যাচাই করুন |
| `192.168.4.1` খুলছে না | setup AP-তে থাকুন, “Stay connected” বাছুন, cache clear করুন |
| বাটনে টিভি সাড়া দেয় না | IR LED দিক, BC547 C/B/E ও 47–220 Ω রেজিস্টার যাচাই করুন |
| Learn-এ কিছু ধরা পড়ছে না | TSOP pinout, 100 nF ক্যাপাসিটর ও রোদ/শক্তিশালী আলো এড়িয়ে চেষ্টা করুন |
| বাংলা লেখা বিকৃত | ব্রাউজার/ফোন আপডেট করুন; কোনো remote font লোড হয় না |

আরও: [FAQ.md](docs/FAQ.md)।

---

## সীমাবদ্ধতা ও নিরাপত্তা

- শুধু **2.4 GHz** Wi-Fi; 5 GHz সমর্থিত নয়।
- IR শেখার জন্য পরিষ্কার দৃশ্য লাগবে; রোদ বা শক্তিশালী লাইট বাধা দিতে পারে।
- শুধু উপরের তালিকাভুক্ত প্রোটোকল ও Raw timing সমর্থিত।
- Setup AP প্রথম বুটে ওপেন; অন্যরা দেখতে পারলে Settings-এ লগইন চালু করুন।
- ড্যাশবোর্ড শুধু local network-এর জন্য; HTTPS/internet firewall নেই, ইন্টারনেটে expose করবেন না।
- একটি IR LED-র সাথে অবশ্যই রেজিস্টার ব্যবহার করুন। মেডিকেল, mains switching, heating, security বা safety-critical ডিভাইসে ব্যবহার করবেন না।
- GitHub Actions শুধু সফটওয়্যার যাচাই করে; wiring, IR range ও বাস্তব আচরণ hardware owner-কে যাচাই করতে হয়।

---

## অবদান ও লাইসেন্স

অবদানের জন্য [CONTRIBUTING.md](CONTRIBUTING.md), নিরাপত্তা রিপোর্টের জন্য [SECURITY.md](SECURITY.md) দেখুন। এই প্রকল্প **MIT** লাইসেন্সের অধীনে; বিস্তারিত [LICENSE](LICENSE)।
