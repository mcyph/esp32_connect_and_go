#include <Arduino.h>
#include "RotaryEncoderDeviceBase.hpp"

class PollingRotaryEncoder: public RotaryEncoderDeviceBase {
  public:
    PollingRotaryEncoder(int pinA, int pinB): RotaryEncoderDeviceBase(pinA, pinB) {}
    
    void poll() {
      n = digitalRead(pinA);
      if ((pinALastState == LOW) && (n == HIGH)) {
        if (digitalRead(pinB) == LOW) {
          currentPos++;
        } else {
          currentPos--;
        }
      }
      pinALastState = n;
    }

    int read() {
      return currentPos;
    }

    void write(int v) {
      currentPos = v;
    }
};
