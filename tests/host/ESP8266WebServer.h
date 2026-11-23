// ESP8266WebServer simulation: routes, basic auth, headers, streaming output
// and multipart upload callbacks can all be driven directly from tests.
#pragma once
#include "Arduino.h"
#include <functional>
#include <map>

#define HTTP_GET 0
#define HTTP_POST 1
#define HTTP_PUT 2
#define HTTP_DELETE 3
#define HTTP_ANY 4
#define BASIC_AUTH 1

#define HTTP_UPLOAD_BUFLEN 2048
#define UPLOAD_FILE_START 0
#define UPLOAD_FILE_WRITE 1
#define UPLOAD_FILE_END 2
#define UPLOAD_FILE_ABORTED 3

struct HTTPUpload {
  int status = UPLOAD_FILE_START;
  String filename;
  String name;
  String type;
  size_t totalSize = 0;
  size_t currentSize = 0;
  size_t contentLength = 0;
  uint8_t buf[HTTP_UPLOAD_BUFLEN] = {0};
};

struct HttpResponse {
  int code = 0;
  String contentType;
  String body;
  String location;
  std::map<String, String> headers;
};

class CapturePrint : public Print {
 public:
  explicit CapturePrint(String &out) : _out(out) {}
  size_t write(uint8_t c) override {
    _out.push_back(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t *data, size_t len) override {
    _out.append(reinterpret_cast<const char *>(data), len);
    return len;
  }

 private:
  String &_out;
};

class ESP8266WebServer {
 public:
  using Handler = std::function<void(void)>;
  explicit ESP8266WebServer(int port) : _port(port), _client(_response.body) {}

  void on(const char *path, uint8_t method, Handler fn) {
    Route r;
    r.path = path;
    r.method = method;
    r.fn = fn;
    _routes.push_back(r);
  }
  void on(const char *path, uint8_t method, Handler fn, Handler uploadFn) {
    Route r;
    r.path = path;
    r.method = method;
    r.fn = fn;
    r.upload = uploadFn;
    _routes.push_back(r);
  }
  void on(const char *path, Handler fn) { on(path, HTTP_ANY, fn); }
  void onNotFound(Handler fn) { _notFound = fn; }
  void begin() { _running = true; }
  void handleClient() { _handled++; }

  void setContentLength(size_t len) { _expectedLength = len; }
  void sendHeader(const String &name, const String &value) {
    _response.headers[name] = value;
    if (name == "Location") _response.location = value;
  }
  void send(int code, const String &contentType, const String &body) {
    _response.code = code;
    _response.contentType = contentType;
    _response.body = body;
  }
  void send(int code, const char *contentType, const String &body) {
    send(code, String(contentType), body);
  }
  void send(int code, const char *contentType, const char *body) {
    send(code, String(contentType), String(body));
  }
  void sendContent(const char *data, size_t len) {
    _response.body.append(data, len);
  }
  void sendContent(const String &data) { _response.body.append(data); }
  int sendContent_P(const char *data, size_t len) {
    sendContent(data, len);
    return static_cast<int>(len);
  }

  String arg(const char *name) const {
    if (strcmp(name, "plain") == 0) return _body;
    auto it = _args.find(name);
    return it == _args.end() ? String() : it->second;
  }
  bool hasArg(const char *name) const { return _args.count(name) > 0; }
  String uri() const { return _uri; }
  String method() const { return String(_methodName); }
  String header(const String &name) const {
    for (const auto &entry : _requestHeaders) {
      if (equalsIgnoreCase(entry.first.c_str(), name.c_str())) return entry.second;
    }
    return String();
  }
  const std::map<String, String> &headers() const { return _requestHeaders; }

  bool authenticate(const char *user, const char *password) {
    String expected = String(user) + ":" + String(password);
    String encoded = base64(expected);
    return _authHeader == ("Basic " + encoded);
  }
  void requestAuthentication() {
    _response.code = 401;
    _response.contentType = "text/plain";
    _response.body = "Unauthorized";
    _response.headers["WWW-Authenticate"] = "Basic realm=\"ROVIXANE IR\"";
  }

