#include "AnalogReadBase.hpp"

class ExternalSTM8AnalogRead: public AnalogReadBase {
    // TODO: Provide support for pins without using an expander under a common interface
    public:
        ExternalSTM8AnalogRead(int logicalPinNumberStart): AnalogReadBase(logicalPinNumberStart, std::vector<int>{
            // TODO!
        }) {

        }

        int getValue(int pin) {
            return FIXME(pin);
        }
};
