#ifndef IHR_COMPATIBILITY_H
#define IHR_COMPATIBILITY_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Project identity
// ROVIXANE IR — Universal Infrared Remote
// https://github.com/ROVIXANE/ROVIXANE-IR
// ---------------------------------------------------------------------------
#define ROVIXANE_IR_FIRMWARE_VERSION "3.0.0"
#define ROVIXANE_IR_DISPLAY_NAME "ROVIXANE IR"
#define ROVIXANE_IR_AP_SSID "ROVIXANE-IR-Setup"
#define ROVIXANE_IR_PRODUCT_NAME "ROVIXANE IR — Universal Infrared Remote"
#define ROVIXANE_IR_SOURCE_URL "https://github.com/ROVIXANE/ROVIXANE-IR"

// Persisted schema. V1 databases stay loadable: SignalStore migrates them in
// place on load and only writes the new schema when the user next changes data.
#define IHR_CONFIG_VERSION 2
#define IHR_SIGNALS_VERSION 2

// ---------------------------------------------------------------------------
// Board: NodeMCU 1.0 (ESP-12E), Arduino Core 3.1.2
// Boot-sensitive pins GPIO0 (D3), GPIO2 (D4) and GPIO15 (D8) are never used
// for IR. GPIO2 is only the built-in LED (active LOW), which is safe.
// ---------------------------------------------------------------------------
#define IHR_PIN_IR_SEND   4   // D2 -> 1k -> BC547 base -> IR LED
#define IHR_PIN_IR_RECV   14  // D5 -> TSOP38238 OUT
#define IHR_PIN_CFG_BTN   5   // D1 -> button -> GND (INPUT_PULLUP)
#define IHR_PIN_LED       2   // D4 -> built-in LED, active LOW

#define IHR_CFG_HOLD_MS   3000UL
#define IHR_HTTP_PORT     80
#define IHR_DNS_PORT      53

// ---------------------------------------------------------------------------
// Memory / storage limits (ESP8266 has ~80 KB RAM total, ~40 KB usable heap)
// Keep ONE persisted-size limit. The old 24 KB API / 20 KB boot mismatch could
// accept a database that the next boot refused to load.
// ---------------------------------------------------------------------------
#define IHR_MAX_JSON_BYTES    (20 * 1024)
#define IHR_MAX_API_BODY      IHR_MAX_JSON_BYTES
#define IHR_MAX_RAW_TIMINGS   128   // uint16_t each = 256 bytes on the stack
#define IHR_RECV_BUF_SIZE     180   // IRremoteESP8266 raw capture buffer
#define IHR_MAX_PROFILES      24
#define IHR_MAX_SIGNALS       64    // per profile
#define IHR_MAX_BUTTONS       64    // per profile
#define IHR_MAX_LABEL         40
#define IHR_MAX_ID            40
#define IHR_MAX_OTA_BYTES     (1000 * 1024)

#define IHR_CONFIG_PATH   "/config.json"
#define IHR_SIGNALS_PATH  "/signals.json"

// ---------------------------------------------------------------------------
// Web security
// ---------------------------------------------------------------------------
// A custom header cannot be attached by a cross-origin HTML form, so requiring
// it (plus a JSON content type) blocks classic CSRF without a session store.
#define IHR_CSRF_HEADER   "X-ROVIXANE-CSRF"
#define IHR_CSRF_VALUE    "1"
#define IHR_SECRET_MASK   "********"
#define IHR_MIN_HTTP_PASSWORD 8

// Machine-readable error codes returned next to the human message.
#define IHR_ERR_UNAUTHORIZED   "unauthorized"
#define IHR_ERR_SETUP_REQUIRED "setup_required"
#define IHR_ERR_CSRF           "csrf"
#define IHR_ERR_CONTENT_TYPE   "content_type"
#define IHR_ERR_VALIDATION     "validation"
#define IHR_ERR_STORAGE        "storage"
#define IHR_ERR_OTA            "ota"
#define IHR_ERR_UNSUPPORTED    "unsupported"

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------
inline void ihrSetLed(bool on) { digitalWrite(IHR_PIN_LED, on ? LOW : HIGH); }

inline bool ihrValidApPassword(const char *pwd) {
  if (pwd == nullptr) return false;
  size_t n = strlen(pwd);
  return n == 0 || (n >= 8 && n <= 63);
}

inline bool ihrValidApSsid(const char *ssid) {
  if (ssid == nullptr) return false;
  size_t n = strlen(ssid);
  return n > 0 && n <= 32;
}

// Signed comparison keeps restart/reset timers correct across millis() wrap.
inline bool ihrTimeReached(uint32_t now, uint32_t deadline) {
  return static_cast<int32_t>(now - deadline) >= 0;
}

#endif
