// Host integration tests: run the REAL firmware sources against simulated
// Arduino / ESP8266 / IR / OTA libraries. No hardware is required.
#include "../../rovixane-ir.ino"

#include <cassert>
#include <cstdio>

static int gChecks = 0;
static int gFails = 0;
#define CHECK(cond, label)                                                     \
  do {                                                                         \
    ++gChecks;                                                                 \
    if (!(cond)) {                                                             \
      ++gFails;                                                                \
      fprintf(stderr, "  FAIL  %s  (line %d)\n", (label), __LINE__);           \
    }                                                                          \
  } while (0)

// admin:secretpass / admin:newsecret1 — real HTTP Basic credentials used by the
// simulated browser.
static String gAuth = "Basic YWRtaW46c2VjcmV0cGFzcw==";

static std::map<String, String> jsonHeaders() {
  std::map<String, String> h;
  h["X-ROVIXANE-CSRF"] = "1";
  h["Content-Type"] = "application/json";
  return h;
}
static std::map<String, String> uploadHeaders() {
  std::map<String, String> h;
  h["X-ROVIXANE-CSRF"] = "1";
  h["Content-Type"] = "multipart/form-data; boundary=test";
  return h;
}

static HttpResponse get(const char *path, const String &auth = gAuth) {
  return gServer.dispatch(HTTP_GET, path, String(), auth);
}
static HttpResponse post(const char *path, const char *body,
                         const String &auth = gAuth,
                         const std::map<String, String> &headers = jsonHeaders()) {
  return gServer.dispatch(HTTP_POST, path, String(body), auth, headers);
}
static HttpResponse put(const char *path, const char *body,
                        const String &auth = gAuth,
                        const std::map<String, String> &headers = jsonHeaders()) {
  return gServer.dispatch(HTTP_PUT, path, String(body), auth, headers);
}
static JsonDocument asJson(const HttpResponse &r) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, r.body);
  CHECK(!err, "response is JSON");
  return doc;
}

static void testWeb() {
  HttpResponse r = get("/", String());
  CHECK(r.code == 200, "dashboard returns 200 in setup mode");
  CHECK(r.body.indexOf("<!DOCTYPE html>") == 0, "dashboard is HTML");
  CHECK(r.body.indexOf("/app.css") >= 0, "dashboard links CSS");
  CHECK(r.body.indexOf("/app.js") >= 0, "dashboard links JS");
  CHECK(r.body.indexOf("/manifest.webmanifest") >= 0, "dashboard links PWA manifest");
  CHECK(r.body.length() == gServer.expectedLength(), "dashboard length announced");
  CHECK(r.headers.count("Content-Security-Policy") == 1, "dashboard sends a CSP");

  r = get("/manifest.webmanifest", String());
  CHECK(r.code == 200 && r.body.indexOf("ROVIXANE IR") >= 0,
        "PWA manifest is served with ROVIXANE branding");

  r = get("/app.css", String());
  CHECK(r.code == 200 && r.body.length() == kWebAppCssLen, "CSS is served fully");
  r = get("/app.js", String());
  CHECK(r.code == 200 && r.body.length() == kWebAppJsLen, "JS is served fully");
  CHECK(get("/api/does-not-exist", String()).code == 404, "unknown API returns 404");
  CHECK(get("/anything-else", String()).code == 302, "captive portal redirects unknown paths");
}

