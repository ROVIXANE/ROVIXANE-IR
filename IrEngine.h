#ifndef IHR_IRENGINE_H
#define IHR_IRENGINE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Compatibility.h"

// Pure IR helpers shared by the HTTP layer and the host tests. Everything here
// is inline/header-only so the Arduino sketch still builds as one unit.
namespace IhrIr {

inline bool ieq(const char *a, const char *b) {
  if (a == nullptr || b == nullptr) return false;
  while (*a && *b) {
    char ca = (*a >= 'a' && *a <= 'z') ? static_cast<char>(*a - 32) : *a;
    char cb = (*b >= 'a' && *b <= 'z') ? static_cast<char>(*b - 32) : *b;
    if (ca != cb) return false;
    ++a;
    ++b;
  }
  return *a == '\0' && *b == '\0';
}

inline void appendHex64(String &out, uint64_t value, uint8_t minDigits) {
  char digits[17];
  int pos = 16;
  digits[pos] = '\0';
  do {
    uint8_t d = value & 0xFULL;
    digits[--pos] = (d < 10) ? static_cast<char>('0' + d)
                             : static_cast<char>('A' + d - 10);
    value >>= 4;
  } while (value > 0);
  int written = 16 - pos;
  if (minDigits > 16) minDigits = 16;
  while (written < minDigits) {
    digits[--pos] = '0';
    ++written;
  }
  out.concat(digits + pos);
}

// Strict 64-bit hex parser: separators are allowed, but more than 16 digits or
// any non-hex character is an error (the old parser silently wrapped).
inline bool parseHex64(const char *text, uint64_t &out) {
  if (text == nullptr) return false;
  while (*text == ' ' || *text == '\t') ++text;
  if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) text += 2;
  uint8_t digits = 0;
  uint64_t value = 0;
  for (; *text != '\0'; ++text) {
    char c = *text;
    uint8_t d;
    if (c >= '0' && c <= '9') d = static_cast<uint8_t>(c - '0');
    else if (c >= 'a' && c <= 'f') d = static_cast<uint8_t>(c - 'a' + 10);
    else if (c >= 'A' && c <= 'F') d = static_cast<uint8_t>(c - 'A' + 10);
    else if (c == ' ' || c == '_' || c == ',') continue;
    else return false;
    if (++digits > 16) return false;
    value = (value << 4) | d;
  }
  if (digits == 0) return false;
  out = value;
  return true;
}

// Reject negative JSON numbers instead of casting them to a huge uint64_t.
inline bool jsonToU64(JsonVariantConst value, uint64_t &out) {
  if (value.is<const char *>()) return parseHex64(value.as<const char *>(), out);
  if (value.is<String>()) {
    String s = value.as<String>();
    return parseHex64(s.c_str(), out);
  }
  if (value.is<uint64_t>() || value.is<unsigned long>() ||
      value.is<unsigned int>()) {
    out = value.as<uint64_t>();
    return true;
  }
  if (value.is<int64_t>() || value.is<long>() || value.is<int>()) {
    int64_t v = value.as<int64_t>();
    if (v < 0) return false;
    out = static_cast<uint64_t>(v);
    return true;
  }
  return false;
}

inline decode_type_t protocolFromName(const char *name) {
  if (name == nullptr) return UNKNOWN;
  if (ieq(name, "NEC")) return NEC;
  if (ieq(name, "LG")) return LG;
  if (ieq(name, "LG2")) return LG2;
  if (ieq(name, "SONY") || ieq(name, "SONY12")) return SONY;
  if (ieq(name, "SAMSUNG")) return SAMSUNG;
  if (ieq(name, "JVC")) return JVC;
  if (ieq(name, "PANASONIC")) return PANASONIC;
  if (ieq(name, "RC5")) return RC5;
  if (ieq(name, "RC6")) return RC6;
  if (ieq(name, "DENON")) return DENON;
  if (ieq(name, "COOLIX")) return COOLIX;
  if (ieq(name, "MIDEA24")) return MIDEA24;
  return UNKNOWN;
}

inline const char *protocolName(decode_type_t type) {
  switch (type) {
    case NEC: return "NEC";
    case LG: return "LG";
    case LG2: return "LG2";
    case SONY: return "SONY";
    case SAMSUNG: return "SAMSUNG";
    case JVC: return "JVC";
    case PANASONIC: return "PANASONIC";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case DENON: return "DENON";
    case COOLIX: return "COOLIX";
    case MIDEA24: return "MIDEA24";
    default: return "UNKNOWN";
  }
}

inline uint16_t defaultBits(decode_type_t type) {
  switch (type) {
    case NEC: return 32;
    case LG: case LG2: return 28;
    case SONY: return 12;
    case SAMSUNG: return 32;
    case JVC: return 16;
    case PANASONIC: return 48;
    case RC5: return 12;
    case RC6: return 20;
    case DENON: return 15;
    case COOLIX: return 24;
    case MIDEA24: return 24;
    default: return 32;
  }
}

