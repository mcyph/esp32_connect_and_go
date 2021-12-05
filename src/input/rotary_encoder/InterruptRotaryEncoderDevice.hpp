#include "../../misc/Callback.hpp"
#include "RotaryEncoderDeviceBase.hpp"

class InterruptRotaryEncoderDevice: public RotaryEncoderDeviceBase {
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
    InterruptRotaryEncoderDevice(int pinA, int pinB): RotaryEncoderDeviceBase(pinA, pinB) {
      callback = makeCCallback(&InterruptRotaryEncoderDevice::onChange, this);
      attachInterrupt(digitalPinToInterrupt(pinA), callback, CHANGE);
      attachInterrupt(digitalPinToInterrupt(pinB), callback, CHANGE);
    }
    
    void poll() {
      // Implementation in `onChange`
    }

    int read() {
      return currentPos;
    }

    void write(int v) {
      currentPos = v;
    }
};