static void testSetupFlow() {
  JsonDocument d = asJson(get("/api/config", String()));
  CHECK(d["deviceName"] == ROVIXANE_IR_DISPLAY_NAME, "default device name");
  CHECK(d["apSsid"] == ROVIXANE_IR_AP_SSID, "default AP SSID");
  CHECK(d["httpAuth"] == true, "login is required by default");
  CHECK(d["setupRequired"] == true, "fresh device is in setup mode");
  CHECK(d["httpPasswordSet"] == false, "no admin password yet");
  CHECK(d["httpPassword"] == "", "empty password is not masked as set");

  CHECK(post("/api/config", "{\"locale\":\"bn\"}", String()).code == 403,
        "writes are blocked before setup");
  CHECK(post("/api/send", "{\"profileId\":\"x\",\"signalId\":\"y\"}", String()).code == 403,
        "sending is blocked before setup");
  CHECK(post("/api/setup", "{\"password\":\"secretpass\"}", String(), {}).code == 403,
        "setup requires the CSRF header");
  std::map<String, String> wrongType;
  wrongType["X-ROVIXANE-CSRF"] = "1";
  wrongType["Content-Type"] = "text/plain";
  CHECK(post("/api/setup", "{\"password\":\"secretpass\"}", String(), wrongType).code == 415,
        "setup requires JSON content type");
  CHECK(post("/api/setup", "{\"password\":\"short\"}", String()).code == 400,
        "setup rejects short passwords");
  CHECK(post("/api/setup", "{\"user\":\"admin\",\"password\":\"secretpass\"}", String()).code == 200,
        "setup creates an admin password");

  CHECK(get("/api/config", String()).code == 401, "config now requires login");
  CHECK(get("/api/config", "Basic YWRtaW46d3Jvbmc=").code == 401, "wrong login rejected");
  d = asJson(get("/api/config"));
  CHECK(d["setupRequired"] == false, "setup mode ended");
  CHECK(d["httpPassword"] == IHR_SECRET_MASK, "stored password is masked");
  CHECK(d["httpPasswordSet"] == true, "password presence is reported");
  CHECK(post("/api/setup", "{\"password\":\"anotherpass\"}").code == 409,
        "setup cannot run twice");
}

static void testConfigApi() {
  JsonDocument d = asJson(get("/api/config"));
  CHECK(d["locale"] == "en", "default locale is English");
  CHECK(d["irRepeat"] == 1, "default repeat is 1");
  CHECK(d["apPassword"] == "", "empty AP password is not masked");
  CHECK(d["wifiPassword"] == "", "empty WiFi password is not masked");

  CHECK(post("/api/config", "{\"locale\":\"bn\"}").code == 200, "locale update accepted");
  d = asJson(get("/api/config"));
  CHECK(d["locale"] == "bn", "locale update persisted");
  CHECK(LittleFS.exists(IHR_CONFIG_PATH), "config.json written to LittleFS");

  CHECK(post("/api/config", "{\"locale\":\"bn\"}", String()).code == 401,
        "config write needs login");
  CHECK(post("/api/config", "{\"locale\":\"bn\"}", gAuth, {}).code == 403,
        "config write requires CSRF header");
  std::map<String, String> wrongType;
  wrongType["X-ROVIXANE-CSRF"] = "1";
  wrongType["Content-Type"] = "text/plain";
  CHECK(post("/api/config", "{\"locale\":\"bn\"}", gAuth, wrongType).code == 415,
        "config write requires JSON");

  String tooLong;
  for (int i = 0; i < 40; ++i) tooLong.concat('x');
  String bad = String("{\"apSsid\":\"") + tooLong + "\"}";
  CHECK(post("/api/config", bad.c_str()).code == 400, "invalid AP SSID rejected");

  // A restored backup carries masked values; they must not erase secrets.
  CHECK(post("/api/config",
             "{\"apPassword\":\"********\",\"wifiPassword\":\"********\","
             "\"httpPassword\":\"********\"}").code == 200,
        "masked secrets are ignored on write");
  CHECK(get("/api/config").code == 200, "old credentials still work");

  HttpResponse changed = post("/api/config", "{\"httpPassword\":\"newsecret1\"}");
  CHECK(changed.code == 200, "password change accepted");
  JsonDocument changedDoc = asJson(changed);
  CHECK(changedDoc["restartRequired"] == true, "password change requires restart");
  CHECK(get("/api/config").code == 401, "old password no longer works");
  gAuth = "Basic YWRtaW46bmV3c2VjcmV0MQ==";
  CHECK(get("/api/config").code == 200, "new password works");

  CHECK(post("/api/config", "{\"httpAuth\":true,\"httpPassword\":\"\"}").code == 400,
        "enabled login rejects an empty password");

  CHECK(post("/api/config", "{\"locale\":\"en\"}").code == 200, "locale restored");
}

