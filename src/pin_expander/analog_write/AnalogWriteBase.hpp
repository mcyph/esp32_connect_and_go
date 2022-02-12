#include <vector>
#include <Arduino.h>

using std::vector;

class AnalogWriteBase {
    private:
        int logicalPinNumberStart;
        vector<int> pinMaximums;

    public:
        AnalogWriteBase(int logicalPinNumberStart, vector<int> pinMaximums) {
            this->logicalPinNumberStart = logicalPinNumberStart;
            this->pinMaximums = pinMaximums;
        }

        int getPinMaximum(int pin) {
            return pinMaximums[pin];
        }

        virtual void setValue(int pin, int value);

        void setFloat(int pin, float value) {
            int maxValue = getPinMaximum(pin);
            setValue(pin, value * maxValue);
        }

        void setDouble(int pin, double value) {
            int maxValue = getPinMaximum(pin);
            setValue(pin, value * maxValue);
        }

        void setPercentage(int pin, int percentage) {
            long maxValue = getPinMaximum(pin);
            setValue(pin, long(percentage) * maxValue / 100L);
        }
};
