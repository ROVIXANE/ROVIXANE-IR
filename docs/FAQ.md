# সাধারণ প্রশ্ন ও সমাধান

এই FAQ নতুন ব্যবহারকারীদের জন্য লেখা। ধাপগুলো source-এর বর্তমান আচরণ ও প্রকাশিত v1.0.0 firmware-এর উপর ভিত্তি করে। যেসব বিষয় হার্ডওয়্যার-নির্ভর, সেগুলো স্পষ্টভাবে **আপনাকে যাচাই করতে হবে** হিসেবে চিহ্নিত।

---

## Wi-Fi ও প্রথম সেটআপ

### `ROVIXANE-IR-Setup` নেটওয়ার্কটি দেখছি না। করণীয়?

- বোর্ডে পাওয়ার আছে কি না দেখুন;
- USB serial monitor-এ 115200 baud-এ `ready` লেখা আছে কি দেখুন;
- বুটের পর কমপক্ষে ১২ সেকেন্ড অপেক্ষা করুন;
- আপনার ফোন 5 GHz-only মোডে আটকে থাকলে 2.4 GHz স্ক্যানে যান;
- আগে লগইন চালু করে ভুল পাসওয়ার্ড দিলে D1/GPIO5 কে GND-র সাথে ৩ সেকেন্ড ধরে ফ্যাক্টরি রিসেট করুন;
- Wi-Fi খারাপ হলে setup AP নিজে থেকেই ফিরে আসে—ডিভাইস হারিয়ে যায় না।

### Android “No internet” বলে পেজ বন্ধ করে দিচ্ছে।

**Stay connected**, **Use this network as-is** বা **Don't switch** বেছে নিন। তারপর ব্রাউজারে manually `http://192.168.4.1` লিখুন। এটি সাধারণ AP-র ক্ষেত্রে দেখা যায়; firmware-এর কোনো ত্রুটি নয়।

### `192.168.4.1` খুলছে না।

- ফোন সত্যিই `ROVIXANE-IR-Setup`-এ সংযুক্ত কি না দেখুন;
- Chrome বা Firefox চেষ্টা করুন;
- এই ঠিকানার browser cache clear করুন;
- VPN বা private DNS সাময়িকভাবে বন্ধ করুন;
- ব্রাউজারে ঠিকানা হিসেবে `http://192.168.4.1` লিখুন—শুধু `192.168.4.1` লিখলে কোনো কোনো browser search করে।

### ডিভাইস বাড়ির Wi-Fi-তে যোগ দিচ্ছে না।

- শুধু **2.4 GHz** SSID ব্যবহার করুন; 5 GHz দেখা যাবে না;
- পাসওয়ার্ড যাচাই করুন; WPA2 পরামর্শযোগ্য;
- হোটেল/ক্যাম্পাসের captive-portal নেটওয়ার্ক সমর্থিত নয়;
- ব্যর্থ চেষ্টার পর প্রায় ১২ সেকেন্ডে setup AP ফিরে আসে;
- router-এ MAC filtering বা client isolation চালু থাকলে সাময়িকভাবে বন্ধ করে দেখুন।

### বাড়ির Wi-Fi-তে যাওয়ার পর IP কীভাবে পাব?

- router-এর connected-devices / DHCP client list দেখুন;
- LAN scanner app ব্যবহার করুন;
- Serial Monitor 115200 baud-এ বুট লগ দেখুন;
- আবার `ROVIXANE-IR-Setup`-এ সংযোগ করে Diagnostics ট্যাব দেখুন।

IP পেলে `http://<device-ip>` ঠিকানায় ড্যাশবোর্ড খুলুন।

---

## IR শেখা ও পাঠানো

### Learn-এ কিছু দেখা যাচ্ছে না।

- আসল রিমোট TSOP38238 lens-এর দিকে ২–১০ সেমি দূরে ধরুন;
- রোদ, শক্তিশালী আলো ও অন্য IR উৎস থেকে দূরে যান;
- TSOP pinout যাচাই করুন: **1 = OUT → D5**, **2 = GND**, **3 = VCC → 3V3**;
- সংকেত noisy হলে VCC/GND-র পাশে 100 nF ক্যাপাসিটর যোগ করুন (**আপনাকে যাচাই করতে হবে**);
- রিমোটের অন্য বাটন চেষ্টা করুন; সব বাটন একই প্রোটোকলে কাজ না-ও করতে পারে।

### বাটন চাপলে টিভি/এসি সাড়া দেয় না।

- IR LED-এর দিক যাচাই করুন: long leg/anode → 47–220 Ω → 3V3 (**UNVERIFIED**);
- BC547 pinout যাচাই করুন: সমতল দিক আপনার দিকে ও পা নিচে রাখলে **C – B – E**;
- ফোনের ক্যামেরা LED-এর দিকে ধরে বাটন চাপুন—হালকা বেগুনি ঝলকানি মানে LED চালিত হচ্ছে;
- উদাহরণ কোডের বদলে **Learn signal** দিয়ে আপনার আসল রিমোটের কোড ব্যবহার করুন;
- কাছে গিয়ে ডিভাইসের সামনের sensor-এর দিকে তাক করে চেষ্টা করুন;
- IR LED-র সাথে রেজিস্টার না থাকলে তা যোগ করুন।

### এসি বা ফ্যান চালানো যাবে কি?

