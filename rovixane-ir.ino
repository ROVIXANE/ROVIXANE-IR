/*
  ROVIXANE IR — Universal Infrared Remote
  Firmware for NodeMCU ESP8266 (ESP-12E)
  https://github.com/ROVIXANE/ROVIXANE-IR
  -----------------------------------------------------------------
  - Sends and learns IR signals (LG / NEC / Sony / Samsung / JVC /
    Panasonic / RC5 / RC6 / Denon / Coolix / Midea24 / raw).
  - Stores every remote and button in /signals.json on LittleFS.
  - Serves a fully offline, mobile-friendly dashboard over WiFi.
  - Falls back to a captive portal (open or password protected) so the
    WiFi name/password can always be changed from a phone.

  The web files live in data/ and are compiled into WebAssets.h by
  `python3 tools/generate_assets.py`. No filesystem upload is required.
*/

#include "Compatibility.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

#include "WebAssets.h"
#include "Storage.h"
#include "AppConfig.h"
#include "SignalStore.h"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static const IPAddress IHR_AP_IP(192, 168, 4, 1);
static const IPAddress IHR_AP_MASK(255, 255, 255, 0);

static AppConfig gCfg;
static ESP8266WebServer gServer(IHR_HTTP_PORT);
static DNSServer gDns;
static IRrecv gIrRecv(IHR_PIN_IR_RECV, IHR_RECV_BUF_SIZE);
static IRsend gIrSend(IHR_PIN_IR_SEND);

static bool gApMode = false;
static bool gStaTrying = false;
static uint32_t gStaTryAt = 0;
static uint32_t gStaRetryAt = 0;

static bool gRestartPending = false;
static bool gFactoryPending = false;
static uint32_t gActionAt = 0;

static bool gBtnWasDown = false;
static uint32_t gBtnDownAt = 0;

struct LearnCapture {
  bool active;
  bool ready;
  char kind[8];
  char protocol[24];
  char hex[20];
  uint16_t bits;
  uint16_t khz;
  uint16_t timings[IHR_MAX_RAW_TIMINGS];
  uint8_t timingCount;
};
static LearnCapture gLearn;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------
static bool ieq(const char *a, const char *b) {
  if (a == nullptr || b == nullptr) return false;
  while (*a && *b) {
    char ca = (*a >= 'a' && *a <= 'z') ? (*a - 32) : *a;
    char cb = (*b >= 'a' && *b <= 'z') ? (*b - 32) : *b;
    if (ca != cb) return false;
    ++a;
    ++b;
  }
  return *a == '\0' && *b == '\0';
}

static void appendHex64(String &out, uint64_t value, uint8_t minDigits) {
  char digits[17];
  int pos = 16;
  digits[pos] = '\0';
  do {
    uint8_t d = value & 0xFULL;
    digits[--pos] = (d < 10) ? char('0' + d) : char('A' + d - 10);
    value >>= 4;
  } while (value > 0);
  int written = 16 - pos;
  if (minDigits > 16) minDigits = 16;
  while (written < minDigits) {
    digits[--pos] = '0';
    ++written;
  }
  out.concat(digits + pos);
}

static bool parseHex64(const char *text, uint64_t &out) {
  if (text == nullptr) return false;
  while (*text == ' ' || *text == '\t') ++text;
  if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) text += 2;
  if (*text == '\0') return false;
  uint64_t value = 0;
  uint8_t digits = 0;
  for (; *text != '\0'; ++text) {
    char c = *text;
    uint8_t d;
    if (c >= '0' && c <= '9') d = c - '0';
    else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
    else if (c == ' ' || c == '_' || c == ',') continue;
    else return false;
    // A 64-bit value has at most 16 hexadecimal digits; otherwise shifting
    // wraps and can silently produce a different transmitted code.
    if (++digits > 16) return false;
    value = (value << 4) | d;
  }
  if (digits == 0) return false;
  out = value;
  return true;
}