static void testSignalsAndSending() {
  JsonDocument d = asJson(get("/api/signals"));
  CHECK(d["format"] == "ihr-signals", "starter database format");
  CHECK(d["version"] == IHR_SIGNALS_VERSION, "database schema version");
  CHECK(d["profiles"].size() == 1, "starter database has one profile");
  CHECK(d["profiles"][0]["id"] == "p_lg_tv_example", "starter profile id");
  CHECK(d["profiles"][0]["category"] == "tv", "starter profile category");
  CHECK(d["profiles"][0]["signals"].size() == 4, "starter profile has 4 signals");
  CHECK(d["profiles"][0]["signals"][0]["protocol"] == "LG", "LG example is present");
  CHECK(d["profiles"][0]["signals"][0]["verified"] == false,
        "starter example is labelled unverified");
  CHECK(d["profiles"][0]["signals"][0]["value"] == "0x20DF10EF", "LG power code intact");

  IRsend::tx = IrTxRecord();
  CHECK(post("/api/send", "{\"profileId\":\"p_lg_tv_example\","
                          "\"signalId\":\"s_lg_power_example\"}").code == 200,
        "LG power code sent");
  CHECK(IRsend::tx.kind == "LG", "LG transmitter used");
  CHECK(IRsend::tx.value == 0x20DF10EFULL, "LG HEX value decoded correctly");
  CHECK(IRsend::tx.bits == 28, "LG bit length passed through");
  CHECK(IRsend::tx.calls == 1, "one transmission for repeat=1");

  CHECK(post("/api/send", "{\"profileId\":\"x\",\"signalId\":\"y\"}").code == 404,
        "unknown signal returns 404");

  const char *custom = R"({"format":"ihr-signals","version":1,"profiles":[
    {"id":"p1","name":"My TV","signals":[
      {"id":"s1","label":"Power","kind":"protocol","protocol":"NEC",
       "value":"0x00FF00FF","bits":32,"repeat":2},
      {"id":"s2","label":"Raw","kind":"raw","khz":38,"repeat":1,
       "timings":[9000,4500,560,560,560,1690,560]}
    ]}]})";
  HttpResponse putResult = put("/api/signals", custom);
  if (putResult.code != 200)
    fprintf(stderr, "  PUT body: %d %s\n", putResult.code, putResult.body.c_str());
  CHECK(putResult.code == 200, "custom database accepted");
  d = asJson(get("/api/signals"));
  CHECK(d["profiles"].size() == 1 && d["profiles"][0]["signals"].size() == 2,
        "custom database saved and reloaded");
  CHECK(d["profiles"][0]["category"] == "tv", "V1 category inferred from name");

  IRsend::tx = IrTxRecord();
  CHECK(post("/api/send", "{\"profileId\":\"p1\",\"signalId\":\"s1\"}").code == 200,
        "NEC button sent");
  CHECK(IRsend::tx.kind == "NEC" && IRsend::tx.value == 0x00FF00FFULL &&
        IRsend::tx.bits == 32 && IRsend::tx.calls == 2,
        "NEC frame and repeat count correct");

  IRsend::tx = IrTxRecord();
  CHECK(post("/api/send", "{\"profileId\":\"p1\",\"signalId\":\"s2\"}").code == 200,
        "raw button sent");
  CHECK(IRsend::tx.kind == "RAW" && IRsend::tx.khz == 38 && IRsend::tx.count == 7,
        "raw timings and carrier correct");
  CHECK(IRsend::tx.raw[0] == 9000 && IRsend::tx.raw[5] == 1690,
        "raw timing values intact");

  // Oversized/negative/zero values must never become a different IR frame.
  const char *overflowHex = R"({"format":"ihr-signals","profiles":[
    {"id":"pbad","signals":[
      {"id":"sbad","label":"Overflow","kind":"protocol","protocol":"NEC",
       "value":"0x10000000000000000","bits":64,"repeat":1}]}]})";
  CHECK(put("/api/signals", overflowHex).code == 200,
        "oversized HEX value stored for send-time validation");
  CHECK(post("/api/send", "{\"profileId\":\"pbad\",\"signalId\":\"sbad\"}").code == 400,
        "oversized HEX IR value rejected at send time");

  const char *negativeValue = R"({"format":"ihr-signals","profiles":[
    {"id":"pbad","signals":[
      {"id":"sbad","label":"Negative","kind":"protocol","protocol":"NEC",
       "value":-1,"bits":32,"repeat":1}]}]})";
  CHECK(put("/api/signals", negativeValue).code == 200,
        "negative protocol value stored for send-time validation");
  CHECK(post("/api/send", "{\"profileId\":\"pbad\",\"signalId\":\"sbad\"}").code == 400,
        "negative IR value rejected at send time");

  const char *zeroTiming = R"({"format":"ihr-signals","profiles":[
    {"id":"pbad","signals":[
      {"id":"sbad","label":"Zero","kind":"raw","khz":38,"timings":[9000,4500,0,560]}]}]})";
  CHECK(put("/api/signals", zeroTiming).code == 200,
        "zero raw timing stored for send-time validation");
  CHECK(post("/api/send", "{\"profileId\":\"pbad\",\"signalId\":\"sbad\"}").code == 400,
        "zero raw timing rejected instead of shifting the frame");

  const char *wideValid = R"({"format":"ihr-signals","profiles":[
    {"id":"pwide","signals":[
      {"id":"swide","label":"Wide","kind":"protocol","protocol":"NEC",
       "value":"0xFFFFFFFFFFFFFFFF","bits":64,"repeat":1}]}]})";
  CHECK(put("/api/signals", wideValid).code == 200, "full-width 64-bit database accepted");
  IRsend::tx = IrTxRecord();
  CHECK(post("/api/send", "{\"profileId\":\"pwide\",\"signalId\":\"swide\"}").code == 200,
        "full-width 64-bit code transmitted");
  CHECK(IRsend::tx.value == UINT64_MAX, "64-bit code remains intact");

  // Invalid replacements must never become the live database.
  const char *badRaw = R"({"format":"ihr-signals","profiles":[
    {"id":"p1","name":"Bad","signals":[
      {"id":"s1","label":"Bad raw","kind":"raw","timings":[9000,-1,560,560]}]}]})";
  CHECK(put("/api/signals", badRaw).code == 400, "negative raw timing rejected");
  d = asJson(get("/api/signals"));
  CHECK(d["profiles"].size() == 1 && d["profiles"][0]["id"] == "pwide"
        && d["profiles"][0]["signals"].size() == 1,
        "previous database kept after rejected write");
  CHECK(put("/api/signals", "{not json").code == 400, "malformed JSON rejected");
  d = asJson(get("/api/signals"));
  CHECK(d["profiles"].size() == 1, "database still intact after malformed write");

  String oversized = "{\"format\":\"ihr-signals\",\"profiles\":[],\"pad\":\"";
  for (int i = 0; i < static_cast<int>(IHR_MAX_API_BODY) + 100; ++i) oversized.concat('x');
  oversized.concat("\"}");
  CHECK(put("/api/signals", oversized.c_str()).code == 413,
        "oversized database rejected");

  CHECK(put("/api/signals", "{\"format\":\"ihr-signals\"}").code == 200,
        "empty database accepted and normalised");
  d = asJson(get("/api/signals"));
  CHECK(d["profiles"].size() == 0, "empty database has no profiles");
}

