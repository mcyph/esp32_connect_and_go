#include "AnalogReadBase.hpp"

class ExternalPiPicoAnalogRead: public AnalogReadBase {
    // TODO: Provide support for pins without using an expander under a common interface
    public:
        ExternalPiPicoAnalogRead(int logicalPinNumberStart): AnalogReadBase(logicalPinNumberStart, std::vector<int>{
            // TODO!
        }) {

        }

        int getValue(int pin) {
            return FIXME(pin);
        }
};
