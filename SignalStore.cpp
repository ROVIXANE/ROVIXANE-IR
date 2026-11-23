#include "SignalStore.h"
#include "Compatibility.h"
#include "Storage.h"
#include "WebAssets.h"
#include <ArduinoJson.h>

namespace SignalStore {

static JsonDocument s_db;

static bool isCategory(const char *c) {
  return c != nullptr &&
         (strcmp(c, "tv") == 0 || strcmp(c, "ac") == 0 ||
          strcmp(c, "fan") == 0 || strcmp(c, "stb") == 0 ||
          strcmp(c, "custom") == 0);
}

static const char *inferCategory(const char *name) {
  if (name == nullptr) return "custom";
  // Case-insensitive substring check without allocating.
  char lower[48];
  size_t n = strlen(name);
  if (n >= sizeof(lower)) n = sizeof(lower) - 1;
  for (size_t i = 0; i < n; ++i) {
    char c = name[i];
    lower[i] = (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
  }
  lower[n] = '\0';
  if (strstr(lower, "tv") || strstr(lower, "television") || strstr(lower, "টিভি"))
    return "tv";
  if (strstr(lower, "ac") || strstr(lower, "air") || strstr(lower, "এসি") ||
      strstr(lower, "conditioner"))
    return "ac";
  if (strstr(lower, "fan") || strstr(lower, "ফ্যান")) return "fan";
  if (strstr(lower, "box") || strstr(lower, "stb") || strstr(lower, "set top") ||
      strstr(lower, "সেটটপ"))
    return "stb";
  return "custom";
}

static void makeId(char *out, size_t cap, const char *prefix, size_t index) {
  snprintf(out, cap, "%s_%08lx_%02u", prefix,
           static_cast<unsigned long>(millis()), static_cast<unsigned>(index & 0xFF));
}

static bool normaliseSignal(JsonObject s, size_t index, bool strict, String &error) {
  if (!s) {
    if (strict) error = F("signal entry is not an object");
    return !strict;
  }
  if (!s["id"].is<const char *>() || strlen(s["id"] | "") == 0) {
    char id[IHR_MAX_ID];
    makeId(id, sizeof(id), "s", index);
    s["id"] = id;
  }
  if (strlen(s["id"] | "") > IHR_MAX_ID) {
    if (strict) { error = F("signal id is too long"); return false; }
    s["id"] = String(s["id"].as<const char *>()).substring(0, IHR_MAX_ID);
  }
  if (!s["label"].is<const char *>() || strlen(s["label"] | "") == 0)
    s["label"] = "Button";
  if (strlen(s["label"] | "") > IHR_MAX_LABEL) {
    if (strict) { error = F("signal label is too long"); return false; }
    s["label"] = String(s["label"].as<const char *>()).substring(0, IHR_MAX_LABEL);
  }
  if (s["labelBn"].is<const char *>() && strlen(s["labelBn"] | "") > IHR_MAX_LABEL) {
    if (strict) { error = F("signal Bengali label is too long"); return false; }
    s["labelBn"] = String(s["labelBn"].as<const char *>()).substring(0, IHR_MAX_LABEL);
  }
  String kind = s["kind"] | "protocol";
  if (kind == "ac") kind = "protocol";  // V1 accepted this spelling but never sent it
  if (kind != "protocol" && kind != "raw") kind = "protocol";
  s["kind"] = kind;
  uint8_t repeat = s["repeat"] | 1;
  if (repeat == 0 || repeat > 10) repeat = 1;
  s["repeat"] = repeat;
  if (kind == "raw") {
    JsonArray timings = s["timings"].as<JsonArray>();
    if (!timings) {
      s.remove("timings");
      timings = s.createNestedArray("timings");
    }
    if (timings.size() > IHR_MAX_RAW_TIMINGS) {
      if (strict) { error = F("raw signal has too many timings"); return false; }
      while (timings.size() > IHR_MAX_RAW_TIMINGS) timings.remove(timings.size() - 1);
    }
    for (JsonVariant v : timings) {
      long t = v.as<long>();
      if (t < 0 || t > 65535) {
        if (strict) { error = F("raw timing out of range"); return false; }
        v.set(38);
      }
    }
    uint16_t khz = s["khz"] | 38;
    if (khz < 30 || khz > 60) khz = 38;
    s["khz"] = khz;
  } else {
    if (!s["protocol"].is<const char *>()) s["protocol"] = "NEC";
    if (!s["value"].is<const char *>() && !s["value"].is<unsigned long>() &&
        !s["value"].is<long>())
      s["value"] = "0x0";
    uint16_t bits = s["bits"] | 32;
    if (bits == 0 || bits > 64) bits = 32;
    s["bits"] = bits;
  }
  return true;
}

static bool normaliseButton(JsonObject b, size_t index, bool strict, String &error) {
  if (!b) {
    if (strict) error = F("button entry is not an object");
    return !strict;
  }
  if (!b["id"].is<const char *>() || strlen(b["id"] | "") == 0) {
    char id[IHR_MAX_ID];
    makeId(id, sizeof(id), "b", index);
    b["id"] = id;
  }
  if (!b["label"].is<const char *>() || strlen(b["label"] | "") == 0)
    b["label"] = "Button";
  if (strlen(b["label"] | "") > IHR_MAX_LABEL) {
    if (strict) { error = F("button label is too long"); return false; }
    b["label"] = String(b["label"].as<const char *>()).substring(0, IHR_MAX_LABEL);
  }
  if (b["labelBn"].is<const char *>() && strlen(b["labelBn"] | "") > IHR_MAX_LABEL) {
    if (strict) { error = F("button Bengali label is too long"); return false; }
    b["labelBn"] = String(b["labelBn"].as<const char *>()).substring(0, IHR_MAX_LABEL);
  }
  uint8_t w = b["w"] | 1;
  if (w < 1) w = 1;
  if (w > 3) w = 3;
  b["w"] = w;
  if (!b["section"].is<const char *>()) b["section"] = "custom";
  b["favorite"] = b["favorite"] | false;
  if (b["signalId"].is<const char *>() && strlen(b["signalId"] | "") > IHR_MAX_ID) {
    if (strict) { error = F("button signal id is too long"); return false; }
    b["signalId"] = String(b["signalId"].as<const char *>()).substring(0, IHR_MAX_ID);
  }
  return true;
}

// `strict` is used for user-supplied PUT bodies: bad input must fail instead of
// being silently rewritten. Lenient mode is used while loading old databases.
static bool normalise(JsonVariant root, bool strict, String &error) {
  if (!root.is<JsonObject>()) {
    if (strict) error = F("database root must be an object");
    return !strict;
  }
  JsonObject obj = root.as<JsonObject>();
  obj["format"] = "ihr-signals";
  obj["version"] = IHR_SIGNALS_VERSION;

  if (!obj["profiles"].is<JsonArray>()) {
    if (strict && obj.containsKey("profiles")) {
      error = F("profiles must be an array");
      return false;
    }
    obj.remove("profiles");
    obj.createNestedArray("profiles");
  }
  JsonArray profiles = obj["profiles"].as<JsonArray>();
  if (profiles.size() > IHR_MAX_PROFILES) {
    if (strict) { error = F("too many profiles"); return false; }
    while (profiles.size() > IHR_MAX_PROFILES) profiles.remove(profiles.size() - 1);
  }

  size_t pi = 0;
  for (JsonObject p : profiles) {
    if (!p) {
      if (strict) { error = F("profile entry is not an object"); return false; }
      ++pi;
      continue;
    }
    if (!p["id"].is<const char *>() || strlen(p["id"] | "") == 0) {
      char id[IHR_MAX_ID];
      makeId(id, sizeof(id), "p", pi);
      p["id"] = id;
    }
    if (strlen(p["id"] | "") > IHR_MAX_ID) {
      if (strict) { error = F("profile id is too long"); return false; }
      p["id"] = String(p["id"].as<const char *>()).substring(0, IHR_MAX_ID);
    }
    if (!p["name"].is<const char *>() || strlen(p["name"] | "") == 0)
      p["name"] = "Remote";
    if (strlen(p["name"] | "") > IHR_MAX_LABEL) {
      if (strict) { error = F("profile name is too long"); return false; }
      p["name"] = String(p["name"].as<const char *>()).substring(0, IHR_MAX_LABEL);
    }
    if (p["nameBn"].is<const char *>() && strlen(p["nameBn"] | "") > IHR_MAX_LABEL) {
      if (strict) { error = F("profile Bengali name is too long"); return false; }
      p["nameBn"] = String(p["nameBn"].as<const char *>()).substring(0, IHR_MAX_LABEL);
    }
    if (!isCategory(p["category"] | "")) {
      p["category"] = inferCategory(p["name"] | "Remote");
    }
    p["favorite"] = p["favorite"] | false;
    if (!p["icon"].is<const char *>()) p["icon"] = "📺";
    if (!p["color"].is<const char *>()) p["color"] = "#4f8cff";

    if (!p["signals"].is<JsonArray>()) {
      if (strict && p.containsKey("signals")) {
        error = F("signals must be an array");
        return false;
      }
      p.remove("signals");
      p.createNestedArray("signals");
    }
    JsonArray signals = p["signals"].as<JsonArray>();
    if (signals.size() > IHR_MAX_SIGNALS) {
      if (strict) { error = F("too many signals in a profile"); return false; }
      while (signals.size() > IHR_MAX_SIGNALS) signals.remove(signals.size() - 1);
    }
    size_t si = 0;
    for (JsonObject s : signals) {
      if (!normaliseSignal(s, si, strict, error)) return false;
      ++si;
    }

    if (!p["buttons"].is<JsonArray>()) {
      if (strict && p.containsKey("buttons")) {
        error = F("buttons must be an array");
        return false;
      }
      p.remove("buttons");
      p.createNestedArray("buttons");
    }
    JsonArray buttons = p["buttons"].as<JsonArray>();
    if (buttons.size() > IHR_MAX_BUTTONS) {
      if (strict) { error = F("too many buttons in a profile"); return false; }
      while (buttons.size() > IHR_MAX_BUTTONS) buttons.remove(buttons.size() - 1);
    }

    // V1 databases only have signals. For custom remotes we can build a simple
    // grid; template remotes get their ergonomic layout from the dashboard, so
    // leave buttons empty there and let the client map by label.
    String category = p["category"] | "custom";
    if (buttons.size() == 0 && category == "custom" && signals.size() > 0) {
      size_t index = 0;
      for (JsonObject s : signals) {
        JsonObject b = buttons.add<JsonObject>();
        char id[IHR_MAX_ID];
        makeId(id, sizeof(id), "b", index);
        b["id"] = id;
        b["label"] = s["label"] | "Button";
        if (s["labelBn"].is<const char *>()) b["labelBn"] = s["labelBn"];
        b["icon"] = s["icon"] | "⭐";
        b["color"] = s["color"] | "#4f8cff";
        b["w"] = 1;
        b["section"] = "custom";
        b["signalId"] = s["id"] | "";
        b["favorite"] = false;
        ++index;
      }
    }
    size_t bi = 0;
    for (JsonObject b : buttons) {
      if (!normaliseButton(b, bi, strict, error)) return false;
      ++bi;
    }
    ++pi;
  }
  return true;
}

static void resetEmpty() {
  s_db.clear();
  JsonObject root = s_db.to<JsonObject>();
  root["format"] = "ihr-signals";
  root["version"] = IHR_SIGNALS_VERSION;
  root.createNestedArray("profiles");
}

static bool loadFromDisk(bool seedIfMissing) {
  String backupPath = String(IHR_SIGNALS_PATH) + ".bak";
  if (!IhrStorage::exists(IHR_SIGNALS_PATH) && !IhrStorage::exists(backupPath.c_str())) {
    if (!seedIfMissing) {
      resetEmpty();
      return false;
    }
    String starter(FPSTR(kStarterDevicesJson));
    if (!IhrStorage::writeText(IHR_SIGNALS_PATH, starter)) {
      Serial.println(F("[DB] failed to seed starter devices"));
      resetEmpty();
      return false;
    }
  }
  s_db.clear();
  if (!IhrStorage::readJson(IHR_SIGNALS_PATH, s_db)) {
    resetEmpty();
    Serial.println(F("[DB] signal database could not be loaded; keeping file for recovery"));
    return false;
  }
  String error;
  if (!normalise(s_db, false, error)) {
    resetEmpty();
    return false;
  }
  return true;
}

bool begin() {
  bool ok = loadFromDisk(true);
  if (!ok) resetEmpty();
  return ok;
}

JsonDocument &doc() { return s_db; }

bool save() { return IhrStorage::writeJson(IHR_SIGNALS_PATH, s_db); }

bool replace(const String &json) {
  // Parse into the live document only after clearing it, but keep the on-disk
  // copy as the rollback source. Invalid input never becomes the live DB.
  s_db.clear();
  DeserializationError err = deserializeJson(s_db, json);
  String error;
  if (err || !s_db.is<JsonObject>() || !normalise(s_db, true, error)) {
    Serial.printf("[DB] rejected replacement: %s\n",
                  err ? err.c_str() : error.c_str());
    loadFromDisk(false);
    return false;
  }
  if (!save()) {
    Serial.println(F("[DB] save failed; rolling back to the previous database"));
    loadFromDisk(false);
    return false;
  }
  return true;
}

String serialize() {
  String out;
  serializeJson(s_db, out);
  return out;
}

size_t jsonSize() { return measureJson(s_db); }

size_t serializeTo(Print &out) { return serializeJson(s_db, out); }

bool clear() {
  resetEmpty();
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

bool findProfile(const char *profileId, JsonObject &profile) {
  JsonArray profiles = s_db["profiles"].as<JsonArray>();
  for (JsonObject p : profiles) {
    if (strcmp(p["id"] | "", profileId) == 0) {
      profile = p;
      return true;
    }
  }
  return false;
}

size_t profileCount() {
  return s_db["profiles"].is<JsonArray>() ? s_db["profiles"].as<JsonArray>().size() : 0;
}

size_t signalCount() {
  size_t total = 0;
  JsonArray profiles = s_db["profiles"].as<JsonArray>();
  for (JsonObject p : profiles)
    total += p["signals"].is<JsonArray>() ? p["signals"].as<JsonArray>().size() : 0;
  return total;
}

}  // namespace SignalStore
