#include "Storage.h"
#include "Compatibility.h"
#include <LittleFS.h>

namespace IhrStorage {

bool begin() {
  if (LittleFS.begin()) return true;
  Serial.println(F("[FS] mount failed - formatting LittleFS"));
  if (!LittleFS.format()) {
    Serial.println(F("[FS] format failed"));
    return false;
  }
  return LittleFS.begin();
}

bool exists(const char *path) { return LittleFS.exists(path); }

bool remove(const char *path) {
  if (!LittleFS.exists(path)) return true;
  if (!LittleFS.remove(path)) {
    Serial.printf("[FS] remove failed: %s\n", path);
    return false;
  }
  return true;
}

bool readJson(const char *path, JsonDocument &doc) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  if (f.size() > IHR_MAX_JSON_BYTES) {
    Serial.printf("[FS] %s is too large (%u bytes)\n", path,
                  static_cast<unsigned>(f.size()));
    f.close();
    return false;
  }
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    Serial.printf("[FS] JSON parse failed for %s: %s\n", path, err.c_str());
    return false;
  }
  return true;
}

bool writeJson(const char *path, const JsonDocument &doc) {
  String tmp = String(path) + ".tmp";
  File f = LittleFS.open(tmp.c_str(), "w");
  if (!f) {
    Serial.printf("[FS] cannot open %s for writing\n", tmp.c_str());
    return false;
  }
  size_t written = serializeJson(doc, f);
  f.close();
  if (written == 0) {
    LittleFS.remove(tmp.c_str());
    return false;
  }
  if (LittleFS.exists(path) && !LittleFS.remove(path)) {
    Serial.printf("[FS] cannot replace %s\n", path);
    LittleFS.remove(tmp.c_str());
    return false;
  }
  if (!LittleFS.rename(tmp.c_str(), path)) {
    Serial.printf("[FS] rename to %s failed\n", path);
    LittleFS.remove(tmp.c_str());
    return false;
  }
  return true;
}

bool readText(const char *path, String &out) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  if (f.size() > IHR_MAX_JSON_BYTES) {
    f.close();
    return false;
  }
  out.reserve(f.size() + 1);
  out = f.readString();
  f.close();
  return true;
}

bool writeText(const char *path, const String &data) {
  if (data.length() > IHR_MAX_JSON_BYTES) return false;
  String tmp = String(path) + ".tmp";
  File f = LittleFS.open(tmp.c_str(), "w");
  if (!f) return false;
  size_t written = f.print(data);
  f.close();
  if (written != data.length()) {
    LittleFS.remove(tmp.c_str());
    return false;
  }
  if (LittleFS.exists(path) && !LittleFS.remove(path)) {
    LittleFS.remove(tmp.c_str());
    return false;
  }
  if (!LittleFS.rename(tmp.c_str(), path)) {
    LittleFS.remove(tmp.c_str());
    return false;
  }
  return true;
}

size_t usedBytes() {
  FSInfo info;
  if (!LittleFS.info(info)) return 0;
  return info.usedBytes;
}

size_t totalBytes() {
  FSInfo info;
  if (!LittleFS.info(info)) return 0;
  return info.totalBytes;
}

}  // namespace IhrStorage
