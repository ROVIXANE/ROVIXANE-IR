#include "SignalStore.h"
#include "Compatibility.h"
#include "Storage.h"
#include "WebAssets.h"
#include <ArduinoJson.h>

namespace SignalStore {

static JsonDocument s_db;

static bool normalise(JsonVariant root) {
  if (!root.is<JsonObject>()) return false;
  JsonObject obj = root.as<JsonObject>();
  obj["format"] = "ihr-signals";
  obj["version"] = 1;

  if (!obj["profiles"].is<JsonArray>()) {
    obj.remove("profiles");
    obj.createNestedArray("profiles");
  }
  JsonArray profiles = obj["profiles"].as<JsonArray>();
  for (JsonObject p : profiles) {
    if (!p["id"] || p["id"].as<String>().length() == 0) {
      p["id"] = String("p_") + String(millis(), 16);
    }
    if (!p["name"] || p["name"].as<String>().length() == 0) p["name"] = "Remote";
    if (!p["signals"].is<JsonArray>()) {
      p.remove("signals");
      p.createNestedArray("signals");
    }
    JsonArray signals = p["signals"].as<JsonArray>();
    for (JsonObject s : signals) {
      if (!s["id"] || s["id"].as<String>().length() == 0) {
        s["id"] = String("s_") + String(millis(), 16);
      }
      if (!s["label"] || s["label"].as<String>().length() == 0) s["label"] = "Button";
      String kind = s["kind"] | "protocol";
      if (kind != "protocol" && kind != "raw" && kind != "ac") s["kind"] = "protocol";
      uint8_t repeat = s["repeat"] | 1;
      if (repeat == 0 || repeat > 10) repeat = 1;
      s["repeat"] = repeat;
    }
  }
  return true;
}

bool begin() {
  if (!IhrStorage::exists(IHR_SIGNALS_PATH)) {
    String starter(FPSTR(kStarterDevicesJson));
    if (!IhrStorage::writeText(IHR_SIGNALS_PATH, starter)) {
      Serial.println(F("[DB] failed to seed starter devices"));
      s_db.clear();
      normalise(s_db.to<JsonObject>());
      return false;
    }
  }
  if (!IhrStorage::readJson(IHR_SIGNALS_PATH, s_db)) {
    s_db.clear();
    normalise(s_db.to<JsonObject>());
    return false;
  }
  normalise(s_db);
  return true;
}

JsonDocument &doc() { return s_db; }

bool save() { return IhrStorage::writeJson(IHR_SIGNALS_PATH, s_db); }

bool replace(const String &json) {
  // Parse straight into the live document: no second JsonDocument or String
  // copy, which matters on the RAM-constrained ESP8266.
  s_db.clear();
  DeserializationError err = deserializeJson(s_db, json);
  if (err || !s_db.is<JsonObject>()) {
    s_db.clear();
    normalise(s_db.to<JsonObject>());
    return false;
  }
  normalise(s_db);
  return save();
}

String serialize() {
  String out;
  serializeJson(s_db, out);
  return out;
}

bool clear() {
  s_db.clear();
  JsonObject root = s_db.to<JsonObject>();
  root["format"] = "ihr-signals";
  root["version"] = 1;
  root.createNestedArray("profiles");
  return save();
}

bool findSignal(const char *profileId, const char *signalId,
                JsonObject &profile, JsonObject &signal) {
  JsonArray profiles = s_db["profiles"].as<JsonArray>();
  for (JsonObject p : profiles) {
    if (strcmp(p["id"] | "", profileId) != 0) continue;
    JsonArray signals = p["signals"].as<JsonArray>();
    for (JsonObject s : signals) {
      if (strcmp(s["id"] | "", signalId) == 0) {
        profile = p;
        signal = s;
        return true;
      }
    }
  }
  return false;
}

}  // namespace SignalStore
