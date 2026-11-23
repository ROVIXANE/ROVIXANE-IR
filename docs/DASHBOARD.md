# ড্যাশবোর্ড গাইড (V2)

ROVIXANE IR ড্যাশবোর্ড একটি mobile-first ওয়েব অ্যাপ, যা firmware-এর ভেতরেই থাকে। BIN ফ্ল্যাশ করার পর ইন্টারনেটের প্রয়োজন হয় না—setup AP বা বাড়ির Wi-Fi দিয়ে সরাসরি ডিভাইসেই পেজ খুলে।

> এই গাইড **v2.0.0**-এর UI বর্ণনা করে। প্রকাশিত v1.0.0 BIN আলাদা এবং অপরিবর্তিত।

---

## ড্যাশবোর্ড কীভাবে খুলবেন

- প্রথম বুটে: `ROVIXANE-IR-Setup` Wi-Fi-তে সংযোগ করে `http://192.168.4.1` খুলুন।
- বাড়ির Wi-Fi-তে সরানোর পর: router-এ পাওয়া IP দিয়ে `http://<device-ip>` খুলুন।

প্রথম বুটে **Create admin password** স্ক্রিন আসবে। কমপক্ষে ৮ অক্ষরের পাসওয়ার্ড দিন। এর আগে ডিভাইস write-protected থাকে—IR পাঠানো, সেটিংস/সংকেত সংরক্ষণ ও OTA কাজ করবে না।

উপরে ডান কোণে **বাং / EN** বাটন দিয়ে ভাষা বদলান; ডার্ক/লাইট theme Settings-এ আছে। পছন্দ ডিভাইসে (এবং ভাষা/থিম ব্রাউজারে) সংরক্ষিত থাকে।

---

## হোম স্ক্রিন

উপর থেকে নিচে:

1. **Setup card** — শুধু প্রথম বুটে; পাসওয়ার্ড তৈরি বা Wi-Fi সেটআপের আহ্বান।
2. **Search** — রিমোট বা বাটনের নাম লিখে সাথে সাথে ফিল্টার।
3. **Favorites** — তারকা चिह्नিত রিমোট/বাটন (থাকলে)।
4. **Recent** — সম্প্রতি চালানো বাটন।
5. **Quick buttons** — প্রায়ই ব্যবহৃত বাটনের ছোট গ্রিড।
6. **All remotes** — সব প্রোফাইলের কার্ড। **Add remote** দিয়ে নতুন রিমোট।

প্রতিটি রিমোট কার্ডে আইকন, নাম, ধরন (TV/AC/Fan/STB/Custom) ও বাটন সংখ্যা দেখা যায়। কার্ডে ট্যাপ করলে সেই রিমোট খোলে। কার্ডের মেনু থেকে favorite, edit, duplicate, export বা delete করা যায়।

---

## রিমোট স্ক্রিন

রিমোট খুললে উপরে back, নাম, favorite (☆) ও more (⋯) থাকে। ধরন অনুযায়ী লেআউট স্বয়ংক্রিয়ভাবে দেখানো হয়:

| ধরন | সাধারণ বাটন |
|---|---|
| **TV** | Power, mute, volume, channel, input, navigation/D-pad, OK, back/home/menu, number pad, media |
| **STB** | Power, guide, navigation, volume/channel, numbers, info/exit, media |
| **Fan** | Power, speed, mode, oscillation, timer, light |
| **AC** | Power, temp −/+, mode, fan, swing, turbo, eco, sleep, timer + state panel |
| **Custom** | আপনি যে বাটন যোগ করেছেন |

- **বাটন চাপ** — ম্যাপ করা সংকেত IR LED দিয়ে পাঠায়; Unmapped বাটনে ট্যাপ করলে learn sheet খোলে।
- **✎ / long-press** — ওই বাটনের edit sheet: নাম, বাংলা নাম, আইকন, গ্রিড প্রস্থ (w), সংকেত বদল/শেখা, favorite ও delete।
- **⋯ মেনু** — রিমোট rename, icon/colour, duplicate, export, delete; **Edit buttons** চালু করলে **＋ Add button** দেখা যায়।
- **AC panel** — তাপমাত্রা, মোড ও ফ্যান স্পিডের state দেখায়। প্রতিটি AC কমান্ড আসলে আপনার শেখা/ম্যাপ করা কোড; অসমর্থিত প্রোটোকল বানিয়ে দেখানো হয় না।