static bool jsonToU64(JsonVariantConst value, uint64_t &out) {
  if (value.is<const char *>()) return parseHex64(value.as<const char *>(), out);
  if (value.is<String>()) {
    String s = value.as<String>();
    return parseHex64(s.c_str(), out);
  }
  if (value.is<int64_t>() && value.as<int64_t>() < 0) return false;
  if (value.is<uint64_t>() || value.is<int64_t>() || value.is<unsigned long>() ||
      value.is<long>() || value.is<unsigned int>() || value.is<int>()) {
    out = value.as<uint64_t>();
    return true;
  }
  return false;
}

static bool textTooLong(JsonVariant value, size_t maxLength) {
  if (value.is<const char *>()) {
    const char *text = value.as<const char *>();
    return text != nullptr && strlen(text) > maxLength;
  }
  if (value.is<String>()) return value.as<String>().length() > maxLength;
  return false;
}

static decode_type_t protocolFromName(const char *name) {
  if (name == nullptr) return UNKNOWN;
  if (ieq(name, "NEC")) return NEC;
  if (ieq(name, "LG")) return LG;
  if (ieq(name, "LG2")) return LG2;
  if (ieq(name, "SONY") || ieq(name, "SONY12")) return SONY;
  if (ieq(name, "SAMSUNG")) return SAMSUNG;
  if (ieq(name, "JVC")) return JVC;
  if (ieq(name, "PANASONIC")) return PANASONIC;
  if (ieq(name, "RC5")) return RC5;
  if (ieq(name, "RC6")) return RC6;
  if (ieq(name, "DENON")) return DENON;
  if (ieq(name, "COOLIX")) return COOLIX;
  if (ieq(name, "MIDEA24")) return MIDEA24;
  return UNKNOWN;
}

static const char *protocolName(decode_type_t type) {
  switch (type) {
    case NEC: return "NEC";
    case LG: return "LG";
    case LG2: return "LG2";
    case SONY: return "SONY";
    case SAMSUNG: return "SAMSUNG";
    case JVC: return "JVC";
    case PANASONIC: return "PANASONIC";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case DENON: return "DENON";
    case COOLIX: return "COOLIX";
    case MIDEA24: return "MIDEA24";
    default: return "UNKNOWN";
  }
}

static uint16_t defaultBits(decode_type_t type) {
  switch (type) {
    case NEC: return 32;
    case LG: case LG2: return 28;
    case SONY: return 12;
    case SAMSUNG: return 32;
    case JVC: return 16;
    case PANASONIC: return 48;
    case RC5: return 12;
    case RC6: return 20;
    case DENON: return 15;
    case COOLIX: return 24;
    case MIDEA24: return 24;
    default: return 32;
  }
}

static bool wifiIsConnected() { return WiFi.status() == WL_CONNECTED; }

static void scheduleRestart(uint32_t delayMs) {
  gRestartPending = true;
  gFactoryPending = false;
  gActionAt = millis() + delayMs;
}

static void scheduleFactoryReset(uint32_t delayMs) {
  gFactoryPending = true;
  gRestartPending = false;
  gActionAt = millis() + delayMs;
}