static void testLearning() {
  static uint16_t raw[] = {0, 4500, 280, 280, 280, 900};
  CHECK(post("/api/learn/start", "{}").code == 200, "learning started");
  CHECK(gLearn.active, "learning flag active");
  IRrecv::testFeed(NEC, 0x00FF00FFULL, 32, raw, 6);
  loop();
  CHECK(!gLearn.active && gLearn.ready, "capture stopped learning");
  JsonDocument d = asJson(get("/api/learn/status"));
  CHECK(d["active"] == false, "status reports learning stopped");
  CHECK(d["capture"]["protocol"] == "NEC", "learned protocol name");
  CHECK(d["capture"]["hex"] == "0x00FF00FF", "learned HEX value");
  CHECK(d["capture"]["bits"] == 32, "learned bit length");
  CHECK(d["capture"]["timings"][0] == 9000, "raw timings converted to microseconds");

  CHECK(post("/api/learn/start", "{}").code == 200, "second learning started");
  IRrecv::testFeed(UNKNOWN, 0, 0, raw, 6);
  loop();
  d = asJson(get("/api/learn/status"));
  CHECK(d["capture"]["kind"] == "raw", "unknown protocol captured as raw");
  CHECK(d["capture"]["protocol"] == "RAW", "raw capture label");
  CHECK(d["capture"]["timings"].size() == 5, "raw capture timings present");

  CHECK(post("/api/learn/start", "{}").code == 200, "third learning started");
  IRrecv::testFeed(UNKNOWN, 0, 0, raw, 6, true);
  loop();
  d = asJson(get("/api/learn/status"));
  CHECK(d["capture"]["kind"] == "error", "overflowed raw capture rejected visibly");
  CHECK(d["capture"]["overflow"] == true, "overflow flag reported");

  static uint16_t longRaw[IHR_MAX_RAW_TIMINGS + 2] = {};
  CHECK(post("/api/learn/start", "{}").code == 200, "long learning started");
  IRrecv::testFeed(UNKNOWN, 0, 0, longRaw, IHR_MAX_RAW_TIMINGS + 2);
  loop();
  d = asJson(get("/api/learn/status"));
  CHECK(d["capture"]["kind"] == "error", "truncated raw capture rejected");
  CHECK(d["capture"]["truncated"] == true, "truncation reported");

  CHECK(post("/api/learn/cancel", "{}").code == 200, "learning cancelled");
  d = asJson(get("/api/learn/status"));
  CHECK(d["active"] == false && d["ready"] == false, "cancel clears capture");

  CHECK(post("/api/learn/start", "{}", String()).code == 401,
        "learning needs authentication");
}

