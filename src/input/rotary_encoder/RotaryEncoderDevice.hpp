#include <Arduino.h>
#include <functional>

class RotaryEncoderDevice {
  protected:
    int val;
    int pinA = 3;
    int pinB = 4;
    int currentPos = 0;
    int pinALastState = LOW;
    int n = LOW;
  
  public:
    RotaryEncoderDevice(int pinA, int pinB) {
      this->pinA = pinA;
      this->pinB = pinB;

      pinMode(pinA, INPUT);
      pinMode(pinB, INPUT);
      poll();
      write(0);
    }

    virtual void poll();
    virtual int read();
    virtual void write(int v);
};
