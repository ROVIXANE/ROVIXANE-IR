// Updater simulation for host tests: records streaming OTA writes.
#pragma once
#include "Arduino.h"

#ifndef U_FLASH
#define U_FLASH 0
#endif
#ifndef UPDATE_SIZE_UNKNOWN
#define UPDATE_SIZE_UNKNOWN 0
#endif

class UpdaterClass {
 public:
  bool begin(size_t size, int command = U_FLASH, int ledPin = -1,
             uint8_t ledOn = LOW, const char *md5 = nullptr) {
    (void)command; (void)ledPin; (void)ledOn; (void)md5;
    begun = true;
    begunSize = size;
    written = 0;
    ended = false;
    error = false;
    return !failBegin;
  }
  size_t write(const uint8_t *data, size_t len) {
    if (!begun || failWrite) {
      error = true;
      return 0;
    }
    if (data != nullptr && len > 0) bytes.append(reinterpret_cast<const char *>(data), len);
    written += len;
    return len;
  }
  bool end(bool evenIfRemaining = false) {
    if (!begun) return false;
    ended = true;
    if (error || failEnd) return false;
    if (!evenIfRemaining && written != begunSize) return false;
    begun = false;
    return true;
  }
  bool hasError() const { return error; }
  const char *getErrorString() const { return "simulated updater error"; }

  bool begun = false;
  bool ended = false;
  bool error = false;
  bool failBegin = false;
  bool failWrite = false;
  bool failEnd = false;
  size_t begunSize = 0;
  size_t written = 0;
  String bytes;
};

inline UpdaterClass Update;
