// Minimal Arduino core simulation used ONLY by tests/host/test_firmware.cpp.
#pragma once
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

#define PROGMEM
#define FPSTR(p) (reinterpret_cast<const __FlashStringHelper *>(p))
#define F(p) FPSTR(p)
inline void *memcpy_P(void *d, const void *s, size_t n) { return memcpy(d, s, n); }
inline size_t strlen_P(const char *s) { return strlen(s); }

class __FlashStringHelper;

class String : public std::string {
 public:
  String() : std::string() {}
  String(const char *s) : std::string(s ? s : "") {}
  String(const std::string &s) : std::string(s) {}
  String(const __FlashStringHelper *s)
      : std::string(reinterpret_cast<const char *>(s)) {}
  String(char c) : std::string(1, c) {}
  String(int v) : std::string(std::to_string(v)) {}
  String(unsigned int v) : std::string(std::to_string(v)) {}
  String(long v) : std::string(std::to_string(v)) {}
  String(unsigned long v) : std::string(std::to_string(v)) {}
  String(long long v) : std::string(std::to_string(v)) {}
  String(unsigned long long v) : std::string(std::to_string(v)) {}
  String(unsigned long v, int base) : std::string() {
    if (v == 0) { push_back('0'); return; }
    char buf[32];
    int i = 32;
    while (v > 0) {
      unsigned d = v % static_cast<unsigned>(base);
      buf[--i] = (d < 10) ? char('0' + d) : char('a' + d - 10);
      v /= static_cast<unsigned>(base);
    }
    append(buf + i, 32 - i);
  }
  void concat(const char *s) { append(s ? s : ""); }
  void concat(const String &s) { append(s); }
  void concat(char c) { push_back(c); }
  size_t write(uint8_t c) { push_back(static_cast<char>(c)); return 1; }
  size_t write(const uint8_t *data, size_t len) {
    append(reinterpret_cast<const char *>(data), len);
    return len;
  }
  int indexOf(const char *needle) const {
    size_t p = find(needle);
    return p == npos ? -1 : static_cast<int>(p);
  }
  bool startsWith(const char *prefix) const { return rfind(prefix, 0) == 0; }
  int toInt() const { return std::atoi(c_str()); }
};

inline String operator+(const String &a, const String &b) {
  return String(static_cast<const std::string &>(a) + static_cast<const std::string &>(b));
}
inline String operator+(const String &a, const char *b) { return a + String(b); }
inline String operator+(const char *a, const String &b) { return String(a) + b; }
inline String operator+(const String &a, unsigned long b) { return a + String(b); }
inline String operator+(const String &a, int b) { return a + String(b); }
inline String &operator+=(String &a, const String &b) { a.concat(b); return a; }

inline size_t strlcpy(char *dst, const char *src, size_t size) {
  size_t n = strlen(src);
  if (size == 0) return n;
  if (n >= size) n = size - 1;
  memcpy(dst, src, n);
  dst[n] = '\0';
  return strlen(src);
}
inline size_t strlcat(char *dst, const char *src, size_t size) {
  size_t have = strnlen(dst, size);
  if (have == size) return size + strlen(src);
  size_t n = strlen(src);
  if (n >= size - have) n = size - have - 1;
  memcpy(dst + have, src, n);
  dst[have + n] = '\0';
  return have + strlen(src);
}

#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

inline uint32_t hostMillis = 0;
inline uint32_t millis() { return hostMillis; }
inline void delay(uint32_t ms) { hostMillis += ms; }
inline void delayMicroseconds(uint32_t) {}
inline void yield() {}
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t, uint8_t) {}
inline int digitalRead(uint8_t) { return HIGH; }
inline bool hostDigitalReadValue = false;
inline int hostDigitalRead() { return hostDigitalReadValue ? LOW : HIGH; }

class Print {
 public:
  virtual ~Print() {}
  size_t print(const char *s) { fputs(s, stdout); return strlen(s); }
  size_t print(const __FlashStringHelper *s) { return print(reinterpret_cast<const char *>(s)); }
  size_t print(const String &s) { return print(s.c_str()); }
  size_t print(int v) { printf("%d", v); return 0; }
  size_t print(unsigned long v) { printf("%lu", v); return 0; }
  size_t println() { fputc('\n', stdout); return 1; }
  size_t println(const char *s) { print(s); return println(); }
  size_t println(const __FlashStringHelper *s) { print(s); return println(); }
  size_t println(const String &s) { print(s); return println(); }
  int printf(const char *fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fputs(buf, stdout);
    return n;
  }
};

class HardwareSerial : public Print {
 public:
  void begin(unsigned long) {}
};
inline HardwareSerial Serial;

class ESPClass {
 public:
  uint32_t getFreeHeap() const { return 42000; }
  String getResetReason() const { return String("Software/System restart"); }
  void restart() { restarted = true; }
  bool restarted = false;
};
inline ESPClass ESP;

// ArduinoJson needs these when compiled without the ARDUINO macro:
inline unsigned long host_micros() { return 0; }