> স্টার্টার `LG TV (examples)` প্রোফাইলের কোড `verified:false`। UI অযাচাইকৃত ম্যাপ করা বাটনে ছোট সতর্কতা চিহ্ন দেখায়। নিজের রিমোট দিয়ে learn না করা পর্যন্ত এগুলোকে নির্ভরযোগ্য ধরবেন না।

---

## প্রতি-বাটন শেখা (recommended)

1. রিমোট খুলুন এবং যে বাটন শিখতে চান সেটি ট্যাপ করুন (unmapped হলে), অথবা বাটনের edit sheet → **Learn**।
2. আসল রিমোটটি TSOP38238-এর দিকে ২–১০ সেমি দূরে ধরে বাটনটি চাপুন।
3. ড্যাশবোর্ড **Protocol**, **HEX**, **bits** অথবা **RAW timings** দেখাবে।
4. **Save to button** চাপুন। ম্যাপিং সাথে সাথে `/signals.json`-এ সংরক্ষিত হয়।

নোট:

- সমর্থিত প্রোটোকল ডিকোড হলে protocol/HEX হিসেবে; অসমর্থিত কিন্তু বৈধ Raw capture হলে raw timings হিসেবে সংরক্ষিত হয়।
- খুব বড়/overflowed বা অবৈধ capture স্পষ্ট error দেখিয়ে বাতিল হয়—ভুল সংকেত নীরবে সেভ হয় না।
- রোদ, শক্তিশালী আলো ও অন্য IR উৎস এড়িয়ে চলুন; রিমোটের lens সোজা ধরুন।

**Learn** ট্যাব-এ আলাদা generic learner-ও আছে: **Start learning** → বাটন চাপুন → ফল দেখুন → **Save as button** দিয়ে বর্তমান রিমোটে নতুন বাটন হিসেবে রাখুন।

---

## Favorites, recents ও profile

- যেকোনো রিমোট বা বাটনে ☆ চাপলে সেটি Favorites-এ যায়; হোমে দ্রুত access।
- সংকেত পাঠালে বাটনটি Recent-এ যায়।
- **Duplicate** পুরো প্রোফাইল ও বাটন কপি করে—একই TV-র দ্বিতীয় রুমের জন্য সুবিধাজনক।
- **Export remote** একটি প্রোফাইল JSON ফাইল নামায়; **Import remote** দিয়ে ফেরানো যায়।
- **Set as default** করলে হোমে সেটি আগে দেখানো যায় (UI অনুযায়ী)।
- Delete confirmation ছাড়া কিছু মুছে না।

---

## Custom remote builder

1. **Add remote** → ধরন **Custom**।
2. নাম, আইকন (ইমোজি), রঙ ও গ্রিড প্রস্থ বাছুন।
3. সংরক্ষণের পর রিমোট খুলুন → ⋯ → **Edit buttons** → **＋ Add button**।
4. প্রতিটি বাটনের নাম, বাংলা নাম, আইকন, প্রস্থ ও সংকেত দিন; অথবা **Learn** দিয়ে সরাসরি শিখুন।
5. বাটনগুলো drag/order না থাকলেও যোগ করার ক্রমেই সাজানো থাকে; edit sheet থেকে পরিবর্তন/মুছে ফেলা যায়।

কাস্টম রিমোটের সম্পূর্ণ button layout DB-তে সংরক্ষিত হয়, তাই firmware restart বা backup/restore-এর পরও একই থাকে।

---

## Settings

