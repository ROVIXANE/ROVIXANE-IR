// DNSServer simulation: only records that the captive portal was started.
#pragma once
#include "Arduino.h"
#include "ESP8266WiFi.h"

class DNSServer {
 public:
  void start(uint16_t port, const char *domain, IPAddress ip) {
    started = true;
    dnsPort = port;
    dnsDomain = domain ? domain : "";
    dnsIp = ip;
  }
  void stop() { started = false; }
  void processNextRequest() { processed++; }
  bool started = false;
  uint16_t dnsPort = 0;
  String dnsDomain;
  IPAddress dnsIp;
  uint32_t processed = 0;
};
