// ESP8266WiFi simulation for host tests.
#pragma once
#include "Arduino.h"

#define WIFI_OFF 0
#define WIFI_STA 1
#define WIFI_AP 2
#define WIFI_AP_STA 3
#define WL_CONNECTED 3
#define WL_IDLE_STATUS 0

class IPAddress {
 public:
  IPAddress() : IPAddress(0, 0, 0, 0) {}
  IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : b{a, b, c, d} {}
  String toString() const {
    char buf[16];
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
    return String(buf);
  }
  uint8_t b[4];
};

class WiFiClass {
 public:
  void mode(uint8_t m) { currentMode = m; }
  void begin(const char *ssid, const char *password) {
    staSsid = ssid ? ssid : "";
    staPassword = password ? password : "";
    beginCalls++;
  }
  void begin(const char *ssid) { begin(ssid, ""); }
  bool softAP(const char *ssid, const char *password = nullptr) {
    apSsid = ssid ? ssid : "";
    apPassword = password ? password : "";
    apCalls++;
    return true;
  }
  bool softAPConfig(IPAddress, IPAddress, IPAddress) { return true; }
  int status() const { return staConnected ? WL_CONNECTED : WL_IDLE_STATUS; }
  IPAddress localIP() const { return staIp; }
  IPAddress softAPIP() const { return IPAddress(192, 168, 4, 1); }
  int RSSI() const { return -55; }

  uint8_t currentMode = WIFI_OFF;
  bool staConnected = false;
  String staSsid, staPassword, apSsid, apPassword;
  int beginCalls = 0, apCalls = 0;
  IPAddress staIp{192, 168, 1, 42};
};
inline WiFiClass WiFi;
