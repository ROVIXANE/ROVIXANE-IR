#ifndef IHR_STORAGE_H
#define IHR_STORAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Crash-safe LittleFS wrapper.
//
// A write goes to "<path>.tmp". The previous file is moved to "<path>.bak"
// before the rename, so a power loss can leave the main path missing but never
// both copies missing. Reads transparently fall back to ".bak" and the next
// successful write repairs the main file. Corrupt primaries are quarantined
// before a recovered backup can be replaced; failed quarantine blocks writes.
namespace IhrStorage {

bool begin();
// Destructive: call only after explicit physical confirmation.
bool formatAndBegin();
bool exists(const char *path);
bool remove(const char *path);

bool readJson(const char *path, JsonDocument &doc);
bool writeJson(const char *path, const JsonDocument &doc);

// Raw file helpers used for the signals database and import/export.
bool readText(const char *path, String &out);
bool writeText(const char *path, const String &data);

size_t fileSize(const char *path);
size_t usedBytes();
size_t totalBytes();

}  // namespace IhrStorage

#endif