হ্যাঁ, যদি ডিভাইস IR ব্যবহার করে এবং তার প্রোটোকল firmware-এ থাকে বা Raw টাইমিং হিসেবে ধরা যায়। AC/Fan নামে প্রোফাইল বানিয়ে বাটনগুলো শিখুন। শুধু Wi-Fi বা Bluetooth-নির্ভর ডিভাইস সমর্থিত নয়।

### কিছু প্রোটোকল তালিকায় নেই কেন?

Firmware ছোট রাখতে শুধু NEC, LG, LG2, SONY, SAMSUNG, JVC, PANASONIC, RC5, RC6, DENON, COOLIX, MIDEA24 ও Raw capture কম্পাইল করা হয়েছে। IRremoteESP8266-এর অন্য প্রোটোকল ইচ্ছাকৃতভাবে বাদ দেওয়া হয়েছে।

### শেখা সংকেত কোথায় থাকে?

`/signals.json`-এ। সেটিংস `/config.json`-এ। দুটিই LittleFS-এ থাকে এবং সাধারণ firmware update-এ সংরক্ষিত হয়।

---

## ব্যাকআপ, সেটিংস ও ভাষা

### firmware update-এ প্রোফাইল মুছে যাবে কি?

সাধারণত না। তবে অপ্রত্যাশিত পরিস্থিতির জন্য আপডেটের আগে **Settings → Download backup** দিয়ে JSON ব্যাকআপ নিন। **Factory reset** সব মুছে দেয়।

### ব্যাকআপ ফাইল কী ধরনের?

`format: "ihr-backup"` সহ একটি JSON ফাইল। পুরনো firmware-এর সাথে সামঞ্জস্যের জন্য format id অপরিবর্তিত; ডাউনলোড filename মাত্র `rovixane-ir-backup-YYYY-MM-DD.json`। ব্যাকআপে সেটিংস থাকে, তাই publicly share করবেন না।

### বাংলা ও ইংরেজি কীভাবে বদলাবেন?

উপরের ডান কোণে **বাং / EN** বাটন চাপুন। Settings → `Interface language`-তেও অপশন আছে। প্রকাশিত v1.0.0-এর ডিফল্ট ভাষা ইংরেজি; নির্বাচিত ভাষা পরে সংরক্ষিত হয়। এই working tree-র নতুন বাংলা শব্দগুলো নতুন বিল্ড ছাড়া পুরনো BIN-এ দেখা যাবে না।

### বাংলা লেখা বিকৃত দেখাচ্ছে।

- ফোন ও ব্রাউজার আপডেট করুন;
- বড় সংস্করণের Bengali font থাকা device ব্যবহার করুন;
- কোনো remote font লোড হয় না—firmware system font-এর উপর নির্ভর করে;
- প্রয়োজনে browser zoom বাড়ান; layout এক কলামে সাজিয়ে থাকে।

### লগইন চালু করেছি, পাসওয়ার্ড ভুলে গেছি।

D1/GPIO5 কে GND-র সাথে প্রায় ৩ সেকেন্ড ধরে রাখুন। এটি ফ্যাক্টরি রিসেট করবে; এরপর setup AP ফিরে আসবে। আগের প্রোফাইলগুলো ব্যাকআপ থেকে ফিরিয়ে আনতে পারবেন।

---

## বিল্ড, release ও যাচাই

### Arduino IDE লাগবে কি?

না। Release BIN ডাউনলোড করে [INSTALL.md](INSTALL.md)-এর Termux কমান্ড দিয়ে ফ্ল্যাশ করলে চলবে। Arduino IDE শুধু source সংশোধন/compile করতে চাইলে লাগবে।

### release BIN কীভাবে বিশ্বাস করব?

প্রতিটি release GitHub Actions-এ source compile ও BIN header/checksum যাচাই সফল হওয়ার পর তৈরি হয়; assets-এ `.sha256.txt` থাকে। ডাউনলোডের পর `sha256sum -c` দিয়ে মিলিয়ে দেখুন।

### CI কি বাস্তব হার্ডওয়্যার পরীক্ষা করে?

না। CI শুধু software compile, asset/host tests, API contract ও header checks করে। Wiring, IR range, transistor polarity, TSOP pinout ও বাড়ির Wi-Fi আচরণ—এসব **আপনাকে locally যাচাই করতে হবে**।

### এই local environment-এ কি ESP8266 compile করা হয়েছে?

না। এখানে `arduino-cli` নেই, তাই বাস্তব ESP8266 compile দাবি করা হয় না। Host tests ও asset checks চলে; firmware compile-এর জন্য CI নির্ভর করুন।

---

## নিরাপত্তা

### ড্যাশবোর্ড ইন্টারনেটে দেওয়া যাবে কি?

না। HTTPS, account system বা internet firewall নেই। ড্যাশবোর্ড শুধু local network-এ রাখুন। Setup AP প্রথম বুটে ওপেন; অন্য লোক দেখতে পারলে Settings-এ লগইন চালু করুন।

### mains বা safety-critical ডিভাইস চালানো যাবে কি?

না। এটি শুধু consumer IR remote-এর জন্য। মেডিকেল, heating, security, mains switching বা safety-critical equipment-এ ব্যবহার করবেন না। IR LED-র সাথে সবসময় রেজিস্টার ব্যবহার করুন।

নিরাপত্তা রিপোর্ট: [SECURITY.md](../SECURITY.md)। আরও ধাপ: [INSTALL.md](INSTALL.md), [DASHBOARD.md](DASHBOARD.md)।
