#ifndef IHR_STORAGE_H
#define IHR_STORAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Thin LittleFS wrapper. All writes are atomic: write "<path>.tmp", close,
// remove the old file, then rename. This avoids half-written JSON after a
// power loss, which is the most common cause of a corrupt config on ESP8266.
namespace IhrStorage {

bool begin();
bool exists(const char *path);
bool remove(const char *path);

bool readJson(const char *path, JsonDocument &doc);
bool writeJson(const char *path, const JsonDocument &doc);

// Raw file helpers used for the signals database (kept verbatim so the UI's
// import/export is byte-for-byte reversible).
bool readText(const char *path, String &out);
bool writeText(const char *path, const String &data);

size_t usedBytes();
size_t totalBytes();

}  // namespace IhrStorage

#endif
