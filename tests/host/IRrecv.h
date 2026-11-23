// IRrecv simulation with a test-controlled capture queue.
#pragma once
#include "Arduino.h"
#include "IRremoteESP8266.h"

const uint16_t kRawTick = 2;
const uint16_t kRawBuf = 180;

struct decode_results {
  decode_type_t decode_type = UNKNOWN;
  union {
    struct {
      uint64_t value;
      uint32_t address;
      uint32_t command;
    };
    uint8_t state[64];
  };
  uint16_t bits = 0;
  volatile uint16_t *rawbuf = nullptr;
  uint16_t rawlen = 0;
  bool overflow = false;
  bool repeat = false;
};

class IRrecv {
 public:
  explicit IRrecv(uint16_t, uint16_t bufsize = kRawBuf, uint8_t timeout = 15,
                    bool saveBuffer = false) {
    (void)bufsize; (void)timeout; (void)saveBuffer; }
  void enableIRIn(bool = false) { enabled = true; }
  void resume() { resumeCalls++; }
  void disableIRIn() { enabled = false; }

  bool decode(decode_results *results) {
    if (!enabled || feedCount == 0 || !results) return false;
    *results = feed[feedCount - 1];
    feedCount--;
    return true;
  }

  static void testFeed(decode_type_t type, uint64_t value, uint16_t bits,
                       uint16_t *raw, uint16_t rawlen, bool overflow = false) {
    decode_results r;
    r.decode_type = type;
    r.value = value;
    r.bits = bits;
    r.rawlen = rawlen;
    r.rawbuf = raw;
    r.overflow = overflow;
    feed[feedCount++] = r;
  }
  static void testReset() { feedCount = 0; }
  static decode_results feed[8];
  static uint8_t feedCount;
  bool enabled = false;
  static inline uint32_t resumeCalls = 0;
};

inline decode_results IRrecv::feed[8];
inline uint8_t IRrecv::feedCount = 0;
