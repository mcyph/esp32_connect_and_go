#include "AnalogReadBase.hpp"

class ExternalUnoAnalogRead: public AnalogReadBase {
    // TODO: Provide support for pins without using an expander under a common interface
    public:
        ExternalUnoAnalogRead(int logicalPinNumberStart): AnalogReadBase(logicalPinNumberStart, std::vector<int>{
            // TODO!
        }) {

        }

        int getValue(int pin) {
            return FIXME(pin);
        }
};
