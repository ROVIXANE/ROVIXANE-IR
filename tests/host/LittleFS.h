// In-memory-backed LittleFS simulation for host tests.
#pragma once
#include "Arduino.h"
#include <fstream>
#include <filesystem>
#include <map>

struct FSInfo {
  size_t usedBytes = 0;
  size_t totalBytes = 4 * 1024 * 1024;
  size_t blockSize = 4096;
  size_t pageSize = 256;
  size_t maxOpenFiles = 8;
  size_t maxPathLength = 64;
};

class File : public Print {
 public:
  File() : _valid(false) {}
  File(const std::string &path, bool write) : _valid(true), _write(write) {
    if (!write) {
      std::ifstream in(path, std::ios::binary);
      if (in) _buf.assign(std::istreambuf_iterator<char>(in),
                          std::istreambuf_iterator<char>());
    }
  }
  explicit operator bool() const { return _valid; }
  bool valid() const { return _valid; }
  void close() {
    if (_valid && _write) {
      std::ofstream out(_path, std::ios::binary | std::ios::trunc);
      out.write(_buf.data(), static_cast<std::streamsize>(_buf.size()));
    }
    _valid = false;
  }
  size_t size() const { return _buf.size(); }
  int available() { return static_cast<int>(_buf.size() - _pos); }
  int read() {
    if (_pos >= _buf.size()) return -1;
    return static_cast<unsigned char>(_buf[_pos++]);
  }
  int peek() { return _pos < _buf.size() ? static_cast<unsigned char>(_buf[_pos]) : -1; }
  size_t write(uint8_t c) {
    _buf.push_back(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t *data, size_t len) {
    _buf.append(reinterpret_cast<const char *>(data), len);
    return len;
  }
  size_t print(const char *s) { _buf.append(s); return strlen(s); }
  size_t print(const String &s) { _buf.append(s); return s.length(); }
  size_t print(unsigned long v) { String s(v); return print(s); }
  size_t println(const char *s) { print(s); _buf.push_back('\n'); return strlen(s) + 1; }
  String readString() { return String(_buf); }
  void setPath(const std::string &p) { _path = p; }

 private:
  bool _valid;
  bool _write = false;
  std::string _path;
  std::string _buf;
  size_t _pos = 0;
};

namespace LittleFSImpl {
inline std::string root("./tests/.littlefs");
inline std::string native(const char *path) {
  std::string p(path ? path : "");
  if (!p.empty() && p[0] == '/') p.erase(0, 1);
  return root + "/" + p;
}
inline void ensureRoot() { std::filesystem::create_directories(root); }
inline void clearRoot() {
  std::error_code ec;
  std::filesystem::remove_all(root, ec);
  ensureRoot();
}
}  // namespace LittleFSImpl

class LittleFSClass {
 public:
  bool begin() {
    LittleFSImpl::ensureRoot();
    return true;
  }
  bool format() {
    LittleFSImpl::clearRoot();
    return true;
  }
  bool exists(const char *path) {
    return std::filesystem::exists(LittleFSImpl::native(path));
  }
  bool remove(const char *path) {
    std::error_code ec;
    return std::filesystem::remove(LittleFSImpl::native(path), ec);
  }
  bool rename(const char *from, const char *to) {
    std::error_code ec;
    std::filesystem::rename(LittleFSImpl::native(from), LittleFSImpl::native(to), ec);
    return !ec;
  }
  File open(const char *path, const char *mode) {
    if (!exists(path) && mode[0] == 'r') return File();
    File f(LittleFSImpl::native(path), mode[0] == 'w');
    f.setPath(LittleFSImpl::native(path));
    return f;
  }
  bool info(FSInfo &out) {
    out.usedBytes = 0;
    out.totalBytes = 4 * 1024 * 1024;
    if (!std::filesystem::exists(LittleFSImpl::root)) return true;
    for (auto &e : std::filesystem::recursive_directory_iterator(LittleFSImpl::root)) {
      if (e.is_regular_file()) out.usedBytes += e.file_size();
    }
    return true;
  }
};
inline LittleFSClass LittleFS;
