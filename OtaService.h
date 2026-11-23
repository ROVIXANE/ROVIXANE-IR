#ifndef IHR_OTASERVICE_H
#define IHR_OTASERVICE_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <Updater.h>
#include "Compatibility.h"

#ifndef UPDATE_SIZE_UNKNOWN
#define UPDATE_SIZE_UNKNOWN 0
#endif

// Streaming firmware update support. No full firmware image is buffered in
// RAM: ESP8266WebServer hands us 2 KB multipart chunks and they go straight to
// the Updater flash writer.
namespace IhrOta {

struct State {
  bool active = false;
  bool done = false;
  bool ok = false;
  bool magicChecked = false;
  size_t announced = 0;
  size_t written = 0;
  String filename;
  String error;
};

inline State &state() {
  static State s;
  return s;
}

inline void reset() {
  State &s = state();
  s = State();
}

inline bool available() {
  // A real OTA slot is at least a few hundred KB. This is a build/layout check,
  // not a guarantee that this particular image will fit.
  return ESP.getFreeSketchSpace() >= 64 * 1024;
}

inline void fail(const String &message) {
  State &s = state();
  if (s.active) {
    // Release the updater without committing a partial image.
    Update.end(false);
  }
  s.ok = false;
  s.done = true;
  s.active = false;
  s.error = message;
  Serial.printf("[OTA] %s\n", message.c_str());
}

inline void handleUpload(ESP8266WebServer &server, bool allowed) {
  HTTPUpload &up = server.upload();
  State &s = state();

  if (up.status == UPLOAD_FILE_START) {
    reset();
    s.filename = up.filename;
    if (!allowed) {
      fail(F("authentication or CSRF check failed"));
      return;
    }
    if (!up.filename.endsWith(".bin")) {
      fail(F("file must end in .bin"));
      return;
    }
    // ESP8266 core 3.1.2 does not expose the exact file size during upload.
    // contentLength is the whole multipart request, so use it as a safe upper
    // bound and rely on Update.end(true) to commit the real byte count.
    size_t freeSpace = ESP.getFreeSketchSpace();
    if (freeSpace < 64 * 1024) {
      fail(F("not enough OTA flash space; use a 4MB/1MB OTA partition layout"));
      return;
    }
    size_t declared = up.contentLength;
    if (declared == 0 || declared > IHR_MAX_OTA_BYTES + 16 * 1024) {
      fail(F("file size is outside the allowed range"));
      return;
    }
    size_t maxDeclared = freeSpace & ~static_cast<size_t>(0xFFF);
    if (maxDeclared < 64 * 1024) {
      fail(F("not enough OTA flash space; use a 4MB/1MB OTA partition layout"));
      return;
    }
    if (declared > maxDeclared) declared = maxDeclared;
    s.announced = declared;
    if (!Update.begin(declared, U_FLASH)) {
      fail(Update.getErrorString());
      return;
    }
    s.active = true;
    Serial.printf("[OTA] receiving %s (declared %u bytes)\n", up.filename.c_str(),
                  static_cast<unsigned>(declared));
    return;
  }

  if (up.status == UPLOAD_FILE_WRITE) {
    if (!s.active) return;
    if (up.currentSize == 0) return;
    if (s.written + up.currentSize > s.announced) {
      fail(F("firmware is larger than the allowed OTA slot"));
      return;
    }
    if (!s.magicChecked) {
      s.magicChecked = true;
      if (up.buf[0] != 0xE9) {
        fail(F("not an ESP8266 firmware image (bad magic byte)"));
        return;
      }
    }
    size_t written = Update.write(up.buf, up.currentSize);
    if (written != up.currentSize) {
      String message = Update.hasError() ? String(Update.getErrorString())
                                         : String(F("flash write failed"));
      fail(message);
      return;
    }
    s.written += written;
    return;
  }

  if (up.status == UPLOAD_FILE_END) {
    if (!s.active) {
      if (!s.done) fail(F("upload did not start"));
      return;
    }
    if (s.written == 0) {
      fail(F("empty firmware upload"));
      return;
    }
    if (!Update.end(true)) {
      fail(Update.getErrorString());
      return;
    }
    s.active = false;
    s.done = true;
    s.ok = true;
    Serial.printf("[OTA] image accepted, %u bytes written\n",
                  static_cast<unsigned>(s.written));
    return;
  }

  if (up.status == UPLOAD_FILE_ABORTED) {
    if (s.active) Update.end(false);
    fail(F("upload aborted"));
  }
}

}  // namespace IhrOta

#endif
