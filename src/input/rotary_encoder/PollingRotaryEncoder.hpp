#include <Arduino.h>
#include "RotaryEncoderDevice.hpp"

class PollingRotaryEncoder: public RotaryEncoderDevice {
  public:
    PollingRotaryEncoder(int pinA, int pinB): RotaryEncoderDevice(pinA, pinB) {}
    
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
