#include "AppConfig.h"
#include "Compatibility.h"
#include "Storage.h"
#include <ArduinoJson.h>

namespace AppCfg {

static void applyDefaults(AppConfig &cfg) {
  AppConfig d;
  if (cfg.version == 0) cfg.version = d.version;
  if (cfg.deviceName[0] == '\0') strlcpy(cfg.deviceName, d.deviceName, sizeof(cfg.deviceName));
  if (cfg.apSsid[0] == '\0') strlcpy(cfg.apSsid, d.apSsid, sizeof(cfg.apSsid));
  if (!ihrValidApSsid(cfg.apSsid)) strlcpy(cfg.apSsid, d.apSsid, sizeof(cfg.apSsid));
  if (!ihrValidApPassword(cfg.apPassword)) cfg.apPassword[0] = '\0';
  if (cfg.wifiSsid[0] == '\0') cfg.wifiSsid[0] = '\0';
  if (cfg.locale[0] == '\0') strlcpy(cfg.locale, "en", sizeof(cfg.locale));
  if (strcmp(cfg.locale, "en") != 0 && strcmp(cfg.locale, "bn") != 0)
    strlcpy(cfg.locale, "en", sizeof(cfg.locale));
  if (cfg.irRepeat == 0 || cfg.irRepeat > 10) cfg.irRepeat = 1;
}

bool load(AppConfig &cfg) {
  JsonDocument doc;
  if (!IhrStorage::readJson(IHR_CONFIG_PATH, doc)) return false;

  cfg = AppConfig();
  cfg.version = doc["version"] | IHR_CONFIG_VERSION;
  strlcpy(cfg.deviceName,   doc["deviceName"]   | ROVIXANE_IR_DISPLAY_NAME, sizeof(cfg.deviceName));
  strlcpy(cfg.apSsid,       doc["ap"]["ssid"]    | ROVIXANE_IR_AP_SSID,  sizeof(cfg.apSsid));
  strlcpy(cfg.apPassword,   doc["ap"]["password"]| "",             sizeof(cfg.apPassword));
  strlcpy(cfg.wifiSsid,     doc["wifi"]["ssid"]   | "",             sizeof(cfg.wifiSsid));
  strlcpy(cfg.wifiPassword, doc["wifi"]["password"]| "",            sizeof(cfg.wifiPassword));
  strlcpy(cfg.httpUser,     doc["auth"]["user"]   | "admin",        sizeof(cfg.httpUser));
  strlcpy(cfg.httpPassword, doc["auth"]["password"]| "admin",       sizeof(cfg.httpPassword));
  strlcpy(cfg.locale,       doc["locale"]         | "en",            sizeof(cfg.locale));
  cfg.irRepeat      = doc["ir"]["repeat"] | 1;
  cfg.captivePortal = doc["ap"]["captivePortal"] | true;
  cfg.httpAuth      = doc["auth"]["enabled"] | false;

  applyDefaults(cfg);
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

String validate(const AppConfig &cfg) {
  if (!ihrValidApSsid(cfg.apSsid)) return F("AP SSID must be 1-32 characters");
  if (!ihrValidApPassword(cfg.apPassword))
    return F("AP password must be empty (open) or 8-63 characters");
  if (strlen(cfg.wifiSsid) > 32) return F("WiFi SSID is too long");
  if (strlen(cfg.wifiPassword) > 64) return F("WiFi password is too long");
  if (strlen(cfg.deviceName) == 0) return F("Device name cannot be empty");
  if (strcmp(cfg.locale, "en") != 0 && strcmp(cfg.locale, "bn") != 0)
    return F("Locale must be 'en' or 'bn'");
  if (cfg.irRepeat == 0 || cfg.irRepeat > 10) return F("Repeat must be 1-10");
  if (cfg.httpAuth && (strlen(cfg.httpUser) == 0 || strlen(cfg.httpPassword) < 4))
    return F("HTTP auth needs a user and a password of at least 4 characters");
  return String();
}

}  // namespace AppCfg
