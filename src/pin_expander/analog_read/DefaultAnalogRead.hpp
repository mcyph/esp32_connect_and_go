#include "AnalogReadBase.hpp"

#ifdef RP2040
ANALOG_PINS = {26, 27, 28};

class DefaultAnalogRead: AnalogReadBase {
    public:
        DefaultAnalogRead(int logicalPinNumberStart): AnalogReadBase(logicalPinNumberStart, vector<int>{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1024, 1024, 1024
        }) {}

        getValue(int pin) override {
            return analogRead(pin);
        }
};
