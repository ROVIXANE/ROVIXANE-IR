// IRsend simulation: records the last transmitted frame for assertions.
#pragma once
#include "Arduino.h"
#include "IRremoteESP8266.h"

struct IrTxRecord {
  String kind;
  uint64_t value = 0;
  uint16_t bits = 0;
  uint16_t khz = 0;
  uint16_t count = 0;
  uint16_t raw[128] = {0};
  uint32_t calls = 0;
};

class IRsend {
 public:
  explicit IRsend(uint16_t, bool = false, bool = true) {}
  void begin() { begun = true; }
  void sendNEC(uint64_t v, uint16_t b = 32) { record("NEC", v, b); }
  void sendLG(uint64_t v, uint16_t b = 28) { record("LG", v, b); }
  void sendLG2(uint64_t v, uint16_t b = 28) { record("LG2", v, b); }
  void sendSony(uint64_t v, uint16_t b = 12) { record("SONY", v, b); }
  void sendSAMSUNG(uint64_t v, uint16_t b = 32) { record("SAMSUNG", v, b); }
  void sendJVC(uint64_t v, uint16_t b = 16) { record("JVC", v, b); }
  void sendPanasonic64(uint64_t v, uint16_t b = 48) { record("PANASONIC", v, b); }
  void sendRC5(uint64_t v, uint16_t b = 12) { record("RC5", v, b); }
  void sendRC6(uint64_t v, uint16_t b = 20) { record("RC6", v, b); }
  void sendDenon(uint64_t v, uint16_t b = 15) { record("DENON", v, b); }
  void sendCOOLIX(uint64_t v, uint16_t b = 24) { record("COOLIX", v, b); }
  void sendMidea24(uint64_t v, uint16_t b = 24) { record("MIDEA24", v, b); }
  void sendRaw(const uint16_t buf[], uint16_t len, uint16_t frequency) {
    tx.kind = "RAW";
    tx.khz = frequency;
    tx.count = len > 128 ? 128 : len;
    for (uint16_t i = 0; i < tx.count; ++i) tx.raw[i] = buf[i];
    tx.calls++;
  }
  static IrTxRecord tx;
  bool begun = false;

 private:
  static void record(const char *kind, uint64_t v, uint16_t b) {
    tx.kind = kind;
    tx.value = v;
    tx.bits = b;
    tx.calls++;
  }
};
inline IrTxRecord IRsend::tx;