| সেটিং | কাজ |
|---|---|
| Device name | ড্যাশবোর্ড/status-এ দেখা নাম |
| Language | English / বাংলা |
| Theme | Dark / Light |
| Setup AP name | প্রথম সেটআপের Wi-Fi নাম |
| Setup AP password | খালি রাখলে AP উন্মুক্ত; নাহলে ৮–৬৩ অক্ষর |
| Open setup AP | AP password ছাড়া খোলা রাখার স্পষ্ট toggle |
| Home Wi-Fi name/password | বাড়ির 2.4 GHz নেটওয়ার্ক |
| Captive portal | AP-তে সংযোগে browser auto-open |
| Require dashboard login | Basic auth চালু/বন্ধ (fresh device-এ বাধ্যতামূলক) |
| Login user / new password | লগইন তথ্য; খালি রাখলে পুরোনো পাসওয়ার্ড থাকে |
| Default repeats (1–10) | প্রতি চাপে সংকেত কতবার |
| Save settings | `/config.json`-এ atomic write |

পাসওয়ার্ড কখনও response-এ ফেরত আসে না; UI `********` ও `*Set` flag দেখে। Wi-Fi, AP বা login বদলালে **Restart** প্রয়োজন হতে পারে—UI notice দেখাবে।

### Firmware update (OTA)

1. **Settings → Firmware update**।
2. `rovixane-ir-nodemcuv2-vX.X.X.bin` বাছুন → **Upload firmware**।
3. Progress bar ১০০% হলে ডিভাইস verified finalize করে নিজে রিস্টার্ট হয়।
4. ভুল ফাইল/magic, সাইজ overflow, নেটওয়ার্ক বিচ্ছিন্নতা বা auth/CSRF failure হলে আপডেট commit হয় না; পুরোনো firmware চালু থাকে।

আপডেটের আগে **Download backup** নিন এবং আপডেট চলাকালীন পাওয়ার/নেটওয়ার্ক স্থির রাখুন।

### Backup & restore

- **Download backup** — রিমোট, বাটন ম্যাপিং ও সেটিংস এক JSON-এ; পাসওয়ার্ড ইচ্ছাকৃতভাবে বাদ।
- **Restore backup** — ফাইল লোড করে; masked/অনুপস্থিত গোপন তথ্য বর্তমান মান অক্ষত রাখে।
- **Import remote** — শুধু একটি প্রোফাইল JSON যোগ করে।

### Diagnostics

Firmware version, schema version, uptime, free heap, filesystem used/total, IP, Wi-Fi mode/RSSI, reset reason ও OTA support দেখা যায়। **Refresh** দিয়ে live মান আপডেট করুন।

---

## রিস্টার্ট ও ফ্যাক্টরি রিসেট

- **Settings → Restart** — Wi-Fi/login/AP পরিবর্তন প্রয়োগ করতে।
- **Settings → Factory reset** — সব রিমোট, বাটন, layout ও সেটিংস মুছে দেয় (আগে confirmation লাগে)।
- **D1 / GPIO5** কে **GND**-র সাথে ৩ সেকেন্ড ধরে রাখলেও factory reset হয়।

Factory reset-এর পর ডিভাইস আবার setup mode-এ ফিরে যায় এবং `ROVIXANE-IR-Setup` AP চালু হয়।

---

## সমর্থিত প্রোটোকল

NEC, LG, LG2, SONY, SAMSUNG, JVC, PANASONIC, RC5, RC6, DENON, COOLIX, MIDEA24 এবং Raw timing। শুধু Wi-Fi/Bluetooth-নির্ভর ডিভাইস এই রিমোট দিয়ে চালানো যাবে না।

---

## মোবাইল ও accessibility

- বড় touch target, responsive grid ও bottom navigation;
- সংকীর্ণ স্ক্রিনে bottom sheet মডাল;
- **Esc** দিয়ে sheet বন্ধ, focus trap ও `aria-live` status;
- বাংলা system font ও বেশি line-height;
- ডার্ক/লাইট theme এবং হালকা animation (reduced-motion respect);
- ব্রাউজার zoom-এ layout ব্যবহারযোগ্য।

আরও সাহায্য: [FAQ.md](FAQ.md), [INSTALL.md](INSTALL.md), [SECURITY.md](../SECURITY.md)।