static void testStatusAndNetwork() {
  JsonDocument d = asJson(get("/api/status"));
  CHECK(d["version"] == ROVIXANE_IR_FIRMWARE_VERSION, "firmware version in status");
  CHECK(d["product"] == ROVIXANE_IR_PRODUCT_NAME, "product name in status");
  CHECK(d["locale"] == "en", "locale in status");
  CHECK(d["schemaVersion"] == IHR_SIGNALS_VERSION, "schema version in status");
  CHECK(d["fsTotal"] == 4u * 1024u * 1024u, "filesystem size reported");
  CHECK(d["otaSupported"] == true, "OTA slot reported");
  CHECK(gApMode == true, "AP starts when no WiFi credentials are saved");

  // Now simulate a device with saved credentials whose router is unavailable.
  strlcpy(gCfg.wifiSsid, "HomeRouter", sizeof(gCfg.wifiSsid));
  gApMode = false;
  WiFi.staConnected = false;
  startWifi();
  CHECK(!gApMode && WiFi.currentMode == WIFI_STA, "STA mode with saved credentials");
  hostMillis += 13000;
  loop();
  CHECK(gApMode, "AP fallback after failed WiFi connection");
  CHECK(gDns.started && gDns.dnsPort == IHR_DNS_PORT, "captive DNS started");
  CHECK(WIFI_AP_STA == WiFi.currentMode, "AP+STA mode keeps retrying WiFi");
  d = asJson(get("/api/status"));
  CHECK(d["apMode"] == true, "status reports AP mode");
  CHECK(d["ip"] == "192.168.4.1", "AP IP reported");
  CHECK(d["wifiSsid"] == "HomeRouter", "status reports configured WiFi SSID");
  CHECK(d["apSsid"] == ROVIXANE_IR_AP_SSID, "status reports config AP SSID");

  // Saved network returns: setup AP and captive DNS must be removed.
  WiFi.staConnected = true;
  loop();
  CHECK(!gApMode, "AP torn down after WiFi recovers");
  CHECK(!gDns.started, "captive DNS stopped after WiFi recovers");
  CHECK(WiFi.apDisconnects >= 1, "softAPdisconnect called");
  CHECK(WiFi.currentMode == WIFI_STA, "back to STA-only mode");
  d = asJson(get("/api/status"));
  CHECK(d["apMode"] == false, "status leaves AP mode after recovery");
  CHECK(d["ip"] == "192.168.1.42", "status reports STA IP after recovery");
}

