#include "AppConfig.h"
#include "Storage.h"
#include <ArduinoJson.h>

namespace AppCfg {

bool load(AppConfig &cfg) {
  JsonDocument doc;
  if (!IhrStorage::readJson(IHR_CONFIG_PATH, doc)) return false;
  if (!doc.is<JsonObject>()) return false;

  // Accept both the V2 nested schema and the V1 flat schema.
  JsonObject ap = doc["ap"].is<JsonObject>() ? doc["ap"].as<JsonObject>() : JsonObject();
  JsonObject wifi = doc["wifi"].is<JsonObject>() ? doc["wifi"].as<JsonObject>() : JsonObject();
  JsonObject auth = doc["auth"].is<JsonObject>() ? doc["auth"].as<JsonObject>() : JsonObject();
  JsonObject ir = doc["ir"].is<JsonObject>() ? doc["ir"].as<JsonObject>() : JsonObject();

  cfg.version = IHR_CONFIG_VERSION;
  if (doc["deviceName"].is<const char *>())
    strlcpy(cfg.deviceName, doc["deviceName"] | ROVIXANE_IR_DISPLAY_NAME, sizeof(cfg.deviceName));
  if (ap["ssid"].is<const char *>())
    strlcpy(cfg.apSsid, ap["ssid"] | "", sizeof(cfg.apSsid));
  else if (doc["apSsid"].is<const char *>())
    strlcpy(cfg.apSsid, doc["apSsid"] | "", sizeof(cfg.apSsid));
  if (ap["password"].is<const char *>())
    strlcpy(cfg.apPassword, ap["password"] | "", sizeof(cfg.apPassword));
  else if (doc["apPassword"].is<const char *>())
    strlcpy(cfg.apPassword, doc["apPassword"] | "", sizeof(cfg.apPassword));
  if (wifi["ssid"].is<const char *>())
    strlcpy(cfg.wifiSsid, wifi["ssid"] | "", sizeof(cfg.wifiSsid));
  else if (doc["wifiSsid"].is<const char *>())
    strlcpy(cfg.wifiSsid, doc["wifiSsid"] | "", sizeof(cfg.wifiSsid));
  if (wifi["password"].is<const char *>())
    strlcpy(cfg.wifiPassword, wifi["password"] | "", sizeof(cfg.wifiPassword));
  else if (doc["wifiPassword"].is<const char *>())
    strlcpy(cfg.wifiPassword, doc["wifiPassword"] | "", sizeof(cfg.wifiPassword));
  if (auth["user"].is<const char *>())
    strlcpy(cfg.httpUser, auth["user"] | "admin", sizeof(cfg.httpUser));
  else if (doc["httpUser"].is<const char *>())
    strlcpy(cfg.httpUser, doc["httpUser"] | "admin", sizeof(cfg.httpUser));
  if (auth["password"].is<const char *>())
    strlcpy(cfg.httpPassword, auth["password"] | "", sizeof(cfg.httpPassword));
  else if (doc["httpPassword"].is<const char *>())
    strlcpy(cfg.httpPassword, doc["httpPassword"] | "", sizeof(cfg.httpPassword));
  if (auth["enabled"].is<bool>())
    cfg.httpAuth = auth["enabled"] | false;
  else if (doc["httpAuth"].is<bool>())
    cfg.httpAuth = doc["httpAuth"] | false;
  if (ir["repeat"].is<int>())
    cfg.irRepeat = ir["repeat"] | 1;
  else if (doc["irRepeat"].is<int>())
    cfg.irRepeat = doc["irRepeat"] | 1;
  if (ap["captivePortal"].is<bool>())
    cfg.captivePortal = ap["captivePortal"] | true;
  else if (doc["captivePortal"].is<bool>())
    cfg.captivePortal = doc["captivePortal"] | true;
  if (doc["locale"].is<const char *>())
    strlcpy(cfg.locale, doc["locale"] | "en", sizeof(cfg.locale));

  if (cfg.irRepeat == 0 || cfg.irRepeat > 10) cfg.irRepeat = 1;
  if (strcmp(cfg.locale, "en") != 0 && strcmp(cfg.locale, "bn") != 0)
    strlcpy(cfg.locale, "en", sizeof(cfg.locale));
  return true;
}

bool save(const AppConfig &cfg) {
  JsonDocument doc;
  doc["version"] = IHR_CONFIG_VERSION;
  doc["deviceName"] = cfg.deviceName;
  doc["ap"]["ssid"] = cfg.apSsid;
  doc["ap"]["password"] = cfg.apPassword;
  doc["ap"]["captivePortal"] = cfg.captivePortal;
  doc["wifi"]["ssid"] = cfg.wifiSsid;
  doc["wifi"]["password"] = cfg.wifiPassword;
  doc["auth"]["user"] = cfg.httpUser;
  doc["auth"]["password"] = cfg.httpPassword;
  doc["auth"]["enabled"] = cfg.httpAuth;
  doc["locale"] = cfg.locale;
  doc["ir"]["repeat"] = cfg.irRepeat;
  return IhrStorage::writeJson(IHR_CONFIG_PATH, doc);
}

bool seedDefaults(AppConfig &cfg) {
  cfg = AppConfig();
  return save(cfg);
}

const char *apPasswordPtr(const AppConfig &cfg) {
  return cfg.apPassword[0] == '\0' ? nullptr : cfg.apPassword;
}

bool setupRequired(const AppConfig &cfg) {
  return cfg.httpAuth && cfg.httpPassword[0] == '\0';
}

String validate(const AppConfig &cfg) {
  if (!ihrValidApSsid(cfg.apSsid)) return F("AP SSID must be 1-32 characters");
  if (!ihrValidApPassword(cfg.apPassword))
    return F("AP password must be empty (open) or 8-63 characters");
  if (strlen(cfg.wifiSsid) > 32) return F("WiFi SSID is too long");
  if (strlen(cfg.wifiPassword) > 64) return F("WiFi password is too long");
  if (strlen(cfg.deviceName) == 0) return F("Device name cannot be empty");
  if (strlen(cfg.deviceName) > 31) return F("Device name is too long");
  if (strcmp(cfg.locale, "en") != 0 && strcmp(cfg.locale, "bn") != 0)
    return F("Locale must be 'en' or 'bn'");
  if (cfg.irRepeat == 0 || cfg.irRepeat > 10) return F("Repeat must be 1-10");
  if (strlen(cfg.httpUser) > 32) return F("Login user is too long");
  if (strlen(cfg.httpPassword) > 64) return F("Login password is too long");
  if (cfg.httpAuth && (strlen(cfg.httpUser) == 0 || strlen(cfg.httpPassword) < IHR_MIN_HTTP_PASSWORD))
    return F("Login needs a user and a password of at least 8 characters");
  return String();
}

}  // namespace AppCfg
