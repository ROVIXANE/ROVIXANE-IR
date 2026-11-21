#ifndef IHR_COMPATIBILITY_H
#define IHR_COMPATIBILITY_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Project identity
// ---------------------------------------------------------------------------
#define IHR_FIRMWARE_VERSION "1.0.0"
#define IHR_CONFIG_VERSION 1

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
// Memory / storage limits (ESP8266 has ~80 KB RAM total)
// ---------------------------------------------------------------------------
#define IHR_MAX_RAW_TIMINGS   128   // uint16_t each = 256 bytes on the stack
#define IHR_RECV_BUF_SIZE     180   // IRremoteESP8266 raw capture buffer
#define IHR_RECV_TIMEOUT_MS   100
#define IHR_MAX_JSON_BYTES    (20 * 1024)
#define IHR_MAX_API_BODY      (24 * 1024)
#define IHR_MAX_WIFI_SCAN     20

#define IHR_CONFIG_PATH   "/config.json"
#define IHR_SIGNALS_PATH  "/signals.json"

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

#endif