static void testOta() {
  IhrOta::reset();
  ESP.restarted = false;

  std::map<String, String> headers = uploadHeaders();
  gServer.beginRequest(HTTP_POST, "/api/ota", gAuth, headers);
  const uint8_t image[] = {0xE9, 0x01, 0x02, 0x03};
  CHECK(gServer.feedUpload(UPLOAD_FILE_START, "firmware.bin", nullptr, 0, sizeof(image)),
        "OTA start routed");
  CHECK(IhrOta::state().active, "OTA writer started");
  CHECK(gServer.feedUpload(UPLOAD_FILE_WRITE, "firmware.bin", image, sizeof(image), sizeof(image)),
        "OTA chunk routed");
  CHECK(gServer.feedUpload(UPLOAD_FILE_END, "firmware.bin", nullptr, 0, sizeof(image)),
        "OTA end routed");
  CHECK(IhrOta::state().ok && IhrOta::state().done, "OTA image accepted");
  CHECK(IhrOta::state().written == sizeof(image), "OTA byte count tracked");

  HttpResponse r = post("/api/ota", "", gAuth, headers);
  CHECK(r.code == 200, "OTA completion acknowledged");
  JsonDocument d = asJson(r);
  CHECK(d["restartRequired"] == true, "OTA requires restart");
  CHECK(d["bytes"] == sizeof(image), "OTA byte count returned");
  hostMillis += 1500;
  loop();
  CHECK(ESP.restarted, "device restarts after verified OTA");

  // Bad magic byte: reject the upload visibly and do not commit.
  ESP.restarted = false;
  IhrOta::reset();
  const uint8_t badImage[] = {0x00, 0x01, 0x02, 0x03};
  gServer.beginRequest(HTTP_POST, "/api/ota", gAuth, headers);
  gServer.feedUpload(UPLOAD_FILE_START, "firmware.bin", nullptr, 0, sizeof(badImage));
  gServer.feedUpload(UPLOAD_FILE_WRITE, "firmware.bin", badImage, sizeof(badImage), sizeof(badImage));
  gServer.feedUpload(UPLOAD_FILE_END, "firmware.bin", nullptr, 0, sizeof(badImage));
  CHECK(!IhrOta::state().ok && IhrOta::state().done, "bad magic rejected");
  r = post("/api/ota", "", gAuth, headers);
  CHECK(r.code == 400, "bad firmware reported to the dashboard");
  d = asJson(r);
  CHECK(d["code"] == IHR_ERR_OTA, "bad firmware uses the ota error code");

  // A non-.bin upload is rejected before the flasher starts.
  IhrOta::reset();
  gServer.beginRequest(HTTP_POST, "/api/ota", gAuth, headers);
  gServer.feedUpload(UPLOAD_FILE_START, "firmware.txt", nullptr, 0, sizeof(image));
  CHECK(IhrOta::state().done && !IhrOta::state().ok, "non-bin file rejected");

  // Unauthenticated upload / missing CSRF must not reach the flasher.
  IhrOta::reset();
  std::map<String, String> noCsrf;
  noCsrf["Content-Type"] = "multipart/form-data; boundary=test";
  gServer.beginRequest(HTTP_POST, "/api/ota", gAuth, noCsrf);
  gServer.feedUpload(UPLOAD_FILE_START, "firmware.bin", nullptr, 0, sizeof(image));
  CHECK(IhrOta::state().done && !IhrOta::state().ok, "missing CSRF blocks OTA");
  CHECK(post("/api/ota", "", gAuth, noCsrf).code == 403, "OTA finalize requires CSRF");
  IhrOta::reset();
  gServer.beginRequest(HTTP_POST, "/api/ota", String(), uploadHeaders());
  gServer.feedUpload(UPLOAD_FILE_START, "firmware.bin", nullptr, 0, sizeof(image));
  CHECK(IhrOta::state().done && !IhrOta::state().ok, "unauthenticated OTA blocked");
  CHECK(post("/api/ota", "", String(), uploadHeaders()).code == 401,
        "OTA finalize requires login");
}

