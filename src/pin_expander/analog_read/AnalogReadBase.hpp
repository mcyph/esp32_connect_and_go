#include <Arduino.h>
#include <vector>

using std::vector;

class AnalogReadBase {
    private:
        int logicalPinNumberStart;
        vector<int> pinMaximums;
        vector<bool> pinLocked;

    public:
        AnalogReadBase(int logicalPinNumberStart, vector<int> pinMaximums) {
            this->logicalPinNumberStart = logicalPinNumberStart;
            this->pinMaximums = pinMaximums;
            this->pinLocked = vector<bool>();
        }

        //==============================================================
        // Pin Information
        //==============================================================

        int getPinMaximum(int pin) {
            return pinMaximums[pin];
        }

        int getLogicalPin(int pin) {
            return logicalPinNumberStart + pin;
        }

        int getActualPin(int logicalPin) {
            return logicalPin - logicalPinNumberStart;
        }

        //==============================================================
        // Value Reading
        //==============================================================

        virtual int getValue(int pin);

        float getFloat(int pin) {
            return getValue(pin) / float(pinMaximums[pin]);
        }

        double getDouble(int pin) {
            return getValue(pin) / double(pinMaximums[pin]);
        }

        unsigned char getPercent(int pin) {
            return getValue(pin) * 100 / pinMaximums[pin];
        }
};