// ---------------------------------------------------------------------------
// IR sending
// ---------------------------------------------------------------------------
static bool sendProtocolFrame(decode_type_t type, uint64_t value, uint16_t bits) {
  switch (type) {
#if defined(SEND_NEC) && SEND_NEC
    case NEC: gIrSend.sendNEC(value, bits); return true;
#endif
#if defined(SEND_LG) && SEND_LG
    case LG: gIrSend.sendLG(value, bits); return true;
#endif
#if defined(SEND_LG) && SEND_LG
    case LG2: gIrSend.sendLG2(value, bits); return true;
#endif
#if defined(SEND_SONY) && SEND_SONY
    case SONY: gIrSend.sendSony(value, bits); return true;
#endif
#if defined(SEND_SAMSUNG) && SEND_SAMSUNG
    case SAMSUNG: gIrSend.sendSAMSUNG(value, bits); return true;
#endif
#if defined(SEND_JVC) && SEND_JVC
    case JVC: gIrSend.sendJVC(value, bits); return true;
#endif
#if defined(SEND_PANASONIC) && SEND_PANASONIC
    case PANASONIC: gIrSend.sendPanasonic64(value, bits); return true;
#endif
#if defined(SEND_RC5) && SEND_RC5
    case RC5: gIrSend.sendRC5(value, bits); return true;
#endif
#if defined(SEND_RC6) && SEND_RC6
    case RC6: gIrSend.sendRC6(value, bits); return true;
#endif
#if defined(SEND_DENON) && SEND_DENON
    case DENON: gIrSend.sendDenon(value, bits); return true;
#endif
#if defined(SEND_COOLIX) && SEND_COOLIX
    case COOLIX: gIrSend.sendCOOLIX(value, bits); return true;
#endif
#if defined(SEND_MIDEA24) && SEND_MIDEA24
    case MIDEA24: gIrSend.sendMidea24(value, bits); return true;
#endif
    default: return false;
  }
}

static bool transmitSignal(JsonObject signal, uint8_t repeat) {
  if (repeat == 0 || repeat > 10) repeat = gCfg.irRepeat;
  if (repeat == 0 || repeat > 10) repeat = 1;

  String kind = signal["kind"] | "protocol";
  if (kind == "raw") {
    JsonArray arr = signal["timings"].as<JsonArray>();
    uint16_t buf[IHR_MAX_RAW_TIMINGS];
    uint8_t count = 0;
    for (JsonVariant v : arr) {
      // Invalid/zero entries must not be silently skipped: doing so changes
      // mark/space alternation and produces a different IR frame.
      if (!v.is<uint32_t>()) return false;
      uint32_t us = v.as<uint32_t>();
      if (us == 0 || us > 65535) return false;
      if (count >= IHR_MAX_RAW_TIMINGS) return false;
      buf[count++] = static_cast<uint16_t>(us);
    }
    if (count < 4) return false;
    uint16_t khz = signal["khz"] | 38;
    if (khz < 30 || khz > 60) khz = 38;
    for (uint8_t i = 0; i < repeat; ++i) {
      gIrSend.sendRaw(buf, count, khz);
      if (i + 1 < repeat) delay(40);
    }
    return true;
  }

  String name = signal["protocol"] | "";
  decode_type_t type = protocolFromName(name.c_str());
  if (type == UNKNOWN) return false;
  uint64_t value = 0;
  if (!jsonToU64(signal["value"], value)) return false;
  uint16_t bits = signal["bits"] | defaultBits(type);
  if (bits == 0 || bits > 64) bits = defaultBits(type);
  for (uint8_t i = 0; i < repeat; ++i) {
    if (!sendProtocolFrame(type, value, bits)) return false;
    if (i + 1 < repeat) delay(40);
  }
  return true;
}

// ---------------------------------------------------------------------------
// IR learning
// ---------------------------------------------------------------------------
static void startLearning() {
  memset(&gLearn, 0, sizeof(gLearn));
  gLearn.active = true;
  gIrRecv.enableIRIn();
}

static void cancelLearning() {
  gLearn.active = false;
  gLearn.ready = false;
}

