/*
  ROVIXANE IR V2 — Universal Infrared Remote
  Firmware for NodeMCU ESP8266 (ESP-12E)
  https://github.com/ROVIXANE/ROVIXANE-IR
  -----------------------------------------------------------------
  - Complete TV / AC / fan / set-top-box / custom remote UI.
  - Sends and learns IR signals (NEC / LG / Sony / Samsung / JVC /
    Panasonic / RC5 / RC6 / Denon / Coolix / Midea24 / raw).
  - Crash-safe /signals.json with .bak recovery and strict validation.
  - First-run admin password, masked secrets, CSRF-protected API.
  - Browser-based firmware update (streaming OTA) with validation.
  - Fully offline, mobile-first dashboard served from flash.
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
#include <Updater.h>

#include "WebAssets.h"
#include "Storage.h"
#include "AppConfig.h"
#include "SignalStore.h"
#include "IrEngine.h"
#include "OtaService.h"

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

static bool gStorageUnavailable = false;
static bool gApMode = false;
static bool gStaTrying = false;
static uint32_t gStaTryAt = 0;
static uint32_t gStaRetryAt = 0;

enum PendingAction { ACTION_NONE, ACTION_RESTART, ACTION_FACTORY };
static PendingAction gAction = ACTION_NONE;
static uint32_t gActionAt = 0;

static bool gBtnWasDown = false;
static uint32_t gBtnDownAt = 0;

struct LearnCapture {
  bool active;
  bool ready;
  bool overflow;
  bool truncated;
  bool unsupported;
  char kind[12];
  char protocol[24];
  char hex[20];
  char error[96];
  uint16_t bits;
  uint16_t khz;
  uint16_t timings[IHR_MAX_RAW_TIMINGS];
  uint8_t timingCount;
};
static LearnCapture gLearn;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------
static bool wifiIsConnected() { return WiFi.status() == WL_CONNECTED; }
static bool setupMode() { return AppCfg::setupRequired(gCfg); }

static void setSecret(JsonObject obj, const char *key, const char *value) {
  obj[key] = value[0] ? IHR_SECRET_MASK : "";
}

static bool textTooLong(const char *text, size_t maxLength) {
  return text != nullptr && strlen(text) > maxLength;
}

static void scheduleAction(PendingAction action, uint32_t delayMs) {
  gAction = action;
  gActionAt = millis() + delayMs;
}

// ---------------------------------------------------------------------------
// IR learning
// ---------------------------------------------------------------------------
static void startLearning() {
  if (gLearn.active) return;
  memset(&gLearn, 0, sizeof(gLearn));
  gLearn.active = true;
  gIrRecv.enableIRIn();
}

static void cancelLearning() {
  gLearn.active = false;
  gLearn.ready = false;
}

static void copyCaptureTimings(decode_results &result) {
  gLearn.timingCount = 0;
  if (result.rawbuf == nullptr || result.rawlen <= 1) return;
  uint16_t maxIndex = result.rawlen - 1;
  if (maxIndex > IHR_MAX_RAW_TIMINGS) {
    maxIndex = IHR_MAX_RAW_TIMINGS;
    gLearn.truncated = true;
  }
  for (uint16_t i = 1; i <= maxIndex; ++i) {
    uint32_t us = static_cast<uint32_t>(result.rawbuf[i]) * kRawTick;
    gLearn.timings[gLearn.timingCount++] =
        static_cast<uint16_t>(us > 65535 ? 65535 : us);
  }
}

static void pollLearning() {
  if (!gLearn.active) return;
  decode_results result;
  if (!gIrRecv.decode(&result)) return;

  gLearn.active = false;
  gLearn.ready = true;
  gLearn.overflow = result.overflow;
  copyCaptureTimings(result);

  if (result.decode_type != UNKNOWN && result.bits > 0 &&
      IhrIr::isSendableProtocol(result.decode_type)) {
    strlcpy(gLearn.kind, "protocol", sizeof(gLearn.kind));
    strlcpy(gLearn.protocol, IhrIr::protocolName(result.decode_type),
            sizeof(gLearn.protocol));
    uint16_t digits = static_cast<uint16_t>(((result.bits + 7) / 8) * 2);
    if (digits > 16) digits = 16;
    String hex = "0x";
    hex.reserve(20);
    IhrIr::appendHex64(hex, result.value, digits);
    strlcpy(gLearn.hex, hex.c_str(), sizeof(gLearn.hex));
    gLearn.bits = result.bits > 64 ? 64 : result.bits;
    gLearn.khz = 38;
    if (gLearn.overflow) {
      strlcpy(gLearn.error, "capture buffer overflow; decoded value is usable",
              sizeof(gLearn.error));
    }
  } else if (gLearn.timingCount >= 4 && !gLearn.overflow && !gLearn.truncated) {
    // Either the decoder did not recognise the frame, or this build can decode
    // the protocol but has no matching sender. Preserve the raw frame.
    strlcpy(gLearn.kind, "raw", sizeof(gLearn.kind));
    strlcpy(gLearn.protocol, "RAW", sizeof(gLearn.protocol));
    gLearn.hex[0] = '\0';
    gLearn.bits = 0;
    gLearn.khz = 38;
    gLearn.unsupported = (result.decode_type != UNKNOWN);
    if (gLearn.unsupported)
      strlcpy(gLearn.error, "decoded protocol is not enabled for sending; raw data kept",
              sizeof(gLearn.error));
  } else {
    strlcpy(gLearn.kind, "error", sizeof(gLearn.kind));
    strlcpy(gLearn.protocol, "UNKNOWN", sizeof(gLearn.protocol));
    strlcpy(gLearn.error,
            (gLearn.overflow || gLearn.truncated) ? "capture incomplete; try again"
                            : "no usable IR signal captured",
            sizeof(gLearn.error));
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
  WiFi.mode(WIFI_STA);
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
    scheduleAction(ACTION_FACTORY, 200);
  }
  gBtnWasDown = down;
}

// ---------------------------------------------------------------------------
// HTTP helpers: auth, CSRF, responses
// ---------------------------------------------------------------------------
static bool csrfOk() {
  return gServer.header(String(IHR_CSRF_HEADER)) == IHR_CSRF_VALUE;
}

static bool jsonContentType() {
  String type = gServer.header(String("Content-Type"));
  return type.startsWith("application/json");
}

static bool authRead() {
  if (!gCfg.httpAuth) return true;
  if (setupMode()) return true;  // only the setup UI + read-only data is visible
  if (gServer.authenticate(gCfg.httpUser, gCfg.httpPassword)) return true;
  gServer.requestAuthentication();
  return false;
}

static void sendError(int code, const char *message,
                      const char *errorCode = nullptr);

static bool authWrite() {
  if (!gCfg.httpAuth) return true;
  if (setupMode()) {
    sendError(403, "first-run setup required", IHR_ERR_SETUP_REQUIRED);
    return false;
  }
  if (gServer.authenticate(gCfg.httpUser, gCfg.httpPassword)) return true;
  gServer.requestAuthentication();
  return false;
}

static void sendJson(int code, JsonDocument &doc) {
  gServer.sendHeader("Cache-Control", "no-store");
  gServer.sendHeader("X-Content-Type-Options", "nosniff");
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

static void sendError(int code, const char *message, const char *errorCode) {
  JsonDocument doc;
  doc["ok"] = false;
  doc["error"] = message;
  if (errorCode != nullptr) doc["code"] = errorCode;
  sendJson(code, doc);
}

static void sendProgmem(const char *data, size_t length, const char *type,
                        bool cacheable) {
  gServer.sendHeader("X-Content-Type-Options", "nosniff");
  if (cacheable) {
    // Revalidate so a dashboard loaded before an OTA never keeps stale JS/CSS.
    gServer.sendHeader("Cache-Control", "no-cache");
  } else {
    gServer.sendHeader("Cache-Control", "no-store");
    gServer.sendHeader("X-Frame-Options", "DENY");
    gServer.sendHeader("Referrer-Policy", "no-referrer");
    gServer.sendHeader(
        "Content-Security-Policy",
        "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; "
        "img-src 'self' data:; connect-src 'self'; frame-ancestors 'none'");
  }
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

// Conservative header checks for every state-changing JSON route.
static bool mutationGuard() {
  if (!authWrite()) return false;
  if (!csrfOk()) {
    sendError(403, "missing CSRF header", IHR_ERR_CSRF);
    return false;
  }
  if (!jsonContentType()) {
    sendError(415, "expected application/json", IHR_ERR_CONTENT_TYPE);
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Configuration API
// ---------------------------------------------------------------------------
static void fillConfigDoc(JsonDocument &doc, const AppConfig &cfg) {
  doc["version"] = ROVIXANE_IR_FIRMWARE_VERSION;
  doc["product"] = ROVIXANE_IR_PRODUCT_NAME;
  doc["build"] = String(__DATE__) + " " + String(__TIME__);
  doc["deviceName"] = cfg.deviceName;
  doc["setupRequired"] = AppCfg::setupRequired(cfg);

  doc["ap"]["ssid"] = cfg.apSsid;
  setSecret(doc["ap"].as<JsonObject>(), "password", cfg.apPassword);
  doc["ap"]["captivePortal"] = cfg.captivePortal;
  doc["wifi"]["ssid"] = cfg.wifiSsid;
  setSecret(doc["wifi"].as<JsonObject>(), "password", cfg.wifiPassword);
  doc["auth"]["user"] = cfg.httpUser;
  setSecret(doc["auth"].as<JsonObject>(), "password", cfg.httpPassword);
  doc["auth"]["enabled"] = cfg.httpAuth;
  doc["locale"] = cfg.locale;
  doc["ir"]["repeat"] = cfg.irRepeat;

  // Flat aliases are kept for backup/restore compatibility — masked, never raw.
  doc["apSsid"] = cfg.apSsid;
  setSecret(doc.as<JsonObject>(), "apPassword", cfg.apPassword);
  doc["wifiSsid"] = cfg.wifiSsid;
  setSecret(doc.as<JsonObject>(), "wifiPassword", cfg.wifiPassword);
  doc["captivePortal"] = cfg.captivePortal;
  doc["httpAuth"] = cfg.httpAuth;
  doc["httpUser"] = cfg.httpUser;
  setSecret(doc.as<JsonObject>(), "httpPassword", cfg.httpPassword);
  doc["irRepeat"] = cfg.irRepeat;

  doc["apPasswordSet"] = cfg.apPassword[0] != '\0';
  doc["wifiPasswordSet"] = cfg.wifiPassword[0] != '\0';
  doc["httpPasswordSet"] = cfg.httpPassword[0] != '\0';
  doc["otaSupported"] = IhrOta::available();
}

static void handleConfigGet() {
  if (!authRead()) return;
  JsonDocument doc;
  fillConfigDoc(doc, gCfg);
  sendJson(200, doc);
}

// Returns true when a flat or nested string field is present. `value` points
// into the document and is only valid while `doc` is alive.
static bool readStringField(JsonObject doc, const char *flat, JsonObject nested,
                            const char *nestedKey, const char *&value) {
  if (nested && nested.containsKey(nestedKey)) {
    value = nested[nestedKey] | "";
    return true;
  }
  if (doc.containsKey(flat)) {
    value = doc[flat] | "";
    return true;
  }
  return false;
}

static bool readBoolField(JsonObject doc, const char *flat, JsonObject nested,
                          const char *nestedKey, bool &value) {
  if (nested && nested[nestedKey].is<bool>()) {
    value = nested[nestedKey] | false;
    return true;
  }
  if (doc[flat].is<bool>()) {
    value = doc[flat] | false;
    return true;
  }
  return false;
}

static bool readIntField(JsonObject doc, const char *flat, JsonObject nested,
                         const char *nestedKey, int &value) {
  if (nested && nested[nestedKey].is<int>()) {
    value = nested[nestedKey] | 0;
    return true;
  }
  if (doc[flat].is<int>()) {
    value = doc[flat] | 0;
    return true;
  }
  return false;
}

static void handleConfigPost() {
  if (!mutationGuard()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large", IHR_ERR_VALIDATION);
    return;
  }
  JsonDocument doc;
  if (deserializeJson(doc, body) || !doc.is<JsonObject>()) {
    sendError(400, "invalid JSON", IHR_ERR_VALIDATION);
    return;
  }
  JsonObject root = doc.as<JsonObject>();
  JsonObject ap = root["ap"].is<JsonObject>() ? root["ap"].as<JsonObject>() : JsonObject();
  JsonObject wifi = root["wifi"].is<JsonObject>() ? root["wifi"].as<JsonObject>() : JsonObject();
  JsonObject auth = root["auth"].is<JsonObject>() ? root["auth"].as<JsonObject>() : JsonObject();
  JsonObject ir = root["ir"].is<JsonObject>() ? root["ir"].as<JsonObject>() : JsonObject();

  AppConfig next = gCfg;
  const char *text = nullptr;

  if (readStringField(root, "deviceName", JsonObject(), "", text)) {
    if (textTooLong(text, 31)) { sendError(400, "device name is too long", IHR_ERR_VALIDATION); return; }
    strlcpy(next.deviceName, text, sizeof(next.deviceName));
  }
  if (readStringField(root, "apSsid", ap, "ssid", text)) {
    if (textTooLong(text, 32)) { sendError(400, "AP SSID is too long", IHR_ERR_VALIDATION); return; }
    strlcpy(next.apSsid, text, sizeof(next.apSsid));
  }
  if (readStringField(root, "wifiSsid", wifi, "ssid", text)) {
    if (textTooLong(text, 32)) { sendError(400, "WiFi SSID is too long", IHR_ERR_VALIDATION); return; }
    strlcpy(next.wifiSsid, text, sizeof(next.wifiSsid));
  }
  if (readStringField(root, "httpUser", auth, "user", text)) {
    if (textTooLong(text, 32)) { sendError(400, "login user is too long", IHR_ERR_VALIDATION); return; }
    strlcpy(next.httpUser, text, sizeof(next.httpUser));
  }
  if (readStringField(root, "locale", JsonObject(), "", text)) {
    if (textTooLong(text, 7)) { sendError(400, "locale is too long", IHR_ERR_VALIDATION); return; }
    strlcpy(next.locale, text, sizeof(next.locale));
  }

  bool flag = false;
  if (readBoolField(root, "captivePortal", ap, "captivePortal", flag))
    next.captivePortal = flag;
  if (readBoolField(root, "httpAuth", auth, "enabled", flag))
    next.httpAuth = flag;

  int number = 0;
  if (readIntField(root, "irRepeat", ir, "repeat", number))
    next.irRepeat = static_cast<uint8_t>(number);

  // Secrets: the sentinel keeps the current value, so a restored (masked)
  // backup can never accidentally erase a password.
  const char *secret = nullptr;
  if (readStringField(root, "apPassword", ap, "password", secret)) {
    if (strcmp(secret, IHR_SECRET_MASK) != 0) {
      if (textTooLong(secret, 63)) { sendError(400, "AP password is too long", IHR_ERR_VALIDATION); return; }
      strlcpy(next.apPassword, secret, sizeof(next.apPassword));
    }
  }
  if (readStringField(root, "wifiPassword", wifi, "password", secret)) {
    if (strcmp(secret, IHR_SECRET_MASK) != 0) {
      if (textTooLong(secret, 64)) { sendError(400, "WiFi password is too long", IHR_ERR_VALIDATION); return; }
      strlcpy(next.wifiPassword, secret, sizeof(next.wifiPassword));
    }
  }
  if (readStringField(root, "httpPassword", auth, "password", secret)) {
    if (strcmp(secret, IHR_SECRET_MASK) != 0) {
      if (textTooLong(secret, 64)) { sendError(400, "login password is too long", IHR_ERR_VALIDATION); return; }
      strlcpy(next.httpPassword, secret, sizeof(next.httpPassword));
    }
  }

  String error = AppCfg::validate(next);
  if (error.length() > 0) {
    sendError(400, error.c_str(), IHR_ERR_VALIDATION);
    return;
  }
  if (!AppCfg::save(next)) {
    sendError(500, "config write failed", IHR_ERR_STORAGE);
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

// First-run setup: only usable while no admin password exists.
static void handleSetup() {
  if (!setupMode()) {
    sendError(409, "device is already configured", IHR_ERR_VALIDATION);
    return;
  }
  if (!csrfOk()) {
    sendError(403, "missing CSRF header", IHR_ERR_CSRF);
    return;
  }
  if (!jsonContentType()) {
    sendError(415, "expected application/json", IHR_ERR_CONTENT_TYPE);
    return;
  }
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large", IHR_ERR_VALIDATION);
    return;
  }
  JsonDocument doc;
  if (deserializeJson(doc, body) || !doc.is<JsonObject>()) {
    sendError(400, "invalid JSON", IHR_ERR_VALIDATION);
    return;
  }
  const char *password = doc["password"] | "";
  const char *user = doc["user"] | "admin";
  if (strlen(password) < IHR_MIN_HTTP_PASSWORD || strlen(password) > 64) {
    sendError(400, "password must be 8-64 characters", IHR_ERR_VALIDATION);
    return;
  }
  if (strlen(user) == 0 || strlen(user) > 32) {
    sendError(400, "user must be 1-32 characters", IHR_ERR_VALIDATION);
    return;
  }
  AppConfig next = gCfg;
  strlcpy(next.httpUser, user, sizeof(next.httpUser));
  strlcpy(next.httpPassword, password, sizeof(next.httpPassword));
  next.httpAuth = true;
  if (!AppCfg::save(next)) {
    sendError(500, "config write failed", IHR_ERR_STORAGE);
    return;
  }
  gCfg = next;
  sendOk("setup complete", false);
}

// ---------------------------------------------------------------------------
// Signals, sending and learning API
// ---------------------------------------------------------------------------
static void handleSignalsGet() {
  if (!authRead()) return;
  gServer.sendHeader("Cache-Control", "no-store");
  gServer.sendHeader("X-Content-Type-Options", "nosniff");
  gServer.setContentLength(SignalStore::jsonSize());
  gServer.send(200, "application/json; charset=utf-8", "");
  SignalStore::serializeTo(gServer.client());
}

static void handleSignalsPut() {
  if (!mutationGuard()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large", IHR_ERR_VALIDATION);
    return;
  }
  if (!SignalStore::replace(body)) {
    sendError(400, "invalid signal database; existing data was kept", IHR_ERR_VALIDATION);
    return;
  }
  sendOk("signals saved");
}

static void handleSend() {
  if (!mutationGuard()) return;
  String body = gServer.arg("plain");
  if (body.length() > IHR_MAX_API_BODY) {
    sendError(413, "payload too large", IHR_ERR_VALIDATION);
    return;
  }
  JsonDocument req;
  if (deserializeJson(req, body) || !req.is<JsonObject>()) {
    sendError(400, "invalid JSON", IHR_ERR_VALIDATION);
    return;
  }
  String profileId = req["profileId"] | "";
  String signalId = req["signalId"] | "";
  JsonObject profile;
  JsonObject signal;
  if (!SignalStore::findSignal(profileId.c_str(), signalId.c_str(), profile, signal)) {
    sendError(404, "signal not found", "not_found");
    return;
  }
  String error;
  if (!IhrIr::transmit(gIrSend, signal, gCfg.irRepeat, error)) {
    sendError(400, error.c_str(), IHR_ERR_UNSUPPORTED);
    return;
  }
  sendOk("signal sent");
}

static void handleLearnStart() {
  if (!mutationGuard()) return;
  startLearning();
  sendOk("learning started");
}

static void handleLearnCancel() {
  if (!mutationGuard()) return;
  cancelLearning();
  sendOk("learning cancelled");
}

static void handleLearnStatus() {
  if (!authRead()) return;
  JsonDocument doc;
  doc["active"] = gLearn.active;
  doc["ready"] = gLearn.ready;
  if (gLearn.ready) {
    JsonObject cap = doc["capture"].to<JsonObject>();
    cap["kind"] = gLearn.kind;
    cap["protocol"] = gLearn.protocol;
    cap["hex"] = gLearn.hex;
    cap["bits"] = gLearn.bits;
    cap["khz"] = gLearn.khz;
    cap["overflow"] = gLearn.overflow;
    cap["truncated"] = gLearn.truncated;
    cap["unsupported"] = gLearn.unsupported;
    if (gLearn.error[0]) cap["error"] = gLearn.error;
    JsonArray timings = cap["timings"].to<JsonArray>();
    for (uint8_t i = 0; i < gLearn.timingCount; ++i) timings.add(gLearn.timings[i]);
  }
  sendJson(200, doc);
}

// ---------------------------------------------------------------------------
// Status / lifecycle API
// ---------------------------------------------------------------------------
static void handleStatus() {
  if (!authRead()) return;
  JsonDocument doc;
  doc["version"] = ROVIXANE_IR_FIRMWARE_VERSION;
  doc["product"] = ROVIXANE_IR_PRODUCT_NAME;
  doc["build"] = String(__DATE__) + " " + String(__TIME__);
  doc["uptimeSec"] = static_cast<uint32_t>(millis() / 1000UL);
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["fsUsed"] = IhrStorage::usedBytes();
  doc["fsTotal"] = IhrStorage::totalBytes();
  doc["signalBytes"] = IhrStorage::fileSize(IHR_SIGNALS_PATH);
  doc["configBytes"] = IhrStorage::fileSize(IHR_CONFIG_PATH);
  doc["profileCount"] = SignalStore::profileCount();
  doc["signalCount"] = SignalStore::signalCount();
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
  doc["setupRequired"] = setupMode();
  doc["otaSupported"] = IhrOta::available();
  doc["sketchSize"] = ESP.getSketchSize();
  doc["freeSketchSpace"] = ESP.getFreeSketchSpace();
  doc["schemaVersion"] = IHR_SIGNALS_VERSION;
  sendJson(200, doc);
}

static void handleRestart() {
  if (!mutationGuard()) return;
  scheduleAction(ACTION_RESTART, 300);
  sendOk("restarting", true);
}

static void handleFactoryReset() {
  if (!mutationGuard()) return;
  String body = gServer.arg("plain");
  JsonDocument req;
  if (deserializeJson(req, body) || !req.is<JsonObject>()) {
    sendError(400, "invalid JSON", IHR_ERR_VALIDATION);
    return;
  }
  const char *confirm = req["confirm"] | "";
  if (strcmp(confirm, "RESET") != 0) {
    sendError(400, "factory reset requires {\"confirm\":\"RESET\"}",
              IHR_ERR_VALIDATION);
    return;
  }
  scheduleAction(ACTION_FACTORY, 300);
  sendOk("factory reset", true);
}

// ---------------------------------------------------------------------------
// Firmware update (OTA)
// ---------------------------------------------------------------------------
static bool otaRequestAllowed() {
  if (setupMode()) return false;
  if (!csrfOk()) return false;
  if (!gCfg.httpAuth) return true;
  return gServer.authenticate(gCfg.httpUser, gCfg.httpPassword);
}

static void handleOtaUpload() {
  IhrOta::handleUpload(gServer, otaRequestAllowed());
}

static void handleOtaFinish() {
  if (!authWrite()) return;
  if (!csrfOk()) {
    sendError(403, "missing CSRF header", IHR_ERR_CSRF);
    return;
  }
  IhrOta::State &s = IhrOta::state();
  if (!s.done) {
    sendError(400, "firmware upload did not complete", IHR_ERR_OTA);
    return;
  }
  if (!s.ok) {
    sendError(400, s.error.length() ? s.error.c_str() : "firmware update failed",
              IHR_ERR_OTA);
    return;
  }
  JsonDocument doc;
  doc["ok"] = true;
  doc["message"] = "firmware upload accepted";
  doc["restartRequired"] = true;
  doc["bytes"] = static_cast<uint32_t>(s.written);
  doc["filename"] = s.filename;
  sendJson(200, doc);
  scheduleAction(ACTION_RESTART, 1200);
}

// ---------------------------------------------------------------------------
// Routing
// ---------------------------------------------------------------------------
static void handleRoot() {
  if (!authRead()) return;
  sendProgmem(kWebIndexHtml, kWebIndexHtmlLen, "text/html; charset=utf-8", false);
}
static void handleCss() {
  if (!authRead()) return;
  sendProgmem(kWebAppCss, kWebAppCssLen, "text/css; charset=utf-8", true);
}
static void handleJs() {
  if (!authRead()) return;
  sendProgmem(kWebAppJs, kWebAppJsLen, "application/javascript; charset=utf-8", true);
}
static void handleManifest() {
  if (!authRead()) return;
  sendProgmem(kWebManifest, kWebManifestLen, "application/manifest+json; charset=utf-8", true);
}

static void handleNotFound() {
  if (!authRead()) return;
  String path = gServer.uri();
  if (path.startsWith("/api/")) {
    sendError(404, "not found", "not_found");
    return;
  }
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
  gServer.on("/api/setup", HTTP_POST, handleSetup);
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
  gServer.on("/api/ota", HTTP_POST, handleOtaFinish, handleOtaUpload);
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
    Serial.println(F("[BOOT] LittleFS unavailable. To erase and initialize it, hold the config button for 3 seconds now."));
    uint32_t heldAt = millis();
    while (digitalRead(IHR_PIN_CFG_BTN) == LOW && millis() - heldAt < IHR_CFG_HOLD_MS) delay(10);
    if (digitalRead(IHR_PIN_CFG_BTN) != LOW || millis() - heldAt < IHR_CFG_HOLD_MS ||
        !IhrStorage::formatAndBegin()) {
      Serial.println(F("[BOOT] storage not initialized; halted to preserve data"));
      gStorageUnavailable = true;
      return;
    }
    Serial.println(F("[BOOT] storage explicitly formatted"));
  }
  if (!AppCfg::load(gCfg)) {
    String configBackup = String(IHR_CONFIG_PATH) + ".bak";
    if (IhrStorage::exists(IHR_CONFIG_PATH) || IhrStorage::exists(configBackup.c_str())) {
      Serial.println(F("[BOOT] configuration unreadable; halted to preserve data"));
      gStorageUnavailable = true;
      return;
    }
    if (!AppCfg::seedDefaults(gCfg)) {
      Serial.println(F("[BOOT] cannot save initial configuration; halted"));
      gStorageUnavailable = true;
      return;
    }
  }
  if (!SignalStore::begin()) {
    Serial.println(F("[BOOT] signal database started empty or recovered"));
  }
  memset(&gLearn, 0, sizeof(gLearn));
  gIrSend.begin();
  setupRoutes();
  startWifi();
  gServer.begin();

  Serial.printf("[BOOT] %s %s ready on %s\n", ROVIXANE_IR_DISPLAY_NAME,
                ROVIXANE_IR_FIRMWARE_VERSION,
                (gApMode ? WiFi.softAPIP() : WiFi.localIP()).toString().c_str());
  if (setupMode()) {
    Serial.println(F("[BOOT] first-run setup required: open the dashboard and create an admin password"));
  }
}

void loop() {
  if (gStorageUnavailable) { delay(100); return; }
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

  if (gAction != ACTION_NONE && ihrTimeReached(millis(), gActionAt)) {
    if (gAction == ACTION_FACTORY) {
      IhrStorage::remove(IHR_CONFIG_PATH);
      IhrStorage::remove(IHR_SIGNALS_PATH);
    }
    gAction = ACTION_NONE;
    ESP.restart();
  }
  delay(2);
}
