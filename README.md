# আইএইচআর ইউনিভার্সাল আইআর রিমোট (ESP8266)

[![CI](https://github.com/rmd734412-web/ihr-universal-ir-remote/actions/workflows/ci.yml/badge.svg)](https://github.com/rmd734412-web/ihr-universal-ir-remote/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/rmd734412-web/ihr-universal-ir-remote?label=release)](https://github.com/rmd734412-web/ihr-universal-ir-remote/releases/latest)

NodeMCU ESP-12E দিয়ে তৈরি একটি **সম্পূর্ণ ওয়াই-ফাই আইআর রিমোট**। মোবাইল ব্রাউজার থেকে টিভি/এসির রিমোট চালানো, নতুন সংকেত শেখা, প্রোফাইল বানানো—সবই ইন্টারনেট ছাড়াই চলে।

[রেজিস্টার/ওয়্যারিং কীভাবে লাগাবেন](WIRING.md) — সরাসরি এই লিংকটি দেখুন।

![IHR Universal IR Remote — NodeMCU ESP-12E, TSOP38238 ও TSAL620 + BC547 ওয়্যারিং ডায়াগ্রাম](docs/images/ihr-ir-wiring.svg)

> ডায়াগ্রামটি ফার্মওয়্যারের GPIO সংজ্ঞা থেকে যাচাই করা। মোবাইলে ছবিতে ট্যাপ করে জুম করুন; [SVG](docs/images/ihr-ir-wiring.svg) ও [PNG](docs/images/ihr-ir-wiring.png) — দুই ফরম্যাটেই আছে।

---

## ডাউনলোড ও প্রথম সেটআপ

**সবচেয়ে সহজ পথ:** [সর্বশেষ GitHub Release](https://github.com/rmd734412-web/ihr-universal-ir-remote/releases/latest) পেজ থেকে `ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin` ডাউনলোড করুন। সঙ্গে `.sha256.txt` ফাইলটিও নামালে ডাউনলোড যাচাই করা যায়।

- 📦 [Firmware Downloads / ফার্মওয়্যার ডাউনলোড](https://github.com/rmd734412-web/ihr-universal-ir-remote/releases/latest)
- 📖 [INSTALL.md — Android/Termux থেকে ফ্ল্যাশ ও সেটআপ](docs/INSTALL.md)
- 📱 [DASHBOARD.md — ড্যাশবোর্ড ব্যবহার](docs/DASHBOARD.md)
- ❓ [FAQ.md — সাধারণ প্রশ্ন](docs/FAQ.md)

সংক্ষেপে:

```sh
pkg install python
pip install esptool
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
python3 -m esptool --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
  write_flash --flash_size 4MB1M 0x00000 \
  ihr-universal-ir-remote-nodemcuv2-v1.0.0.bin
```

PORT-এর নাম `/dev/ttyACM0` হতে পারে। কানেকশন না ধরলে বোর্ডের **FLASH** বাটন চেপে রেখে কমান্ডটি চালান। USB OTG কেবল লাগবে। LittleFS/IOS upload লাগবে না—ওয়েব পেজগুলো BIN-এর ভেতরেই আছে।

> ডেভেলপাররা চাইলে Actions ট্যাবের সবুজ CI রান থেকেও artifact নামাতে পারেন: **Build firmware (NodeMCU ESP-12E) → Artifacts → `ihr-universal-ir-remote-nodemcuv2-v1.0.0`**। সাধারণ ব্যবহারকারীদের জন্য Release-এই সঠিক উৎস।


---

## ১. কী কী করতে পারবেন

| কাজ | কীভাবে |
|---|---|
| ওয়াই-ফাই দিয়ে রিমোট চালানো | মোবাইলে ড্যাশবোর্ড খুলে বাটন চাপুন |
| আইআর সংকেত শেখা | **Learn** ট্যাব → Start learning → রিমোটের বাটন চাপুন |
| সংকেত সংরক্ষণ | শেখার পর **Save as button** |
| LG ও অন্যান্য টিভি | LG, NEC, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 + raw |
| নিজের মতো বাটন | যেকোনো নাম, আইকন, রঙ ও কোড দিয়ে বাটন বানানো |
| বাটন সম্পাদনা | বাটনের উপরে ডান কোণে ✎ চাপুন (নাম/রঙ/কোড বদলানো) |
| বাটন মুছে ফেলা | ✎ দিয়ে সম্পাদনার সময় বা প্রোফাইল মুছে |
| একাধিক প্রোফাইল | **+ Profile**, Duplicate, Edit, Delete |
| ওয়াই-ফাই নাম/পাসওয়ার্ড বদল | **Settings** → Save settings → Restart |
| ওপেন বা পাসওয়ার্ড-সহ ওয়াই-ফাই | AP password খালি রাখলে ওপেন, নাহলে ৮–৬৩ অক্ষর |
| ব্যাকআপ / রিস্টোর | **Export JSON** / **Restore JSON** |
| সুন্দর মোবাইল ড্যাশবোর্ড | অ্যান্ড্রয়েড/আইফোন—দুটোতেই কাজ করে |
| বাংলা ও ইংরেজি | উপরের ডান কোণে **বাং** বাটন (সেটিংসেও আছে) |
| অফলাইন ইন্টারফেস | সব HTML/CSS/JS ডিভাইসের ভেতরেই আছে, ইন্টারনেট লাগবে না |

---

## ২. প্রয়োজনীয় জিনিস

### হার্ডওয়্যার
- NodeMCU ESP8266 ESP-12E
- TSOP38238 আইআর রিসিভার
- TSAL620 আইআর LED
- BC547 ট্রানজিস্টর
- 1kΩ রেজিস্টার (বেসের জন্য)
- **একটি 47–220Ω রেজিস্টার LED-র সঙ্গে যোগ করার পরামর্শ** (IR LED-র নিরাপদ রেঞ্জের জন্য; বিস্তারিত [WIRING.md](WIRING.md)-এ)
- ঐচ্ছিক: 100nF ক্যাপাসিটর (TSOP-এর পাশে), ছোট push button (ফ্যাক্টরি রিসেটের জন্য)

### সফটওয়্যার
- প্রস্তুত BIN চাইলে কোনো IDE লাগবে না—উপরের **ডাউনলোড ও প্রথম সেটআপ** অংশ দেখুন।
- নিজে কম্পাইল/আপলোড করতে চাইলে: Arduino IDE 2.x
- ESP8266 board package **3.1.2** (Board Manager থেকে)
- লাইব্রেরি (Library Manager থেকে): **ArduinoJson 7.4.2**, **IRremoteESP8266 2.8.6**
- ভিতরের লাইব্রেরি (আলাদা ইনস্টল লাগবে না): LittleFS, ESP8266WiFi, ESP8266WebServer, DNSServer

---

## ৩. খুব সহজে শুরু করুন

**নতুন ব্যবহারকারী:** উপরের [INSTALL.md](docs/INSTALL.md) গাইডটি অনুসরণ করুন। নিচের Arduino IDE ধাপগুলো শুধু তাদের জন্য যারা source থেকে নিজে কম্পাইল/আপলোড করতে চান।

### বিকল্প — Arduino IDE সেটআপ
1. Arduino IDE খুলুন → **File → Preferences** → “Additional boards manager URLs”-এ দিন:
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
2. **Tools → Board → Boards Manager** →Search দিয়ে `esp8266` → **esp8266 by ESP8266 Community (3.1.2)** ইনস্টল করুন।
3. **Tools → Manage Libraries** → `ArduinoJson` (7.4.2) ও `IRremoteESP8266` (2.8.6) ইনস্টল করুন।

### ধাপ ১ — কোড আপলোড
1. Arduino IDE-তে **File → Open** দিয়ে এই ফোল্ডারটি খুলুন (`ihr-universal-ir-remote`)。
   ভেতরে `ihr-universal-ir-remote.ino` ফাইলটি অটো খুলবে।
2. Tools মেনুতে সেট করুন:

   | সেটিং | মান |
   |---|---|
   | Board | NodeMCU 1.0 (ESP-12E Module) |
   | Upload Speed | 115200 |
   | CPU Frequency | 80 MHz |
   | Flash Size | 4MB (FS: 1MB) |

3. USB কেবল দিয়ে NodeMCU কম্পিউটারে দিন → **→ (Upload)** চাপুন।
   “Connecting…”-এ আটকে গেলে বোর্ডের **FLASH** বাটন কিছুক্ষণ চেপে রাখুন।
4. আপলোড শেষ হলে **Serial Monitor** খুলুন (115200)। `1.0.0 ready` লেখা দেখলেই বোঝা যাবে চলছে।

> কোনো LittleFS/IOS upload লাগবে না। ওয়েব পেজগুলো ফার্মওয়্যারের ভেতরেই আছে।

### ধাপ ২ — মোবাইলে ড্যাশবোর্ড খুলুন
1. আপনার ফোনের WiFi-তে নতুন নেটওয়ার্ক `IHR-IR-Setup` দেখা যাবে → সেটিতে কানেক্ট করুন (পাসওয়ার্ড লাগবে না)।
2. কিছু ফোনে APK পপ-আপ আসবে; সেটিতে **OK/Stay connected** চাপুন। পপ-আপ না এলে ব্রাউজারে যান: **http://192.168.4.1**
3. ড্যাশবোর্ড এসে গেলেই কাজ শুরু।

---

## ৪. নিজের ওয়াই-ফাইতে চালানো

1. ড্যাশবোর্ডে **Settings** ট্যাব খুলুন।
2. **WiFi network (SSID)** → আপনার রাউটারের নাম।
3. **WiFi password** → রাউটারের পাসওয়ার্ড (ওপেন নেটওয়ার্ক হলে খালি রাখুন)।
4. **Save settings** চাপুন → তারপর **Restart** চাপুন।
5. কয়েক সেকেন্ড পর ফোনটি আপনার রাউটারে কানেক্ট করুন। রাউটারের অ্যাডমিন পেজে ডিভাইসটির IP দেখুন (যেমন `192.168.1.50`) অথবা Serial Monitor-এ IP লেখা দেখুন।
6. ব্রাউজারে `http://192.168.1.50` (আপনার IP) লিখে ড্যাশবোর্ড খুলুন।

যদি রাউটারের পাসওয়ার্ড ভুল হয় বা রাউটার না থাকে, **১২ সেকেন্ড পর আবার `IHR-IR-Setup` এপি ফিরে আসবে**—তাই ডিভাইস “হারিয়ে” যাবে না।

---

## ৫. রিমোট ব্যবহারের ধারণা

### ধাপে ধাপে

1. **Remotes** ট্যাব → **+ Profile** দিয়ে একটি প্রোফাইল বানান (যেমন “Living Room LG”)।
2. প্রোফাইলের নাম/আইকন/রঙ **Edit** দিয়ে বদলানো যায়। **Duplicate** দিয়ে নকলও বানানো যায়।
3. **+ Add button** দিয়ে নতুন বাটন যোগ করুন। অথবা **Learn** ট্যাব থেকে বাটন শিখুন।
4. বাটন চাপলেই IR সিগনাল যাবে। নাম যেকোনো দিন—volume up, power, arrow ইত্যাদি।
5. বাটনের ✎ চাপলে নাম, বাংলা নাম, আইকন, রঙ, প্রোটোকল, HEX কোড, bits, repeat—সব বদলানো যায়।

### সংকেত শেখা (সবচেয়ে গুরুত্বপূর্ণ)
1. **Learn** ট্যাব → **Start learning**।
2. আপনার আসল রিমোটটি TSOP38238 রিসিভারের দিকে (প্রায় ১–২ ইঞ্চি) তাক করে বাটন চাপুন।
3. ১–২ সেকেন্ড পর “Protocol / HEX / Bits / Raw timings” দেখা যাবে।
4. **Save as button** চাপুন → নাম ও রঙ ঠিক করে সংরক্ষণ করুন।

> `LG TV (examples)` প্রোফাইলের কোডগুলো **শুধু উদাহরণ** (সব টিভিতে একই নয়)। আপনার টিভির জন্য **Learn** দিয়ে নিজের কোড শিখলেই নিশ্চিত কাজ করবে।

### ভাষা বদল
উপরের ডান কোণে **বাং / EN** বাটন চাপলেই সাথে সাথে বদলে যাবে। Settings → Language-তেও আছে।

---

## ৬. ব্যাকআপ, রিস্টোর, রিস্টার্ট

- **Export JSON**: সব প্রোফাইল, বাটন ও সেটিংস একটা `.json` ফাইলে ডাউনলোড হয় (ফোনে থাকে)।
- **Restore JSON**: সেই ফাইল আবার বেছে নিলে সব ফিরে আসে।
- **Restart**: ডিভাইস রিস্টার্ট হয় (ওয়াই-ফাই/লগইন বদলের পর দরকার)।
- **Factory reset**: সব প্রোফাইল ও সেটিংস মুছে যায়। ভুলে গেলে বোর্ডের **D1 (GPIO5)** পিনটিকে GND-র সঙ্গে **৩ সেকেন্ড** ধরে রাখুন—একই কাজ হবে (ঐচ্ছিক বাটন)।

> লগইন চালু করে পাসওয়ার্ড ভুলে গেছেন? ৩ সেকেন্ডের ফ্যাক্টরি রিসেটই একমাত্র রাস্তা।

---

## ৭. সমস্যা হলে

আরও বিস্তারিত সমাধান: [FAQ.md](docs/FAQ.md)।

| সমস্যা | করণীয় |
|---|---|
| ফোনে `IHR-IR-Setup` দেখছি না | বোর্ডে পাওয়ার আছে কি না দেখুন; Serial Monitor-এ `ready` আছে কি না; ১২ সেকেন্ড অপেক্ষা করুন |
| `192.168.4.1` খুলছে না | ফোনের WiFi সেটিংসে “Stay connected”/“Use as-is” চাপুন; ব্রাউজার ক্যাশে ক্লিয়ার করুন |
| বাটন চাপলে টিভি সাড়া দেয় না | LED-র দিক (polarity) ও BC547-এর C/B/E ঠিক আছে কি দেখুন ([WIRING.md](WIRING.md))। ফোনের ক্যামেরা দিয়ে LED-এর ছোট আলো দেখে পরীক্ষা করুন |
| Learn-এ কিছু ধরা পড়ছে না | TSOP-এর ১=OUT, ২=GND, ৩=VCC ক্রম ঠিক করুন; VCC-র পাশে 100nF দিন; সরাসরি রোদ এড়িয়ে চালান |
| টিভি কম দূর থেকে কাজ করে | LED-র সঙ্গে 47–220Ω রেজিস্টার যোগ করুন, TSAL620 কে টিভির সামনের সেন্সরের দিকে তাক করুন |
| বাংলা লেখা বিকৃত | ফোন/ব্রাউজার আপডেট করুন; ফন্ট ইন্টারনেট থেকে আসে না, সিস্টেম ফন্ট ব্যবহার হয় |
| কোড আপলোড হয় না | Board=NodeMCU 1.0, COM port ঠিক আছে কি দেখুন; FLASH বাটন চেপে আপলোড করুন |

---

## ৮. ফাইল কী কী

| ফাইল | কাজ |
|---|---|
| `ihr-universal-ir-remote.ino` | মূল firmware (WiFi, সার্ভার, IR send/learn) |
| `AppConfig.*`, `Storage.*`, `SignalStore.*` | সেটিংস ও সংকেত LittleFS-এ সংরক্ষণ |
| `WebAssets.h` | ওয়েব পেজগুলো ফ্ল্যাশে রাখার জন্য অটো-জেনারেটেড ফাইল |
| `data/index.html`, `app.css`, `app.js` | ড্যাশবোর্ডের আসল source |
| `data/devices.json` | প্রথম বুটে লোড হওয়া LG উদাহরণ প্রোফাইল |
| `tools/generate_assets.py` | `data/` বদলের পর `WebAssets.h` বানানোর টুল |
| `WIRING.md` | বিস্তারিত ওয়্যারিং গাইড |
| `docs/INSTALL.md` | ফার্মওয়্যার ফ্ল্যাশ, প্রথম সেটআপ ও আপডেট গাইড |
| `docs/DASHBOARD.md` | ড্যাশবোর্ড ও রিমোট ব্যবহারের গাইড |
| `docs/FAQ.md` | সাধারণ প্রশ্ন ও সমস্যার সমাধান |
| `CONTRIBUTING.md`, `SECURITY.md`, `CHANGELOG.md` | অবদান, নিরাপত্তা ও সংস্করণ তথ্য |
| `RELEASE_NOTES.md` | GitHub Release-এর মুক্তি নোট |
| `tests/` | কম্পিউটারে চালানো যাচাই (হার্ডওয়্যার লাগবে না) |

`data/`-এ কিছু বদলে থাকলে চালান:
```sh
python3 tools/generate_assets.py
```
এরপর আবার আপলোড করুন।

কোড যাচাই করতে (ঐচ্ছিক):
```sh
sh tests/run_tests.sh
```

---

## ৯. ছোট প্রযুক্তিগত তথ্য

- পিন: **D2/GPIO4** = IR send, **D5/GPIO14** = IR receive, **D1/GPIO5** = Optional reset button, **D4/GPIO2** = বিল্ট-ইন LED।
- FIRMWARE version: `1.0.0`
- সব ডেটা `/config.json` ও `/signals.json`-এ থাকে (atomic write, পাওয়ার গেলে নষ্ট হয় না)।
- ওয়েব সার্ভার: port 80, ক্যাপ্টিভ DNS: port 53, AP IP: `192.168.4.1`।
- API: `/api/config`, `/api/signals`, `/api/send`, `/api/learn/start|status|cancel`, `/api/status`, `/api/restart`, `/api/factory-reset`।

---

## ১০. সীমাবদ্ধতা ও নিরাপত্তা

- শুধু **2.4 GHz** Wi-Fi সমর্থিত; 5 GHz নেটওয়ার্ক দেখা যাবে না।
- IR শেখার জন্য ট্রান্সমিটার/রিসিভারের সামনে পরিষ্কার দৃশ্য লাগবে; রোদ বা শক্তিশালী লাইট বাধা দিতে পারে।
- শুধু NEC, LG, Sony, Samsung, JVC, Panasonic, RC5, RC6, Denon, Coolix, Midea24 ও raw timing সমর্থিত।
- সেটআপ AP প্রথম বারের জন্য ওপেন রাখা হয়; অন্যরা দেখতে পারলে Settings-এ লগইন চালু করুন।
- ড্যাশবোর্ড শুধু লোকাল নেটওয়ার্কের জন্য; ইন্টারনেটে expose করবেন না।
- একটি IR LED-র সঙ্গে অবশ্যই রেজিস্টার ব্যবহার করুন। মেডিকেল, মেইন-সুইচিং বা নিরাপত্তা-সংক্রান্ত ডিভাইস নিয়ন্ত্রণে ব্যবহার করবেন না।
- GitHub Actions শুধু সফটওয়্যার বিল্ড ও যাচাই করে; বাস্তব হার্ডওয়্যার পরীক্ষা ব্যবহারকারীকেই করতে হয়।

বিস্তারিত: [SECURITY.md](SECURITY.md), [FAQ.md](docs/FAQ.md)।

**লাইসেন্স:** MIT — [LICENSE](LICENSE) দেখুন।