static void pollLearning() {
  if (!gLearn.active) return;
  decode_results result;
  if (!gIrRecv.decode(&result)) return;

  gLearn.active = false;
  gLearn.ready = true;

  if (result.decode_type != UNKNOWN && result.bits > 0) {
    strlcpy(gLearn.kind, "protocol", sizeof(gLearn.kind));
    strlcpy(gLearn.protocol, protocolName(result.decode_type), sizeof(gLearn.protocol));
    uint16_t digits = static_cast<uint16_t>(((result.bits + 7) / 8) * 2);
    if (digits > 16) digits = 16;
    gLearn.hex[0] = '0';
    gLearn.hex[1] = 'x';
    gLearn.hex[2] = '\0';
    String hex;
    hex.reserve(20);
    hex = "0x";
    appendHex64(hex, result.value, digits);
    strlcpy(gLearn.hex, hex.c_str(), sizeof(gLearn.hex));
    gLearn.bits = result.bits;
    if (gLearn.bits > 64) gLearn.bits = 64;
  } else {
    strlcpy(gLearn.kind, "raw", sizeof(gLearn.kind));
    strlcpy(gLearn.protocol, "RAW", sizeof(gLearn.protocol));
    gLearn.hex[0] = '\0';
    gLearn.bits = 0;
  }
  gLearn.khz = 38;

  // Keep the raw capture too: useful when a protocol decoder is unavailable.
  if (result.rawbuf != nullptr && result.rawlen > 1) {
    uint16_t maxIndex = result.rawlen - 1;
    if (maxIndex > IHR_MAX_RAW_TIMINGS) maxIndex = IHR_MAX_RAW_TIMINGS;
    for (uint16_t i = 1; i <= maxIndex; ++i) {
      uint32_t us = static_cast<uint32_t>(result.rawbuf[i]) * kRawTick;
      gLearn.timings[gLearn.timingCount++] =
          static_cast<uint16_t>(us > 65535 ? 65535 : us);
    }
  }
  gIrRecv.resume();
}

// ---------------------------------------------------------------------------
// WiFi
// ---------------------------------------------------------------------------
static void startAccessPoint() {
  if (gApMode) return;
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(IHR_AP_IP, IHR_AP_IP, IHR_AP_MASK);
  WiFi.softAP(gCfg.apSsid, AppCfg::apPasswordPtr(gCfg));
  gApMode = true;
  gStaTrying = false;
  if (gCfg.captivePortal) gDns.start(IHR_DNS_PORT, "*", IHR_AP_IP);
}

static void stopAccessPoint() {
  if (!gApMode) return;
  gDns.stop();
  WiFi.softAPdisconnect(true);
  gApMode = false;
}

static void startWifi() {
  if (gCfg.wifiSsid[0] != '\0') {
    WiFi.mode(WIFI_STA);
    WiFi.begin(gCfg.wifiSsid, gCfg.wifiPassword);
    gStaTrying = true;
    gStaTryAt = millis();
    gStaRetryAt = millis();
  } else {
    startAccessPoint();
  }
}

static void pollWifi() {
  if (gStaTrying && !wifiIsConnected() && millis() - gStaTryAt > 12000UL) {
    startAccessPoint();
  }
  // Once the saved network comes back, remove the setup AP and captive DNS so
  // phones do not stay attached to a network that no longer leads to the device.
  if (gApMode && gCfg.wifiSsid[0] != '\0' && wifiIsConnected()) {
    stopAccessPoint();
    gStaTrying = false;
  }
  if (gApMode && gCfg.wifiSsid[0] != '\0' && !wifiIsConnected() &&
      millis() - gStaRetryAt > 60000UL) {
    WiFi.begin(gCfg.wifiSsid, gCfg.wifiPassword);
    gStaRetryAt = millis();
  }
}

static void pollConfigButton() {
  bool down = digitalRead(IHR_PIN_CFG_BTN) == LOW;
  if (down && !gBtnWasDown) gBtnDownAt = millis();
  if (!down && gBtnWasDown && gBtnDownAt &&
      millis() - gBtnDownAt >= IHR_CFG_HOLD_MS) {
    scheduleFactoryReset(200);
  }
  gBtnWasDown = down;
}

// ---------------------------------------------------------------------------
// HTTP helpers
// ---------------------------------------------------------------------------
static bool authOk() {
  if (!gCfg.httpAuth) return true;
  if (gServer.authenticate(gCfg.httpUser, gCfg.httpPassword)) return true;
  gServer.requestAuthentication();
  return false;
}

