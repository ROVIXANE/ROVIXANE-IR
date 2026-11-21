// ESP8266WebServer simulation: routes can be dispatched directly by tests.
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

struct HttpResponse {
  int code = 0;
  String contentType;
  String body;
  String location;
  std::map<String, String> headers;
};

class ESP8266WebServer {
 public:
  using Handler = std::function<void(void)>;
  explicit ESP8266WebServer(int port) : _port(port) {}

  void on(const char *path, uint8_t method, Handler fn) {
    _routes.push_back({path, method, fn});
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

  bool authenticate(const char *user, const char *password) {
    String expected = String(user) + ":" + String(password);
    String encoded = base64(expected);
    return _authHeader == ("Basic " + encoded);
  }
  void requestAuthentication() {
    _response.code = 401;
    _response.contentType = "text/plain";
    _response.body = "Unauthorized";
    _response.headers["WWW-Authenticate"] = "Basic realm=\"IHR IR Remote\"";
  }

  HttpResponse dispatch(uint8_t method, const char *path, const String &body = String(),
                        const String &authHeader = String()) {
    _uri = path;
    _body = body;
    _authHeader = authHeader;
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
    uint8_t method;
    Handler fn;
  };
  std::vector<Route> _routes;
  Handler _notFound;
  std::map<String, String> _args;
  int _port;
  bool _running = false;
  uint32_t _handled = 0;
  String _uri, _body, _authHeader, _methodName;
  size_t _expectedLength = 0;
  HttpResponse _response;

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