static void testRecoveryAndFactoryReset() {
  // Recreate a database, then simulate a torn main file with an intact .bak.
  CHECK(put("/api/signals", R"({"format":"ihr-signals","profiles":[
      {"id":"recover","name":"Recovered TV","category":"tv","signals":[
        {"id":"s1","label":"Power","protocol":"NEC","value":"0x00FF00FF","bits":32}]}]})").code == 200,
        "recovery fixture saved");
  CHECK(LittleFS.exists("/signals.json.bak"),
        "previous good database is retained as .bak");
  String backup;
  CHECK(IhrStorage::readText(IHR_SIGNALS_PATH, backup), "fixture readable");
  CHECK(IhrStorage::writeText("/signals.json.bak", backup), "backup fixture written");
  {
    File f = LittleFS.open(IHR_SIGNALS_PATH, "w");
    CHECK(static_cast<bool>(f), "corrupt fixture opened");
    f.print("{ truncated");
    f.close();
  }
  CHECK(SignalStore::begin(), "database recovers from .bak");
  CHECK(SignalStore::profileCount() == 1, "recovered profile count");
  JsonDocument d = asJson(get("/api/signals"));
  CHECK(d["profiles"][0]["id"] == "recover", "recovered profile content");

  CHECK(LittleFS.exists("/signals.json.corrupt"), "damaged primary quarantined");
  CHECK(put("/api/signals", R"({"profiles":[]})").code == 200, "write after recovery succeeds");
  JsonDocument recoveredBackup;
  CHECK(IhrStorage::readJson("/signals.json.bak", recoveredBackup), "recovered backup remains readable");
  CHECK(recoveredBackup["profiles"][0]["id"] == "recover", "recovered backup not replaced by corruption");

  ESP.restarted = false;
  CHECK(post("/api/factory-reset", "{}").code == 400, "factory reset needs confirmation");
  CHECK(post("/api/factory-reset", "{\"confirm\":\"RESET\"}").code == 200,
        "factory reset accepted");
  hostMillis += 500;
  loop();
  CHECK(ESP.restarted, "device restarts after factory reset");
  CHECK(!LittleFS.exists(IHR_CONFIG_PATH), "config removed by factory reset");
  CHECK(!LittleFS.exists(IHR_SIGNALS_PATH), "signals removed by factory reset");
  CHECK(!LittleFS.exists("/signals.json.bak"), "signals backup removed by factory reset");
  CHECK(!LittleFS.exists("/signals.json.corrupt"), "quarantine removed by factory reset");
}

static void testStorageFaults() {
  JsonDocument original;
  original["value"] = "original";
  CHECK(IhrStorage::writeJson("/fault.json", original), "fault fixture saved");
  JsonDocument replacement;
  replacement["value"] = "replacement";
  LittleFSFaults::writeLimit = 5;
  CHECK(!IhrStorage::writeJson("/fault.json", replacement), "partial JSON write rejected");
  LittleFSFaults::writeLimit = static_cast<size_t>(-1);
  JsonDocument loaded;
  CHECK(IhrStorage::readJson("/fault.json", loaded), "original after partial write readable");
  CHECK(loaded["value"] == "original", "partial write did not replace original");

  CHECK(IhrStorage::writeJson("/fault.json", replacement), "backup fixture created");
  File bad = LittleFS.open("/fault.json", "w");
  bad.print("{bad");
  bad.close();
  File quarantine = LittleFS.open("/fault.json.corrupt", "w");
  quarantine.print("previous damaged copy");
  quarantine.close();
  CHECK(!IhrStorage::readJson("/fault.json", loaded), "existing quarantine prevents unsafe recovery");
  CHECK(!IhrStorage::writeJson("/fault.json", replacement), "failed quarantine blocks writes");
  CHECK(IhrStorage::readJson("/fault.json.bak", loaded) && loaded["value"] == "original",
        "backup preserved after failed quarantine");
  CHECK(IhrStorage::remove("/fault.json"), "remove clears blocked fixture");
  CHECK(IhrStorage::writeJson("/fault.json", original), "fault fixture restored");
  LittleFSFaults::failMount = true;
  CHECK(!IhrStorage::begin(), "mount failure reported");
  CHECK(!LittleFS.autoFormat, "automatic formatting disabled before mount");
  LittleFSFaults::failMount = false;
  CHECK(LittleFS.exists("/fault.json"), "mount failure did not format storage");

  File config = LittleFS.open(IHR_CONFIG_PATH, "w");
  config.print("{ damaged");
  config.close();
  gStorageUnavailable = false;
  setup();
  CHECK(gStorageUnavailable, "unreadable configuration halts boot");
  String contents;
  CHECK(IhrStorage::readText(IHR_CONFIG_PATH, contents) && contents == "{ damaged",
        "unreadable configuration preserved");
}

int main() {
  LittleFSImpl::clearRoot();
  hostMillis = 0;
  WiFi.staConnected = true;
  IRrecv::testReset();
  IRsend::tx = IrTxRecord();

  printf("Running firmware integration tests...\n");
  setup();
  testWeb();
  testSetupFlow();
  testConfigApi();
  testSignalsAndSending();
  testLearning();
  testStatusAndNetwork();
  testOta();
  testRecoveryAndFactoryReset();
  testStorageFaults();

  printf("%d checks, %d failures\n", gChecks, gFails);
  return gFails == 0 ? 0 : 1;
}