static void sendJson(int code, JsonDocument &doc) {
  String body;
  serializeJson(doc, body);
  gServer.send(code, "application/json; charset=utf-8", body);
}

static void sendOk(const char *message = "ok", bool restartRequired = false) {
  JsonDocument doc;
  doc["ok"] = true;
  doc["message"] = message;
  doc["restartRequired"] = restartRequired;
  sendJson(200, doc);
}

static void sendError(int code, const char *message) {
  JsonDocument doc;
  doc["ok"] = false;
  doc["error"] = message;
  sendJson(code, doc);
}

static void sendProgmem(const char *data, size_t length, const char *type,
                        bool cacheable) {
  if (cacheable) gServer.sendHeader("Cache-Control", "public, max-age=3600");
  gServer.setContentLength(length);
  gServer.send(200, type, "");
  char chunk[128];
  for (size_t offset = 0; offset < length; offset += sizeof(chunk)) {
    size_t n = length - offset;
    if (n > sizeof(chunk)) n = sizeof(chunk);
    memcpy_P(chunk, data + offset, n);
    gServer.sendContent(chunk, n);
  }
}

static void fillConfigDoc(JsonDocument &doc, const AppConfig &cfg) {
  doc["version"] = ROVIXANE_IR_FIRMWARE_VERSION;
  doc["deviceName"] = cfg.deviceName;
  doc["ap"]["ssid"] = cfg.apSsid;
  doc["ap"]["password"] = cfg.apPassword;
  doc["ap"]["captivePortal"] = cfg.captivePortal;
  doc["wifi"]["ssid"] = cfg.wifiSsid;
  doc["wifi"]["password"] = cfg.wifiPassword;
  doc["auth"]["user"] = cfg.httpUser;
  doc["auth"]["password"] = cfg.httpPassword;
  doc["auth"]["enabled"] = cfg.httpAuth;
  doc["locale"] = cfg.locale;
  doc["ir"]["repeat"] = cfg.irRepeat;
  // Flat aliases make the dashboard payload simple and backup files readable.
  doc["apSsid"] = cfg.apSsid;
  doc["apPassword"] = cfg.apPassword;
  doc["wifiSsid"] = cfg.wifiSsid;
  doc["wifiPassword"] = cfg.wifiPassword;
  doc["captivePortal"] = cfg.captivePortal;
  doc["httpAuth"] = cfg.httpAuth;
  doc["httpUser"] = cfg.httpUser;
  doc["httpPassword"] = cfg.httpPassword;
  doc["irRepeat"] = cfg.irRepeat;
}

static void handleRoot() {
  if (!authOk()) return;
  sendProgmem(kWebIndexHtml, kWebIndexHtmlLen, "text/html; charset=utf-8", false);
}
static void handleCss() {
  if (!authOk()) return;
  sendProgmem(kWebAppCss, kWebAppCssLen, "text/css; charset=utf-8", true);
}
static void handleJs() {
  if (!authOk()) return;
  sendProgmem(kWebAppJs, kWebAppJsLen, "application/javascript; charset=utf-8", true);
}
static void handleManifest() {
  if (!authOk()) return;
  sendProgmem(kWebManifest, kWebManifestLen, "application/manifest+json; charset=utf-8", true);
}

static void handleConfigGet() {
  if (!authOk()) return;
  JsonDocument doc;
  fillConfigDoc(doc, gCfg);
  sendJson(200, doc);
}