  Print &client() { return _client; }
  HTTPUpload &upload() { return _upload; }

  // Prepare a request context (headers/auth) for multipart upload callbacks.
  void beginRequest(uint8_t method, const char *path, const String &authHeader,
                    const std::map<String, String> &headers) {
    _method = method;
    _methodName = methodName(method);
    _uri = path;
    _authHeader = authHeader;
    _requestHeaders = headers;
    _body = String();
    _response = HttpResponse();
    _expectedLength = 0;
  }

  // Feed one multipart chunk to the registered upload handler.
  bool feedUpload(int status, const String &filename, const uint8_t *data,
                  size_t len, size_t contentLength) {
    _upload = HTTPUpload();
    _upload.status = status;
    _upload.filename = filename;
    _upload.contentLength = contentLength;
    _upload.totalSize = contentLength;
    size_t copy = len;
    if (copy > sizeof(_upload.buf)) copy = sizeof(_upload.buf);
    _upload.currentSize = copy;
    if (data != nullptr && copy > 0) memcpy(_upload.buf, data, copy);
    for (const Route &r : _routes) {
      if (r.upload) {
        r.upload();
        return true;
      }
    }
    return false;
  }

  HttpResponse dispatch(uint8_t method, const char *path,
                        const String &body = String(),
                        const String &authHeader = String(),
                        const std::map<String, String> &headers = {}) {
    _method = method;
    _methodName = methodName(method);
    _uri = path;
    _body = body;
    _authHeader = authHeader;
    _requestHeaders = headers;
    _response = HttpResponse();
    _expectedLength = 0;
    for (const Route &r : _routes) {
      if (r.path == path && (r.method == method || r.method == HTTP_ANY)) {
        r.fn();
        return _response;
      }
    }
    if (_notFound) _notFound();
    else send(404, "text/plain", "Not found");
    return _response;
  }

  int port() const { return _port; }
  bool running() const { return _running; }
  uint32_t handledCount() const { return _handled; }
  size_t expectedLength() const { return _expectedLength; }

 private:
  struct Route {
    String path;
    uint8_t method = HTTP_ANY;
    Handler fn;
    Handler upload;
  };

  static bool equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
      char ca = (*a >= 'A' && *a <= 'Z') ? static_cast<char>(*a + 32) : *a;
      char cb = (*b >= 'A' && *b <= 'Z') ? static_cast<char>(*b + 32) : *b;
      if (ca != cb) return false;
      ++a;
      ++b;
    }
    return *a == '\0' && *b == '\0';
  }

  static const char *methodName(uint8_t method) {
    switch (method) {
      case HTTP_GET: return "GET";
      case HTTP_POST: return "POST";
      case HTTP_PUT: return "PUT";
      case HTTP_DELETE: return "DELETE";
      default: return "ANY";
    }
  }

  std::vector<Route> _routes;
  Handler _notFound;
  std::map<String, String> _args;
  int _port;
  bool _running = false;
  uint32_t _handled = 0;
  String _uri, _body, _authHeader, _methodName;
  uint8_t _method = HTTP_GET;
  size_t _expectedLength = 0;
  HttpResponse _response;
  CapturePrint _client;
  HTTPUpload _upload;
  std::map<String, String> _requestHeaders;

  static String base64(const String &in) {
    static const char *tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String out;
    size_t i = 0;
    while (i < in.length()) {
      uint32_t v = static_cast<unsigned char>(in[i++]) << 16;
      bool has2 = i < in.length();
      if (has2) v |= static_cast<unsigned char>(in[i++]) << 8;
      bool has3 = i < in.length();
      if (has3) v |= static_cast<unsigned char>(in[i++]);
      out.concat(tbl[(v >> 18) & 63]);
      out.concat(tbl[(v >> 12) & 63]);
      out.concat(has2 ? tbl[(v >> 6) & 63] : '=');
      out.concat(has3 ? tbl[v & 63] : '=');
    }
    return out;
  }
};