// Mirrors sendProtocolFrame so learning can fall back to raw when the build
// does not include a sender for a protocol it happens to decode.
inline bool isSendableProtocol(decode_type_t type) {
  switch (type) {
#if defined(SEND_NEC) && SEND_NEC
    case NEC: return true;
#endif
#if defined(SEND_LG) && SEND_LG
    case LG: case LG2: return true;
#endif
#if defined(SEND_SONY) && SEND_SONY
    case SONY: return true;
#endif
#if defined(SEND_SAMSUNG) && SEND_SAMSUNG
    case SAMSUNG: return true;
#endif
#if defined(SEND_JVC) && SEND_JVC
    case JVC: return true;
#endif
#if defined(SEND_PANASONIC) && SEND_PANASONIC
    case PANASONIC: return true;
#endif
#if defined(SEND_RC5) && SEND_RC5
    case RC5: return true;
#endif
#if defined(SEND_RC6) && SEND_RC6
    case RC6: return true;
#endif
#if defined(SEND_DENON) && SEND_DENON
    case DENON: return true;
#endif
#if defined(SEND_COOLIX) && SEND_COOLIX
    case COOLIX: return true;
#endif
#if defined(SEND_MIDEA24) && SEND_MIDEA24
    case MIDEA24: return true;
#endif
    default: return false;
  }
}

inline bool sendProtocolFrame(IRsend &sender, decode_type_t type,
                              uint64_t value, uint16_t bits) {
  switch (type) {
#if defined(SEND_NEC) && SEND_NEC
    case NEC: sender.sendNEC(value, bits); return true;
#endif
#if defined(SEND_LG) && SEND_LG
    case LG: sender.sendLG(value, bits); return true;
    case LG2: sender.sendLG2(value, bits); return true;
#endif
#if defined(SEND_SONY) && SEND_SONY
    case SONY: sender.sendSony(value, bits); return true;
#endif
#if defined(SEND_SAMSUNG) && SEND_SAMSUNG
    case SAMSUNG: sender.sendSAMSUNG(value, bits); return true;
#endif
#if defined(SEND_JVC) && SEND_JVC
    case JVC: sender.sendJVC(value, bits); return true;
#endif
#if defined(SEND_PANASONIC) && SEND_PANASONIC
    case PANASONIC: sender.sendPanasonic64(value, bits); return true;
#endif
#if defined(SEND_RC5) && SEND_RC5
    case RC5: sender.sendRC5(value, bits); return true;
#endif
#if defined(SEND_RC6) && SEND_RC6
    case RC6: sender.sendRC6(value, bits); return true;
#endif
#if defined(SEND_DENON) && SEND_DENON
    case DENON: sender.sendDenon(value, bits); return true;
#endif
#if defined(SEND_COOLIX) && SEND_COOLIX
    case COOLIX: sender.sendCOOLIX(value, bits); return true;
#endif
#if defined(SEND_MIDEA24) && SEND_MIDEA24
    case MIDEA24: sender.sendMidea24(value, bits); return true;
#endif
    default: return false;
  }
}

// Returns a user-facing reason in `error` instead of silently dropping timings.
inline bool transmit(IRsend &sender, JsonObject signal, uint8_t repeatDefault,
                     String &error) {
  uint8_t repeat = repeatDefault;
  if (signal["repeat"].is<int>()) repeat = signal["repeat"] | repeatDefault;
  if (repeat == 0 || repeat > 10) {
    error = F("repeat must be 1-10");
    return false;
  }

  String kind = signal["kind"] | "protocol";
  if (kind == "raw") {
    JsonArray arr = signal["timings"].as<JsonArray>();
    if (!arr || arr.size() < 4 || arr.size() > IHR_MAX_RAW_TIMINGS) {
      error = F("raw signal needs 4-128 timings");
      return false;
    }
    uint16_t buf[IHR_MAX_RAW_TIMINGS];
    uint16_t count = 0;
    for (JsonVariant v : arr) {
      long us = v.as<long>();
      if (us <= 0 || us > 65535) {
        error = F("raw timing must be 1-65535 microseconds");
        return false;
      }
      buf[count++] = static_cast<uint16_t>(us);
    }
    uint16_t khz = signal["khz"] | 38;
    if (khz < 30 || khz > 60) {
      error = F("carrier must be 30-60 kHz");
      return false;
    }
    for (uint8_t i = 0; i < repeat; ++i) {
      sender.sendRaw(buf, count, khz);
      if (i + 1 < repeat) delay(40);
    }
    return true;
  }

  String name = signal["protocol"] | "";
  decode_type_t type = protocolFromName(name.c_str());
  if (type == UNKNOWN) {
    error = String(F("unsupported protocol: ")) + name;
    return false;
  }
  uint64_t value = 0;
  if (!jsonToU64(signal["value"], value)) {
    error = F("value must be a 0-16 digit HEX or non-negative integer");
    return false;
  }
  uint16_t bits = signal["bits"] | defaultBits(type);
  if (bits == 0 || bits > 64) {
    error = F("bit length must be 1-64");
    return false;
  }
  for (uint8_t i = 0; i < repeat; ++i) {
    if (!sendProtocolFrame(sender, type, value, bits)) {
      error = String(F("protocol is not enabled in this build: ")) + name;
      return false;
    }
    if (i + 1 < repeat) delay(40);
  }
  return true;
}

}  // namespace IhrIr

#endif