static void handleConfigPost() {
  if (!authOk()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large");
    return;
  }
  JsonDocument doc;
  if (deserializeJson(doc, body)) {
    sendError(400, "invalid JSON");
    return;
  }

  // Reject over-long strings BEFORE strlcpy() silently truncates them.
  static const struct { const char *name; size_t max; } limits[] = {
      {"deviceName", 31}, {"apSsid", 32},       {"apPassword", 63},
      {"wifiSsid", 32},   {"wifiPassword", 64}, {"httpUser", 32},
      {"httpPassword", 64}, {"locale", 7}};
  for (size_t i = 0; i < sizeof(limits) / sizeof(limits[0]); ++i) {
    if (doc.containsKey(limits[i].name) && textTooLong(doc[limits[i].name], limits[i].max)) {
      sendError(400, "field too long");
      return;
    }
  }

  AppConfig next = gCfg;
  if (doc.containsKey("deviceName"))
    strlcpy(next.deviceName, doc["deviceName"] | "", sizeof(next.deviceName));
  if (doc.containsKey("apSsid"))
    strlcpy(next.apSsid, doc["apSsid"] | "", sizeof(next.apSsid));
  if (doc.containsKey("apPassword"))
    strlcpy(next.apPassword, doc["apPassword"] | "", sizeof(next.apPassword));
  if (doc.containsKey("wifiSsid"))
    strlcpy(next.wifiSsid, doc["wifiSsid"] | "", sizeof(next.wifiSsid));
  if (doc.containsKey("wifiPassword"))
    strlcpy(next.wifiPassword, doc["wifiPassword"] | "", sizeof(next.wifiPassword));
  if (doc.containsKey("captivePortal")) next.captivePortal = doc["captivePortal"] | true;
  if (doc.containsKey("httpAuth")) next.httpAuth = doc["httpAuth"] | false;
  if (doc.containsKey("httpUser"))
    strlcpy(next.httpUser, doc["httpUser"] | "", sizeof(next.httpUser));
  if (doc.containsKey("httpPassword"))
    strlcpy(next.httpPassword, doc["httpPassword"] | "", sizeof(next.httpPassword));
  if (doc.containsKey("irRepeat")) next.irRepeat = doc["irRepeat"] | 1;
  if (doc.containsKey("locale"))
    strlcpy(next.locale, doc["locale"] | "en", sizeof(next.locale));

  // Also accept nested keys, so a full backup can be restored as-is.
  if (doc["ap"].is<JsonObject>()) {
    if (doc["ap"].as<JsonObject>().containsKey("ssid"))
      strlcpy(next.apSsid, doc["ap"]["ssid"] | "", sizeof(next.apSsid));
    if (doc["ap"].as<JsonObject>().containsKey("password"))
      strlcpy(next.apPassword, doc["ap"]["password"] | "", sizeof(next.apPassword));
    if (doc["ap"].as<JsonObject>().containsKey("captivePortal"))
      next.captivePortal = doc["ap"]["captivePortal"] | true;
  }
  if (doc["wifi"].is<JsonObject>()) {
    if (doc["wifi"].as<JsonObject>().containsKey("ssid"))
      strlcpy(next.wifiSsid, doc["wifi"]["ssid"] | "", sizeof(next.wifiSsid));
    if (doc["wifi"].as<JsonObject>().containsKey("password"))
      strlcpy(next.wifiPassword, doc["wifi"]["password"] | "", sizeof(next.wifiPassword));
  }
  if (doc["auth"].is<JsonObject>()) {
    JsonObject auth = doc["auth"].as<JsonObject>();
    if (auth.containsKey("user"))
      strlcpy(next.httpUser, auth["user"] | "", sizeof(next.httpUser));
    if (auth.containsKey("password"))
      strlcpy(next.httpPassword, auth["password"] | "", sizeof(next.httpPassword));
    if (auth.containsKey("enabled")) next.httpAuth = auth["enabled"] | false;
  }
  if (doc["ir"].is<JsonObject>() && doc["ir"].as<JsonObject>().containsKey("repeat"))
    next.irRepeat = doc["ir"]["repeat"] | 1;

  String error = AppCfg::validate(next);
  if (error.length() > 0) {
    sendError(400, error.c_str());
    return;
  }
  if (!AppCfg::save(next)) {
    sendError(500, "config write failed");
    return;
  }

  bool restartRequired = strcmp(next.apSsid, gCfg.apSsid) != 0 ||
                         strcmp(next.apPassword, gCfg.apPassword) != 0 ||
                         strcmp(next.wifiSsid, gCfg.wifiSsid) != 0 ||
                         strcmp(next.wifiPassword, gCfg.wifiPassword) != 0 ||
                         next.httpAuth != gCfg.httpAuth ||
                         next.captivePortal != gCfg.captivePortal ||
                         strcmp(next.httpUser, gCfg.httpUser) != 0 ||
                         strcmp(next.httpPassword, gCfg.httpPassword) != 0;
  gCfg = next;
  sendOk("config saved", restartRequired);
}

