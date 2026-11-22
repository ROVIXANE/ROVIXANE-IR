/*@create-file:build.opt@
-D_IR_ENABLE_DEFAULT_=false
-DDECODE_NEC=true
-DSEND_NEC=true
-DDECODE_LG=true
-DSEND_LG=true
-DSEND_RAW=true
-DDECODE_SONY=true
-DSEND_SONY=true
-DDECODE_SAMSUNG=true
-DSEND_SAMSUNG=true
-DDECODE_JVC=true
-DSEND_JVC=true
-DDECODE_PANASONIC=true
-DSEND_PANASONIC=true
-DDECODE_RC5=true
-DSEND_RC5=true
-DDECODE_RC6=true
-DSEND_RC6=true
-DDECODE_DENON=true
-DSEND_DENON=true
-DDECODE_COOLIX=true
-DSEND_COOLIX=true
-DDECODE_MIDEA24=true
-DSEND_MIDEA24=true
-DDECODE_HASH=true
*/
#ifndef IHR_GLOBALS_H
#define IHR_GLOBALS_H

// This file is implicitly included in every translation unit by the ESP8266
// core (core 3.1+). It exists to keep IRremoteESP8266 small: only the TV
// protocols used by this project are compiled in. ArduinoDroid may ignore
// this mechanism; in that case the library defaults (all protocols) apply and
// the sketch still compiles - no code depends on a protocol being absent.

#endif
