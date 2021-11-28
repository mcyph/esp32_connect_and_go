#include "../../misc/Callback.hpp"
#include "RotaryEncoderDevice.hpp"

class InterruptRotaryEncoderDevice: public RotaryEncoderDevice {
  private:
    voidCCallback callback;

    void onChange() {
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

  public:
    InterruptRotaryEncoderDevice(int pinA, int pinB): RotaryEncoderDevice(pinA, pinB) {
      callback = makeCCallback(&InterruptRotaryEncoderDevice::onChange, this);
      attachInterrupt(digitalPinToInterrupt(pinA), callback, CHANGE);
      attachInterrupt(digitalPinToInterrupt(pinB), callback, CHANGE);
    }
    
    void poll() {}

    int read() {
      return currentPos;
    }

    void write(int v) {
      currentPos = v;
    }
};
