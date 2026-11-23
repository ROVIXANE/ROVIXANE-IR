#include "Storage.h"
#include "Compatibility.h"
#include <LittleFS.h>

namespace IhrStorage {

static String bakPath(const char *path) { return String(path) + ".bak"; }
static String tmpPath(const char *path) { return String(path) + ".tmp"; }

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
  bool ok = true;
  const char *suffixes[] = {"", ".bak", ".tmp"};
  for (const char *suffix : suffixes) {
    String p = String(path) + suffix;
    if (LittleFS.exists(p.c_str()) && !LittleFS.remove(p.c_str())) {
      Serial.printf("[FS] remove failed: %s\n", p.c_str());
      ok = false;
    }
  }
  return ok;
}

static bool readJsonOne(const char *path, JsonDocument &doc) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  if (f.size() == 0 || f.size() > IHR_MAX_JSON_BYTES) {
    Serial.printf("[FS] %s has invalid size (%u bytes)\n", path,
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

bool readJson(const char *path, JsonDocument &doc) {
  doc.clear();
  if (readJsonOne(path, doc)) return true;
  String bak = bakPath(path);
  doc.clear();
  if (readJsonOne(bak.c_str(), doc)) {
    Serial.printf("[FS] recovered %s from %s\n", path, bak.c_str());
    return true;
  }
  return false;
}

// Move the destination out of the way, install the new file, then drop the
// backup. Any failure restores the previous file.
static bool installFile(const char *path, const String &tmp) {
  String bak = bakPath(path);
  bool hadOld = LittleFS.exists(path);
  if (hadOld) {
    if (LittleFS.exists(bak.c_str())) LittleFS.remove(bak.c_str());
    if (!LittleFS.rename(path, bak.c_str())) {
      Serial.printf("[FS] cannot back up %s\n", path);
      LittleFS.remove(tmp.c_str());
      return false;
    }
  }
  if (!LittleFS.rename(tmp.c_str(), path)) {
    Serial.printf("[FS] rename to %s failed\n", path);
    if (hadOld) LittleFS.rename(bak.c_str(), path);
    LittleFS.remove(tmp.c_str());
    return false;
  }
  if (hadOld) LittleFS.remove(bak.c_str());
  return true;
}

bool writeJson(const char *path, const JsonDocument &doc) {
  String tmp = tmpPath(path);
  File f = LittleFS.open(tmp.c_str(), "w");
  if (!f) {
    Serial.printf("[FS] cannot open %s for writing\n", tmp.c_str());
    return false;
  }
  size_t written = serializeJson(doc, f);
  f.close();
  if (written == 0 || written > IHR_MAX_JSON_BYTES) {
    Serial.printf("[FS] refusing %u-byte write to %s\n",
                  static_cast<unsigned>(written), path);
    LittleFS.remove(tmp.c_str());
    return false;
  }
  return installFile(path, tmp);
}

bool readText(const char *path, String &out) {
  const char *candidates[] = {path, nullptr};
  String bak = bakPath(path);
  candidates[1] = bak.c_str();
  for (int i = 0; i < 2; ++i) {
    File f = LittleFS.open(candidates[i], "r");
    if (!f) continue;
    if (f.size() > IHR_MAX_JSON_BYTES) {
      f.close();
      continue;
    }
    out.reserve(f.size() + 1);
    out = f.readString();
    f.close();
    return true;
  }
  return false;
}

bool writeText(const char *path, const String &data) {
  if (data.length() == 0 || data.length() > IHR_MAX_JSON_BYTES) return false;
  String tmp = tmpPath(path);
  File f = LittleFS.open(tmp.c_str(), "w");
  if (!f) return false;
  size_t written = f.print(data);
  f.close();
  if (written != data.length()) {
    LittleFS.remove(tmp.c_str());
    return false;
  }
  return installFile(path, tmp);
}

size_t fileSize(const char *path) {
  File f = LittleFS.open(path, "r");
  if (!f) return 0;
  size_t n = f.size();
  f.close();
  return n;
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
