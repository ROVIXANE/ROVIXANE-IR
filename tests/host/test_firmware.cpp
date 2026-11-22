// Host integration tests: run the REAL firmware sources against simulated
// Arduino / ESP8266 / IR libraries. No hardware is required.
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

static HttpResponse get(const char *path, const String &auth = String()) {
  return gServer.dispatch(HTTP_GET, path, String(), auth);
}
static HttpResponse post(const char *path, const char *body,
                         const String &auth = String()) {
  return gServer.dispatch(HTTP_POST, path, String(body), auth);
}
static HttpResponse put(const char *path, const char *body,
                        const String &auth = String()) {
  return gServer.dispatch(HTTP_PUT, path, String(body), auth);
}
static JsonDocument asJson(const HttpResponse &r) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, r.body);
  CHECK(!err, "response is JSON");
  return doc;
}

static void testWeb() {
  HttpResponse r = get("/");
  CHECK(r.code == 200, "dashboard returns 200");
  CHECK(r.body.indexOf("<!DOCTYPE html>") == 0, "dashboard is HTML");
  CHECK(r.body.indexOf("/app.css") >= 0, "dashboard links CSS");
  CHECK(r.body.indexOf("/app.js") >= 0, "dashboard links JS");
  CHECK(r.body.indexOf("/manifest.webmanifest") >= 0, "dashboard links PWA manifest");
  CHECK(r.body.length() == gServer.expectedLength(), "dashboard length announced");

  r = get("/manifest.webmanifest");
  CHECK(r.code == 200 && r.body.indexOf("ROVIXANE IR") >= 0,
        "PWA manifest is served with ROVIXANE branding");

  r = get("/app.css");
  CHECK(r.code == 200 && r.body.length() == kWebAppCssLen, "CSS is served fully");
  r = get("/app.js");
  CHECK(r.code == 200 && r.body.length() == kWebAppJsLen, "JS is served fully");
  CHECK(get("/api/does-not-exist").code == 404, "unknown API returns 404");
  CHECK(get("/anything-else").code == 302, "captive portal redirects unknown paths");
}

static void testConfig() {
  JsonDocument d = asJson(get("/api/config"));
  CHECK(d["deviceName"] == ROVIXANE_IR_DISPLAY_NAME, "default device name");
  CHECK(d["apSsid"] == ROVIXANE_IR_AP_SSID, "default AP SSID");
  CHECK(d["apPassword"] == "", "default AP is open");
  CHECK(d["locale"] == "en", "default locale is English");
  CHECK(d["httpAuth"] == false, "login disabled by default");
  CHECK(d["irRepeat"] == 1, "default repeat is 1");

  CHECK(post("/api/config", "{\"locale\":\"bn\"}").code == 200, "locale update accepted");
  d = asJson(get("/api/config"));
  CHECK(d["locale"] == "bn", "locale update persisted");
  CHECK(LittleFS.exists(IHR_CONFIG_PATH), "config.json written to LittleFS");

  String tooLong;
  for (int i = 0; i < 40; ++i) tooLong.concat('x');
  String bad = String("{\"apSsid\":\"") + tooLong + "\"}";
  CHECK(post("/api/config", bad.c_str()).code == 400, "invalid AP SSID rejected");

  CHECK(post("/api/config", "{\"locale\":\"en\"}").code == 200, "locale restored");
}

static void testSignalsAndSending() {
  JsonDocument d = asJson(get("/api/signals"));
  CHECK(d["format"] == "ihr-signals", "starter database format");
  CHECK(d["profiles"].size() == 1, "starter database has one profile");
  CHECK(d["profiles"][0]["id"] == "p_lg_tv_example", "starter profile id");
  CHECK(d["profiles"][0]["signals"].size() == 4, "starter profile has 4 buttons");
  CHECK(d["profiles"][0]["signals"][0]["protocol"] == "LG", "LG example is present");

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

  CHECK(post("/api/learn/cancel", "{}").code == 200, "learning cancelled");
  d = asJson(get("/api/learn/status"));
  CHECK(d["active"] == false && d["ready"] == false, "cancel clears capture");
}

static void testStatusAndNetwork() {
  JsonDocument d = asJson(get("/api/status"));
  CHECK(d["version"] == ROVIXANE_IR_FIRMWARE_VERSION, "firmware version in status");
  CHECK(d["product"] == ROVIXANE_IR_PRODUCT_NAME, "product name in status");
  CHECK(d["locale"] == "en", "locale in status");
  CHECK(d["fsTotal"] == 4u * 1024u * 1024u, "filesystem size reported");
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

static void testAuth() {
  const char *auth = "Basic YWRtaW46c2VjcmV0";  // admin:secret
  CHECK(post("/api/config", "{\"httpAuth\":true,\"httpUser\":\"admin\","
                            "\"httpPassword\":\"secret\"}").code == 200,
        "login enabled");
  CHECK(get("/api/config").code == 401, "config needs authentication");
  CHECK(get("/").code == 401, "dashboard needs authentication");
  CHECK(get("/api/config", auth).code == 200, "valid credentials accepted");
  CHECK(post("/api/config", "{\"httpAuth\":false}", auth).code == 200,
        "login disabled with credentials");
  CHECK(get("/api/config").code == 200, "dashboard open again");
}

static void testFactoryAndRestart() {
  CHECK(post("/api/factory-reset", "{}").code == 200, "factory reset accepted");
  hostMillis += 500;
  loop();
  CHECK(ESP.restarted, "device restarts after factory reset");
  CHECK(!LittleFS.exists(IHR_CONFIG_PATH), "config removed by factory reset");
  CHECK(!LittleFS.exists(IHR_SIGNALS_PATH), "signals removed by factory reset");
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
  testConfig();
  testSignalsAndSending();
  testLearning();
  testStatusAndNetwork();
  testAuth();
  testFactoryAndRestart();

  printf("%d checks, %d failures\n", gChecks, gFails);
  return gFails == 0 ? 0 : 1;
}