static void handleSignalsGet() {
  if (!authOk()) return;
  String body = SignalStore::serialize();
  gServer.send(200, "application/json; charset=utf-8", body);
}

static void handleSignalsPut() {
  if (!authOk()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large");
    return;
  }
  if (!SignalStore::replace(body)) {
    sendError(400, "invalid signal database");
    return;
  }
  sendOk("signals saved");
}

static void handleSend() {
  if (!authOk()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large");
    return;
  }
  JsonDocument req;
  if (deserializeJson(req, body)) {
    sendError(400, "invalid JSON");
    return;
  }
  String profileId = req["profileId"] | "";
  String signalId = req["signalId"] | "";
  JsonObject profile;
  JsonObject signal;
  if (!SignalStore::findSignal(profileId.c_str(), signalId.c_str(), profile, signal)) {
    sendError(404, "signal not found");
    return;
  }
  uint8_t repeat = signal["repeat"] | gCfg.irRepeat;
  if (repeat == 0 || repeat > 10) repeat = gCfg.irRepeat;
  if (!transmitSignal(signal, repeat)) {
    sendError(400, "unsupported or invalid signal");
    return;
  }
  sendOk("signal sent");
}

static void handleLearnStart() {
  if (!authOk()) return;
  startLearning();
  sendOk("learning started");
}

static void handleLearnCancel() {
  if (!authOk()) return;
  cancelLearning();
  sendOk("learning cancelled");
}

static void handleLearnStatus() {
  if (!authOk()) return;
  JsonDocument doc;
  doc["active"] = gLearn.active;
  doc["ready"] = gLearn.ready;
  if (gLearn.ready) {
    JsonObject cap = doc.createNestedObject("capture");
    cap["kind"] = gLearn.kind;
    cap["protocol"] = gLearn.protocol;
    cap["hex"] = gLearn.hex;
    cap["bits"] = gLearn.bits;
    cap["khz"] = gLearn.khz;
    JsonArray timings = cap.createNestedArray("timings");
    for (uint8_t i = 0; i < gLearn.timingCount; ++i) timings.add(gLearn.timings[i]);
  }
  sendJson(200, doc);
}

static void handleStatus() {
  if (!authOk()) return;
  JsonDocument doc;
  doc["version"] = ROVIXANE_IR_FIRMWARE_VERSION;
  doc["product"] = ROVIXANE_IR_PRODUCT_NAME;
  doc["uptimeSec"] = static_cast<uint32_t>(millis() / 1000UL);
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["fsUsed"] = IhrStorage::usedBytes();
  doc["fsTotal"] = IhrStorage::totalBytes();
  String ip = gApMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
  doc["ip"] = ip;
  doc["apMode"] = gApMode;
  doc["apSsid"] = gCfg.apSsid;
  doc["wifiSsid"] = gCfg.wifiSsid;
  doc["wifiConnected"] = wifiIsConnected();
  doc["rssi"] = wifiIsConnected() ? WiFi.RSSI() : 0;
  doc["resetReason"] = ESP.getResetReason();
  doc["locale"] = gCfg.locale;
  doc["deviceName"] = gCfg.deviceName;
  doc["learning"] = gLearn.active;
  sendJson(200, doc);
}

