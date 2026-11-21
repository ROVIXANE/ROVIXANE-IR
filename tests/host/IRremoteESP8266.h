// IRremoteESP8266 simulation for host tests (symbolic API only).
#pragma once
#include "Arduino.h"

#define _IR_ENABLE_DEFAULT_ true
#define DECODE_NEC true
#define SEND_NEC true
#define DECODE_LG true
#define SEND_LG true
#define DECODE_SONY true
#define SEND_SONY true
#define DECODE_SAMSUNG true
#define SEND_SAMSUNG true
#define DECODE_JVC true
#define SEND_JVC true
#define DECODE_PANASONIC true
#define SEND_PANASONIC true
#define DECODE_RC5 true
#define SEND_RC5 true
#define DECODE_RC6 true
#define SEND_RC6 true
#define DECODE_DENON true
#define SEND_DENON true
#define DECODE_COOLIX true
#define SEND_COOLIX true
#define DECODE_MIDEA24 true
#define SEND_MIDEA24 true
#define DECODE_HASH true
#define SEND_RAW true

enum decode_type_t {
  UNKNOWN = -1,
  UNUSED = 0,
  RC5, RC6, NEC, SONY, PANASONIC, JVC, SAMSUNG, WHYNTER, AIWA_RC_T501, LG,
  SANYO, MITSUBISHI, DISH, SHARP, COOLIX, DAIKIN, DENON, KELVINATOR, SHERWOOD,
  MITSUBISHI_AC, RCMM, SANYO_LC7461, MIDEA24, LG2
};
