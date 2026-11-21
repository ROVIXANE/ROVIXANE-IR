#ifndef IHR_APPCONFIG_H
#define IHR_APPCONFIG_H

#include <Arduino.h>
#include "Compatibility.h"

struct AppConfig {
  uint8_t version = IHR_CONFIG_VERSION;
  char deviceName[32]   = "IHR IR Remote";
  char apSsid[33]       = "IHR-IR-Setup";
  char apPassword[64]   = "";          // empty => OPEN AP
  char wifiSsid[33]     = "";
  char wifiPassword[65] = "";
  char httpUser[33]     = "admin";
  char httpPassword[65] = "admin";
  char locale[8]        = "en";        // "en" or "bn"
  uint8_t irRepeat      = 1;
  bool captivePortal    = true;
  bool httpAuth         = false;
};

namespace AppCfg {

// Fills `cfg` from /config.json, applying defaults to missing fields.
// Returns false when the file does not exist or cannot be parsed.
bool load(AppConfig &cfg);

// Atomically persists the whole structure.
bool save(const AppConfig &cfg);

// Creates /config.json with factory defaults.
bool seedDefaults(AppConfig &cfg);

// NULL means "open AP" for the ESP8266 WiFi.softAP / WiFiManager API.
const char *apPasswordPtr(const AppConfig &cfg);

// Validates values coming from the dashboard. Returns an error string, or
// empty string on success.
String validate(const AppConfig &cfg);

}  // namespace AppCfg

#endif