static void handleRestart() {
  if (!authOk()) return;
  scheduleRestart(300);
  sendOk("restarting", true);
}

static void handleFactoryReset() {
  if (!authOk()) return;
  scheduleFactoryReset(300);
  sendOk("factory reset", true);
}

static void handleNotFound() {
  if (gCfg.httpAuth && !gServer.authenticate(gCfg.httpUser, gCfg.httpPassword)) {
    gServer.requestAuthentication();
    return;
  }
  String path = gServer.uri();
  if (path.startsWith("/api/")) {
    sendError(404, "not found");
    return;
  }
  // Captive-portal probes get a friendly redirect to the active dashboard.
  String target = gApMode ? IHR_AP_IP.toString()
                          : (wifiIsConnected() ? WiFi.localIP().toString()
                                               : IHR_AP_IP.toString());
  gServer.sendHeader("Location", "http://" + target + "/");
  gServer.send(302, "text/plain", "Redirecting to " + target);
}

static void setupRoutes() {
  gServer.on("/", HTTP_GET, handleRoot);
  gServer.on("/app.css", HTTP_GET, handleCss);
  gServer.on("/app.js", HTTP_GET, handleJs);
  gServer.on("/manifest.webmanifest", HTTP_GET, handleManifest);
  gServer.on("/api/config", HTTP_GET, handleConfigGet);
  gServer.on("/api/config", HTTP_POST, handleConfigPost);
  gServer.on("/api/signals", HTTP_GET, handleSignalsGet);
  gServer.on("/api/signals", HTTP_PUT, handleSignalsPut);
  gServer.on("/api/send", HTTP_POST, handleSend);
  gServer.on("/api/learn/start", HTTP_POST, handleLearnStart);
  gServer.on("/api/learn/status", HTTP_GET, handleLearnStatus);
  gServer.on("/api/learn/cancel", HTTP_POST, handleLearnCancel);
  gServer.on("/api/status", HTTP_GET, handleStatus);
  gServer.on("/api/restart", HTTP_POST, handleRestart);
  gServer.on("/api/factory-reset", HTTP_POST, handleFactoryReset);
  gServer.onNotFound(handleNotFound);
}

// ---------------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(IHR_PIN_LED, OUTPUT);
  pinMode(IHR_PIN_CFG_BTN, INPUT_PULLUP);
  ihrSetLed(false);
  delay(50);

  if (!IhrStorage::begin()) {
    Serial.println(F("[BOOT] LittleFS unavailable"));
  }
  if (!AppCfg::load(gCfg)) {
    AppCfg::seedDefaults(gCfg);
  }
  if (!SignalStore::begin()) {
    Serial.println(F("[BOOT] signal database started empty"));
  }
  memset(&gLearn, 0, sizeof(gLearn));
  gIrSend.begin();
  setupRoutes();
  startWifi();
  gServer.begin();

  Serial.printf("[BOOT] %s %s ready, IP %s\n", ROVIXANE_IR_DISPLAY_NAME,
                ROVIXANE_IR_FIRMWARE_VERSION,
                (gApMode ? WiFi.softAPIP() : WiFi.localIP()).toString().c_str());
}

void loop() {
  gServer.handleClient();
  if (gApMode && gCfg.captivePortal) gDns.processNextRequest();
  pollWifi();
  pollLearning();
  pollConfigButton();

  bool buttonHeld = gBtnWasDown && gBtnDownAt &&
                    (millis() - gBtnDownAt >= IHR_CFG_HOLD_MS);
  if (gLearn.active || buttonHeld) {
    ihrSetLed((millis() / 250) % 2 == 0);
  } else {
    ihrSetLed(false);
  }

  if ((gRestartPending || gFactoryPending) && millis() >= gActionAt) {
    if (gFactoryPending) {
      IhrStorage::remove(IHR_CONFIG_PATH);
      IhrStorage::remove(IHR_SIGNALS_PATH);
    }
    ESP.restart();
  }
  delay(2);
}
